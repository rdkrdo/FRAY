#include "archive.h"
#include "hsd/hsd_memory.h"

#include <malloc.h>

#include <di/di.h>

static s32 file_load_status = 0;

Archive_Allocation alloc_data[6];

//80015BD0
void* Archive_Alloc(u32 offset, u32 size){
    void* mem;
    u32 intr = IRQ_Disable();
    if(alloc_data[offset].x28_unk == 0){
        if(alloc_data[offset].x20_unk == 0){
            /* This is where the custom allocator would be useful
            OSHeapHandle heap_handle = HSD_GetHeap();
            HSD_SetHeap(alloc_data[offset].heap_handle);
            mem = HSD_MemAlloc(size);
            HSD_SetHeap(heap_handle);
            */
           mem = HSD_MemAlloc(size);
        }else{
            //mem = sub_80014FC8(alloc_data[offset].x14_unk, size);
            mem = HSD_MemAlloc(size);
            if(alloc_data[offset].x20_unk == 3){
                mem = (void*)((u32*)mem + 4);
            }
        }
    }else{
        mem = NULL;
    }
    IRQ_Restore(intr);
    return mem;
}

//800161A0
static s32 Archive_GetFileLoadStatus(){
    //sub_800195D0();
    return file_load_status;
}

//8001615C
static void Archive_DVDCallback(s32 result, dvdcmdblk* fileInfo){
    if(result >= 0){
        file_load_status = 1;
    }
    file_load_status = -1;
    HSD_Halt("DVDReadAsync failed");
}

//8001634C
u32 Archive_GetDVDFileLengthByEntry(s32 entry){
    u32 len;
    u32 intr = IRQ_Disable();
    dvdfileinfo handle;
    if(!DVDFastOpen(entry, &handle)){
        HSD_Halt("Archive_GetDVDFileLengthByEntry: Could not open file");
    }
    len = handle.len;
    IRQ_Restore(intr);
    return len;
}

//800163D8
u32 Archive_GetDVDFileLengthByName(char* filepath){
    u32 len = 0;
    /*char* file_path = Archive_PathFromFilename(filename);*/
    s32 entry = DVDConvertPathToEntrynum(filepath);
    if(entry == -1){
        HSD_Halt("Archive_GetDVDFileLengthByName: Could not locate file");
    }
    u32 intr = IRQ_Disable();
    dvdfileinfo handle;
    if(!DVDFastOpen(entry, &handle)){
        HSD_Halt("Archive_GetDVDFileLengthByName: Could not open file");
    }
    len = handle.len;
    IRQ_Restore(intr);
    return len;
}

//8001668C
void Archive_LoadFileIntoMemory(char* filepath, u8* mem, u32 filelength){
    s32 entry = DVDConvertPathToEntrynum(filepath);
    if(entry == -1){
        HSD_Halt("Archive_LoadFileIntoMemory: Could not locate file");
    }

    /* The below is unique to Fray in order to accomplish the file loading */
    dvdfileinfo handle;
    if(!DVDFastOpen(entry, &handle)){
        HSD_Halt("Archive_LoadFileIntoMemory: Could not open file");
    }
    dvdcmdblk cmdblk;
    u32 addr = handle.addr;
    DVD_ReadPrio(&cmdblk, mem, filelength, addr, 2);
}

//80016A54
void Archive_InitializeDAT(HSD_Archive* archive, u8* dat_file, u32 file_size){
    assert(HSD_ArchiveParse(archive, dat_file, file_size) != -1);
    u32 offset = 0;
    while(true){
        char* str = HSD_ArchiveGetExtern(archive, offset++);
        if ( str == NULL )
            break;
        HSD_ArchiveLocateExtern(archive, str, 0);
    }
}

//80016C64
void Archive_LoadFileSections(char* filename, u32 sections, ...){
    va_list ap;

    u32 file_size = Archive_GetDVDFileLengthByName(filename);
    u8* dat_file = memalign(32, file_size); //This (size + 0x1F) & 0xFFFFFFE0 aligns the file size along 0x20 sized boundaries, IE Anything from 0x74581 - 0x745A0 would become 0x745A0
    HSD_Archive* archive = (HSD_Archive*)malloc(sizeof(HSD_Archive));
    Archive_LoadFileIntoMemory(filename, dat_file, file_size);
    Archive_InitializeDAT(archive, dat_file, file_size);

    va_start(ap, sections);
    while(sections > 0) {
        void** file_ptr = va_arg(ap, void **);
        char* section_name = va_arg(ap, char *);
        *file_ptr = HSD_ArchiveGetPublicAddress(archive, section_name);
        if(*file_ptr == NULL){
            HSD_Halt("Could not find section");
        }
        sections -= 2;
    }
    va_end(ap);
}