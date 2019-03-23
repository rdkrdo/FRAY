#include "hsd_jobj.h"

static void JObjInfoInit();

HSD_JObjInfo hsdJObj = { JObjInfoInit };

static HSD_JObjInfo* default_class = NULL;

static HSD_JObj *current_jobj = NULL; //r13_400C
static void (*callback_4010)(HSD_JObj*, f32); //r13_4010
static void (*callback_4014)(f32); //r13_4014
static void (*callback_4018)() = NULL; //r13_4018
static HSD_JObj *r13_401C = NULL; //r13_401C

//8036F934
void HSD_JObjReqAnim(HSD_JObj *jobj, f32 frame){
	if (jobj != NULL){
		HSD_AObjReqAnim(jobj->aobj, frame);
		if (union_type_dobj(jobj)){
			HSD_DObjReqAnimAllByFlags(jobj->u.dobj, 0x7FF, frame);
		}
		HSD_RObjReqAnimAllByFlags(jobj->robj, 0x7FF, frame);
	}
}

//8036F9B8
static void JObjSortAnim(HSD_AObj *aobj){
	if(aobj != NULL && aobj->fobj != NULL){
		for (HSD_FObj* i = aobj->fobj; ; i = i->next){
			HSD_FObj* n_fobj = i->next;
			if(i == NULL)
				break;
			if(n_fobj->obj_type == TYPE_JOBJ){ //This is the actual code, even if it's confusing
				i->next = n_fobj->next;
				n_fobj->next = aobj->fobj;
				aobj->fobj = n_fobj;
			}
		}
	}
}

//8036FDC0
void JObjUpdateFunc(HSD_JObj* jobj, u32 type, f32* fval){
	if(jobj != NULL){
		switch(type){
			case 1:
				if((jobj->flags & 0x200000) != 0){
					HSD_RObj* robj = HSD_RObjGetByType(jobj->robj, 0x40000000, 0);
					if(robj != NULL){
						robj->unk = *fval;
					}
				}
				assert(JOBJ_USE_QUATERNION(jobj));
				jobj->rotation.x = *fval;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 2:
				assert(JOBJ_USE_QUATERNION(jobj));
				jobj->rotation.y = *fval;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 3:
				assert(JOBJ_USE_QUATERNION(jobj));
				jobj->rotation.z = *fval;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 4:
				if(*fval < 0.f){
					*fval = 0.f;
				}
				if(1.875f < *fval){
					*fval = 1;
				}
				assert(jobj->aobj != NULL);
				HSD_JObj* jp = (HSD_JObj*)jobj->aobj->hsd_obj;
				assert(jp->u.spline != NULL);
				guVector result;
				splArcLengthPoint(jp->u.spline, &result, *fval);
				jobj->position = result;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 5:
				jobj->position.x = *fval;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 6:
				jobj->position.y = *fval;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 7:
				jobj->position.z = *fval;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 8:
				if((f32)((u32)*fval & 0x7FFFFFFF) < 0.001f){
					*fval = 0.001f;
				}
				jobj->scale.x = *fval;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 9:
				if((f32)((u32)*fval & 0x7FFFFFFF) < 0.001f){
					*fval = 0.001f;
				}
				jobj->scale.y = *fval;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 10:
				if((f32)((u32)*fval & 0x7FFFFFFF) < 0.001f){
					*fval = 0.001f;
				}
				jobj->scale.z = *fval;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 11:
				if (*fval <= 1.75f) {
					HSD_JObjSetFlags(jobj,0x10);
				}else {
					HSD_JObjClearFlags(jobj,0x10);
				}
				break;
			case 12:
				if(*fval <= 1.75f){
					HSD_JObjSetFlagsAll(jobj, 0x10);
				}else{
					HSD_JObjClearFlagsAll(jobj, 0x10);
				}
				break;
			case 20:
			case 21:
			case 22:
			case 23:
			case 24:
			case 25:
			case 26:
			case 27:
			case 28:
			case 29:
				/*void* jo = r13_401C;
				f32 val = 176.f;
				while(jo != NULL){
					u32 uval = (u32)*fval;
					(*jo->0x4)(jobj, type, uval);
					jo = jo->next; //0x00
				}*/
				break;
			case 30:
			case 31:
			case 32:
			case 33:
			case 34:
			case 35:
			case 36:
			case 37:
			case 38:
			case 39:
				/*void* jo = r13_401C;
				while(jo != NULL){
					(*jo->0x4)(jobj, type, *fval);
					jo = jo->next;
				}*/
				break;
			case 40:
				if(callback_4018 != NULL){
					(*callback_4018)(0, (u32)*fval & 0x3f, (u32)*fval >> 6 & 0xffffff, jobj); //8005DB70 during gameplay
				}
				break;
			case 41:
				if(callback_4014 != NULL){
					(*callback_4014)(*fval);
				}
				break;
			case 42:
				if(callback_4010 != NULL){
					(*callback_4010)(jobj, *fval);
				}
				break;
			case 50:
				jobj->mtx[0][0] = fval[0];
				jobj->mtx[1][0] = fval[1];
				jobj->mtx[2][0] = fval[2];
				break;
			case 51:
				jobj->mtx[0][1] = fval[0];
				jobj->mtx[1][1] = fval[1];
				jobj->mtx[2][1] = fval[2];
				break;
			case 52:
				jobj->mtx[0][2] = fval[0];
				jobj->mtx[1][2] = fval[1];
				jobj->mtx[2][2] = fval[2];
				break;
			case 53:
				jobj->mtx[0][3] = fval[0];
				jobj->mtx[1][3] = fval[1];
				jobj->mtx[2][3] = fval[2];
				break;
			case 54:
			case 55:
			case 56:
			case 57:
				Mtx mtx;
				if(jobj->parent == NULL){
					c_guMtxCopy(jobj->mtx, mtx);
				}else{ //InverseConcat
					c_guMtxConcat(jobj->parent->mtx, jobj->mtx, mtx);
					c_guMtxInverse(mtx, mtx);
				}
				if(type == 54 || type == 56){
					//HSD_MtxGetTranslate(mtx, &jobj->position);
					jobj->position.x = mtx[0][3];
					jobj->position.y = mtx[1][3];
					jobj->position.z = mtx[2][3];
				}
				if((type - 54) < 2){
					//HSD_MtxGetRotation(mtx, &jobj->rotation)
				}
				if((type == 54) || type == 57){
					//HSD_MtxGetScale(mtx, &jobj->scale);
					c_guMtxScale(mtx, jobj->scale.x, jobj->scale.y, jobj->scale.z);
				}
				break;
		}
	}
}

//80370780
void HSD_JObjAnim(HSD_JObj* jobj){
	if (jobj != NULL){
    	HSD_JObjCheckDepend(jobj);
    	HSD_AObjInterpretAnim(jobj->aobj, jobj, JObjUpdateFunc);
    	HSD_RObjAnimAll(jobj->robj);
    	if (union_type_dobj(jobj)){
			HSD_DObjAnimAll(jobj->u.dobj);
		}
	}
}

//803707F8
static void JObjAnimAll(HSD_JObj* jobj){
	if(jobj != NULL){
		HSD_JObjCheckDepend(jobj);
    	HSD_AObjInterpretAnim(jobj->aobj, jobj, JObjUpdateFunc);
    	HSD_RObjAnimAll(jobj->robj);
    	if(union_type_dobj(jobj)){
			HSD_DObjAnimAll(jobj->u.dobj);
		}
		if(JOBJ_INSTANCE(jobj)){
			for(HSD_JObj* i = jobj->child; i != NULL; i = i->next){
				JObjAnimAll(jobj);
			}
		}
	}
}

//80370928
void HSD_JObjAnimAll(HSD_JObj* jobj){
	if (jobj != NULL){
		HSD_AObjInitEndCallBack();
    	HSD_JObjCheckDepend(jobj);
    	HSD_AObjInterpretAnim(jobj->aobj, jobj, JObjUpdateFunc);
    	HSD_RObjAnimAll(jobj->robj);
    	if(union_type_dobj(jobj)){
			HSD_DObjAnimAll(jobj->u.dobj);
		}
		if(JOBJ_INSTANCE(jobj)){
			for(HSD_JObj* i = jobj->child; i != NULL; i = i->next){
				JObjAnimAll(jobj);
			}
		}
		HSD_AObjInvokeCallBacks();
	}
}

//80371590
void HSD_JObjRemoveAll(HSD_JObj *jobj){
	if(jobj != NULL){
		if(jobj->parent){
			HSD_JObj *prev = HSD_JObjGetPrev(jobj);
			if(prev != NULL){
				prev->next = NULL;
			}else{
				jobj->parent->next = NULL;
			}
		}
		HSD_JObj *curr = jobj;
		HSD_JObj *next = NULL;
		while(curr){
			next = curr->next;
			curr->parent = NULL;
			curr->next = NULL;
			
			if(curr != NULL){
				BOOL norefs = HSD_OBJ_NOREF == curr->class_parent.ref_count;
				if(norefs == FALSE){
					curr->class_parent.ref_count -= 1;
					norefs = curr->class_parent.ref_count == 0;
				}
				/*
				if(isMaxMinRefs){
					v21 = (_WORD *)((char *)v16 + 6);
					result = (char **)*((unsigned __int16 *)v16 + 3);
					if ( (signed int)result - 1 >= 0 ){
						*v21 = (_WORD)result + 1;
						assert(*v21); //HSD_OBJ(o)->ref_count_individual != 0
						result = curr;
						v23 = curr->flags;
						return result;
					}
					if(curr){
						result = curr;
						v22 = curr->parent;
					}
				}*/
			}
			curr = next;
		}
	}
}

//80371750
void RecalcParentTrspBits(HSD_JObj *jobj){
	if(jobj != NULL){
		for(HSD_JObj* i = jobj; i; i->next){
			u32 v2 = 0x8FFFFFFF;
			for(HSD_JObj* j = jobj->child; j->next; j = j->next){
				v2 |= (j->flags | (j->flags << 10)) & 0x70000000;
			}
			if(!(i->flags & ~v2))
				break;
			i->flags = i->flags & v2;
		}
	}
}

//803717A8
void HSD_JObjAddChild(HSD_JObj *jobj, HSD_JObj *child){
	if(jobj != NULL && child != NULL){
		if(!child->parent){ //child should be a orphan
			if(!child->next){ //child should not have siblings
				if(jobj->child){
					assert(JOBJ_INSTANCE(jobj)); //!(jobj->flags & JOBJ_INSTANCE)
					HSD_JObj* i;
					for(i = jobj->child; jobj->next; i = jobj->next){
						HSD_Halt("HSD_JObjAddChild: last != child");
					}
					i->next = child;
				}else{
					jobj->child = child;
				}
				child->next = jobj;
				u32 ori_flags = child->flags;
				u32 flags = (ori_flags | (ori_flags << 10)) & 0x70000000;
				for(HSD_JObj* senti = jobj; senti; senti->parent){
					ori_flags = senti->flags;
					if(!(flags & ~ori_flags))
						break;
					senti->flags = ori_flags | flags;
				}
			}
		}
	}
}

//803718F4
void HSD_JObjReparent(HSD_JObj *jobj, HSD_JObj *pjobj){
	if(jobj != NULL){
		if(jobj->parent){
			HSD_JObj* prchild = jobj->parent->child;
			if(prchild == jobj){
				prchild = jobj->next;
			}else{
				HSD_JObj* prev = HSD_JObjGetPrev(jobj);
				assert(prev);
				prev->next = jobj->next;
			}
			
			for(HSD_JObj* i = jobj->parent; i; i = i->next){
				prchild = i->child;
				u32 a3 = 0x8FFFFFFF;
				while(prchild){
					u32 flags = prchild->flags;
					prchild = prchild->next;
					a3 |= (flags | (flags << 10)) & 0x70000000;
				}
				if(!(i->flags & ~a3))
					break;
				i->flags = i->flags & a3;
			}
			jobj->parent = NULL;
		}
		jobj->next = NULL;
		HSD_JObjAddChild(pjobj, jobj);
	}
}

//80371A04
void HSD_JObjAddNext(HSD_JObj *jobj, HSD_JObj *next){
	HSD_JObj* curr;
	
	if(jobj != NULL && next != NULL){
		curr = jobj;
		if(jobj->parent != NULL){
			curr = jobj->parent->child;
			jobj->parent->child = NULL;
			jobj->flags = jobj->flags & 0x8FFFFFFF;
		}
		
		HSD_JObj* parent = jobj->parent;
		if(next != NULL){
			HSD_JObj* next_next = next->next;
			if(next->parent != NULL){
				HSD_JObj* next_p_c = next->parent->child;
				if(next_p_c == next){
					next->parent->child = next_next;
				}else{
					HSD_JObj* prev = HSD_JObjGetPrev(next);
					assert(prev != NULL);
					prev->next = next_next;
				}
				RecalcParentTrspBits(next->parent);
				next->parent = NULL;
			}
			next->next = NULL;
			HSD_JObjAddChild(parent, next);
		}

		if(next->child != NULL){
			for(HSD_JObj* i = next->child; i != NULL; i = i->next){
				i->next = curr;
			}
		}else{
			next->child = curr;
		}
		
		while(curr != NULL){
			curr->parent = next;
			u32 flags = (curr->flags | curr->flags << 10) & 0x70000000;
			HSD_JObj* i = next;
			while(i != NULL && (flags & ~i->flags) != 0){
				i->flags = i->flags | flags;
				i = i->parent;
			}
			curr = curr->next;
		}
	}
}

//80371B60
HSD_JObj* HSD_JObjGetPrev(HSD_JObj *jobj){	
	if(jobj != NULL){
		if(!jobj->parent)
			return NULL;

		if(jobj->parent->child == jobj)
			return NULL;
		
		for (HSD_JObj* i = jobj->parent->child; i; i = i->next){
			if(i->next == jobj){
				return i;
			}
		}
	}
	return NULL;
}

//80371BEC
HSD_DObj* HSD_JObjGetDObj(HSD_JObj *jobj){
	if(jobj != NULL && union_type_dobj(jobj)){
		return jobj->u.dobj;
	}
	return NULL;
}

//80371C24
void HSD_JObjAddDObj(HSD_JObj *jobj, HSD_DObj *dobj){
	if(jobj && dobj && union_type_dobj(jobj)){
		dobj->next = jobj->u.dobj;
		jobj->u.dobj = dobj;
	}
}

//80371C68
void HSD_JObjPrependRObj(HSD_JObj *jobj, HSD_RObj *robj){
	if(jobj && robj){
		robj->next = jobj->robj;
		jobj->robj = robj;
	}
}

//80371C98
void HSD_JObjDeleteRObj(HSD_JObj *jobj, HSD_RObj *robj){
	HSD_RObj **rp;
	if(jobj && robj){
		for(rp = &jobj->robj; *rp; &(*rp)->next){
			if(*rp == robj){
				*rp = robj->next;
				robj->next = NULL;
				return;
			}
		}
	}
}

//80371CE8
u32 HSD_JObjGetFlags(HSD_JObj *jobj){
	return (jobj) ? jobj->flags : 0;
}

//80371D00
bool HSD_JObjSetFlags(HSD_JObj *jobj, u32 flags){
	bool result = false;
	if(jobj){
		if((jobj->flags ^ flags) & 8){
			if(!(jobj->flags & 0x800000) && (jobj->flags & 0x40))
				result = true;
			if(!result)
				result = HSD_JObjSetMtxDirtySub(jobj);
		}
		jobj->flags |= flags;
	}
	return result;
}

//80371D9C
bool HSD_JObjSetFlagsAll(HSD_JObj *jobj, u32 flags){
	bool result = false;
	if(jobj){
		if((jobj->flags ^ flags) & 8){
			if(!(jobj->flags & 0x800000) && (jobj->flags & 0x40))
				result = true;
			if(!result)
				result = HSD_JObjSetMtxDirtySub(jobj);
		}
		jobj->flags |= flags;
		
		if(!(JOBJ_INSTANCE(jobj))){
			for(HSD_JObj* i = jobj->child; i; i = i->child){
				if(i){
					if((i->flags ^ flags) & 8){
						result = false;
						if(!(i->flags & 0x800000) && (i->flags & 0x40))
							result = true;
						if(!result)
							result = HSD_JObjSetMtxDirtySub(i);
					}
					i->flags |= flags;
					
					if(!(JOBJ_INSTANCE(i))){
						for (HSD_JObj* j = i->child; j; j = j->child )
							result = HSD_JObjSetFlagsAll(j, flags);
					}
				}
			}
		}
	}
}

//80371F00
bool HSD_JObjClearFlags(HSD_JObj *jobj, u32 flags){
	bool result = false;
	if(jobj){
		if((jobj->flags ^ flags) & 8){
			if(!(jobj->flags & 0x800000) && (jobj->flags & 0x40))
				result = true;
			if(!result)
				result = HSD_JObjSetMtxDirtySub(jobj);
		}
		jobj->flags &= ~flags;
	}
	return result;
}

//80371F9C
bool HSD_JObjClearFlagsAll(HSD_JObj *jobj, u32 flags){
	bool result = false;
	if(jobj){
		if((jobj->flags ^ flags) & 8){
			if(!(jobj->flags & 0x800000) && (jobj->flags & 0x40))
				result = true;
			if(!result)
				result = HSD_JObjSetMtxDirtySub(jobj);
		}
		jobj->flags &= ~flags;
		
		if(!(JOBJ_INSTANCE(jobj))){
			HSD_JObj *i;
			for(i = jobj->child; i; i = i->child){
				if(i){
					if((i->flags ^ flags) & 8){
						result = false;
						if(!(i->flags & 0x800000) && (i->flags & 0x40))
							result = true;
						if(!result)
							result = HSD_JObjSetMtxDirtySub(i);
					}
					i->flags &= ~flags;
					
					if(!(JOBJ_INSTANCE(i))){
						for (HSD_JObj* j = i->child; j; j = j->child )
							result = HSD_JObjClearFlagsAll(j, flags);
					}
				}
			}
		}
	}
	return result;
}

//8037210C
HSD_JObj* HSD_JObjAlloc(){
  HSD_JObj *jobj = hsdNew(HSD_JObjGetDefaultClass());
  assert(jobj);
  return jobj;
}

//80372168
void HSD_JObjSetCurrent(HSD_JObj *jobj){ //INCOMPLETE
	if(jobj)
		current_jobj = jobj;
}

//80372314
HSD_JObj* HSD_JObjGetCurrent(){
	return current_jobj;
}

//803732E8
BOOL HSD_JObjSetMtxDirtySub(HSD_JObj* jobj){
	BOOL isDirty = FALSE;
	jobj->flags = jobj->flags | 0x40;
	if((jobj->flags & 0x1000) == 0){
		HSD_JObj* child = jobj->child;
		while(child != NULL){
			if((child->flags & 0x1000000) == 0){
				isDirty = FALSE;
				if((child->flags & 0x800000) == 0 && (child->flags & 0x40) != 0){
					isDirty = TRUE;
				}
				if(isDirty == FALSE && child->flags & 0x1000 == 0){
					child->flags = child->flags | 0x40;

					HSD_JObj* child_child = child->child;
					while(child_child != NULL){
						if((child_child->flags & 0x1000000) == 0){
							isDirty = FALSE;
							if((child_child->flags & 0x800000) == 0 && (child_child->flags & 0x40) != 0){
								isDirty = TRUE;
							}
							if(isDirty == FALSE){
								HSD_JObjSetMtxDirtySub(child_child);
							}
						}
						child_child = child_child->next;
					}
				}
			}
			child = child->next;
		}
	}
	return isDirty;
}

//80373404
void HSD_JObjSetCallback(void* cb){
	callback_4018 = cb;
}

//8037340C
static void JObjInit(HSD_Class* o){
	HSD_PARENT_INFO(&hsdJObj)->init(o);

	if(o != NULL){
		HSD_JObj* jobj = HSD_JOBJ(o);
		jobj->flags = 0x40;
		jobj->scale.x = 1.0f;
		jobj->scale.y = 1.0f;
		jobj->scale.z = 1.0f;
	}
}

//803736F8
static void JObjRelease(HSD_Class* o){
	HSD_JObj* jobj = HSD_JOBJ(o);
	HSD_JOBJ_INFO(jobj->class_parent.class_init)->release_child(o); //Technically uses the class_parent from the jobj to get to this, so will probably need to account for that later
	if(HSD_IDGetDataFromTable(NULL, jobj->desc, NULL) == o){
		HSD_IDRemoveByIDFromTable(NULL, jobj->desc);
	}
	if(jobj->pvec != NULL){
		HSD_VecFree(jobj->pvec);
	}
	if(jobj->vmtx != NULL){
		HSD_MtxFree(jobj->vmtx);
	}
	HSD_PARENT_INFO(&hsdJObj)->release(o);
}

//80373790
static void JObjAmnesia(HSD_ClassInfo* info){
	if(info == HSD_CLASS_INFO(default_class)){
		default_class = NULL;
	}
	if(info == HSD_CLASS_INFO(&hsdJObj)){
		r13_401C = NULL;
		current_jobj = NULL;
	}
	HSD_PARENT_INFO(&hsdJObj)->amnesia(info);
}

//803737F4
static void JObjInfoInit(){
	hsdInitClassInfo(HSD_CLASS_INFO(&hsdJObj), HSD_CLASS_INFO(&hsdClass), HSD_BASE_CLASS_LIBRARY, "hsd_jobj", sizeof(HSD_JObjInfo), sizeof(HSD_JObj));
	HSD_CLASS_INFO(&hsdJObj)->init = JObjInit;
	HSD_CLASS_INFO(&hsdJObj)->release = JObjRelease;
	HSD_CLASS_INFO(&hsdJObj)->amnesia = JObjAmnesia;
	HSD_JOBJ_INFO(&hsdJObj)->disp = HSD_JObjDispSub;
	HSD_JOBJ_INFO(&hsdJObj)->make_pmtx = HSD_JObjMakeMatrix;
	HSD_JOBJ_INFO(&hsdJObj)->make_rmtx = mkRBillBoardMtx;
	HSD_JOBJ_INFO(&hsdJObj)->load = JObjLoad;
	HSD_JOBJ_INFO(&hsdJObj)->release_child = JObjReleaseChild;
}

//803743B8
static void HSD_JObjDispSub(HSD_JObj *jobj, MtxP vmtx, Mtx pmtx, HSD_TrspMask trsp_mask, u32 rendermode){
	HSD_JObjSetCurrent(jobj);
	if ((rendermode & 0x4000000) == 0 && (jobj->flags & 0x10000) != 0){
		HSD_LobjSetupSpecularInit(pmtx);
	}
	HSD_PObjClearMtxMark(0, 0);
	HSD_DObj* dobj = jobj->u.dobj;
	while(dobj != NULL){
		if((dobj->flags & 1) == 0 && (dobj->flags & trsp_mask << 1) != 0){
			HSD_DObjSetCurrent(dobj);
			//HSD_DOBJ_INFO(&dobj)->unk(dobj, vmtx, pmtx, rendermode);
		}
		dobj = dobj->next;
	}
	HSD_DObjSetCurrent(NULL);
	HSD_JObjSetCurrent(NULL);
}