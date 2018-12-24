#include "ftMario.h"

//800E0960
void Mario_OnLoad(HSD_GObj* gobj){
    Player* player = GOBJ_PLAYER(gobj);
    u8* char_dat = player->x10C_char_dat;
    u8* char_dat_infile_x48 = &char_dat[0x48];
    char_dat = char_dat - 4;
    u8* player_afp = player->x2D8_player_article_floats - 8;
    player->x2224_flags = player->x2224_flags & 0xFE | 1;

    u32 i = 16;
    do {
        f32 data1 = ((f32*)char_dat)[2];
        char_dat = char_dat + 8; //Update offset by 8 bytes, so 4 bytes are skipped between each AFP
        f32 data2 =  ((f32*)char_dat)[1];
        player_afp[2] = data1;
        player_afp = player_afp + 8;
        player_afp[1] = data2;
        --i;
    } while(i > 0);
    player_afp[2] = ((f32*)char_dat)[2];
    player->x2D4_player_article_floats = player->x2D8_player_article_floats;
    u32 fireball_related = *((u32*)char_dat_infile_x48);
    sub_8026B3F8(fireball_related, 0x30); //Storing something about the fireball

    u32 unk = ((u32*)char_dat_infile_x48)[2];
    f32 afp_data = player->x2D4_player_article_floats[5];
    sub_8026B3F8(unk, afp_data);
}

//800E0DA8
void Mario_Special_Neutral_SetActionState_x157(HSD_GObj* gobj){
    Player* player = GOBJ_PLAYER(gobj);
    player->x2200_iasa_avail = FALSE;
    player->x2210_flags = 0;
    Player_ChangeActionState(gobj, MARIO_ACTIONSTATE_SPECIAL_NEUTRAL_158, 0, NULL, 0.0f, 1.0f, 0.0f);
    sub_8006EBA4(gobj);
    player->x21BC_Projectile_Spawn = Mario_SpawnProjectile;
}

//800E0E54
void Mario_Special_Neutral_IASA(HSD_GObj* gobj){
    Player* player = GOBJ_PLAYER(gobj);
    if(player->x2200_iasa_avail == TRUE){
        Interrupt_Wait(gobj);
    }
}

//800E0E84
void Mario_Special_Neutral_Physics(HSD_GObj* gobj){
    sub_80084F3C(gobj);
}

//800E0EA4
void Mario_Special_Neutral_CollInterrupt(HSD_GObj* gobj){
    if(Player_CopyCurrFramePhysicsToPrev(gobj) == TRUE){
        Mario_Special_Neutral_SetActionState_x158(gobj);
    }
}

//800E0EE0
void Mario_SpawnProjectile(HSD_GObj* gobj, guVector pos){
    Player* player = GOBJ_PLAYER(gobj);
    BOOL new_fireball = FALSE;
    if((player->x2210_flags >> 7) & 1){
        player->x2210_flags &= 0xFFFFFF7F;
        new_fireball = TRUE;
    }
    if(new_fireball){
        u32 index = Player_BoneID2Index(player, 0x17);
        HSD_JObj* spawn_bone = (HSD_JObj*)(*(player->x5E8_bone_lookup_table) + (index * BONE_LOOKUP_STRUCT_SIZE));
        HSD_JObj* jobj = JObj_Spawn(spawn_bone, NULL, pos);
        if(player->x4_internal_id == INTERNAL_MARIO){
            sub_8039B6F8(gobj, pos, ITEM_MARIO_FIREBALL, player->facedir);
            sub_8005FDDC(0x47A, gobj, spawn_bone, &player->facedir);
        }else{ //IF DR. MARIO
            u32 values[9];            
            u32 iterations = 9;
            u32 color_idx = 0;
            u32 num_values = 0;

            do {
                if(iterations != player->x222C_flags && iterations != player->x2230_flags){
                    values[color_idx] = color_idx;
                    num_values += 1;
                }
                color_idx += 1;
                iterations -= 1;
            } while (iterations != 0);
            player->x2230_flags = player->x222C_flags;
            u32 pill_color = values[HSD_Randi(num_values)];
            player->x222C_flags = pill_color;
            sub_802C0510(gobj, pos, pill_color, ITEM_DRMARIO_PILL, player->facedir);
        }
    }
}

//800E1040
void Mario_Special_Neutral_Air_SetActionState_x158(HSD_GObj* gobj){
    Player* player = GOBJ_PLAYER(gobj);
    player->x2200_iasa_avail = FALSE;
    player->x2210_flags = 0;
    Player_ChangeActionState(gobj, MARIO_ACTIONSTATE_SPECIAL_NEUTRAL_158, 0, NULL, 0.0f, 1.0f, 0.0f);
    sub_8006EBA4(gobj);
    player->x21BC_Projectile_Spawn = Mario_SpawnProjectile;
}

//800E1178
void Mario_Special_Neutral_SetActionState_x158(HSD_GObj* gobj){
    Player* player = GOBJ_PLAYER(gobj);
    Player_LoseGroundJump_ECBDisable(player);
    Player_ChangeActionState(gobj, MARIO_ACTIONSTATE_SPECIAL_NEUTRAL_158, 0x5000, NULL, player->x894_action_state_frames, 1.0f, 0.0f);
    player->x21BC_Projectile_Spawn = Mario_SpawnProjectile;
}

//800E11E0
void Mario_Special_Neutral_Air_SetActionState_x157(HSD_GObj* gobj){
    Player* player = GOBJ_PLAYER(gobj);
    sub_8007D7FC(player);
    Player_ChangeActionState(gobj, MARIO_ACTIONSTATE_SPECIAL_NEUTRAL_157, 0x5000, NULL, player->x894_action_state_frames, 1.0f, 0.0f);
    player->x21BC_Projectile_Spawn = Mario_SpawnProjectile;
}