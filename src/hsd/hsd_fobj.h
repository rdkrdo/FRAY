#ifndef _hsd_fobj_h_
#define _hsd_fobj_h_

#include <string.h>

#include <gctypes.h>
#include <ogc/gu.h>

#include "hsd_object.h"

#define HSD_A_OP_NONE 0
#define HSD_A_OP_CON 1
#define HSD_A_OP_LIN 2
#define HSD_A_OP_SPL0 3
#define HSD_A_OP_SPL 4
#define HSD_A_OP_SLP 5
#define HSD_A_OP_KEY 6

#define HSD_A_FRAC_FLOAT (0 << 5)
#define HSD_A_FRAC_S16 (1 << 5)
#define HSD_A_FRAC_U16 (2 << 5)
#define HSD_A_FRAC_S8 (3 << 5)
#define HSD_A_FRAC_U8 (4 << 5)

#define TYPE_ROBJ 1
#define TYPE_JOBJ 12

//Frame Object
typedef struct _HSD_FObj {
    struct _HSD_FObj* next; //0x00
    u8* ad; //0x04
    u8* ad_head; //0x08
    u32 length; //0x0C
    u8 flags; //0x10
    u8 op; //0x11
    u8 op_intrp; //0x12
    u8 obj_type; //0x13
    u8 frac_value; //0x14
    u8 frac_slope; //0x15
    u16 nb_pack; //0x16
    s16 startframe; //0x18
    u16 fterm; //0x1A
    f32 time; //0x1C
    f32 p0; //0x20
    f32 p1; //0x24
    f32 d0; //0x28
    f32 d1; //0x2C
} HSD_FObj;

typedef struct _HSD_FObjDesc {
    struct _HSD_FObjDesc* next; //0x00
    u32 length; //0x04
    f32 startframe; //0x08
    u8 type; //0x0C
    u8 frac_value; //0x0D
    u8 frac_slope; //0x0E
    u8 dummy0; //0x0F
    u8* ad; //0x10
} HSD_FObjDesc;

typedef struct _HSD_FObjData {
    f32 fv;
    s32 iv;
    guVector p;
} FObjData;

HSD_ObjAllocData* HSD_FObjGetAllocData(void);
void HSD_FObjInitAllocData(void);
void HSD_FObjRemove(HSD_FObj*);
void HSD_FObjRemoveAll(HSD_FObj*);
u8 HSD_FObjSetState(HSD_FObj*, u8);
u32 HSD_FObjGetState(HSD_FObj*);
void HSD_FObjReqAnimAll(HSD_FObj*, f32);
void HSD_FObjStopAnim(HSD_FObj*, void*, void (*)(), f32);
void HSD_FObjStopAnimAll(HSD_FObj*, void*, void (*)(), f32);
void FObjUpdateAnim(HSD_FObj*, void*, void (*)(void*, u32, FObjData));
void HSD_FObjInterpretAnim(HSD_FObj*, void*, void (*)(), f32);
void HSD_FObjInterpretAnimAll(HSD_FObj*, void*, void (*)(), f32);
HSD_FObj* HSD_FObjLoadDesc(HSD_FObjDesc*);
HSD_FObj* HSD_FObjAlloc(void);
void HSD_FObjFree(HSD_FObj*);

#endif
