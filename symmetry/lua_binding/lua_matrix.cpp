/*
** Lua binding: sym_matrix
** Generated automatically by tolua++-1.0.92 on Mon Jul 14 21:08:16 2008.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_sym_matrix_open (lua_State* tolua_S);

#include "matrix.h"

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_CMatrix4 (lua_State* tolua_S)
{
 CMatrix4* self = (CMatrix4*) tolua_tousertype(tolua_S,1,0);
	delete self;
	return 0;
}

static int tolua_collect_CVector3 (lua_State* tolua_S)
{
 CVector3* self = (CVector3*) tolua_tousertype(tolua_S,1,0);
	delete self;
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"CMatrix4");
 tolua_usertype(tolua_S,"CVector3");
}

/* method: new of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4_new00
static int tolua_sym_matrix_CMatrix4_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"CMatrix4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   CMatrix4* tolua_ret = (CMatrix4*)  new CMatrix4();
   tolua_pushusertype(tolua_S,(void*)tolua_ret,"CMatrix4");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4_new00_local
static int tolua_sym_matrix_CMatrix4_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"CMatrix4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   CMatrix4* tolua_ret = (CMatrix4*)  new CMatrix4();
   tolua_pushusertype_and_takeownership(tolua_S,(void *)tolua_ret,"CMatrix4");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4_new01
static int tolua_sym_matrix_CMatrix4_new01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"CMatrix4",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  float Matrix = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   CMatrix4* tolua_ret = (CMatrix4*)  new CMatrix4(&Matrix);
   tolua_pushusertype(tolua_S,(void*)tolua_ret,"CMatrix4");
   tolua_pushnumber(tolua_S,(lua_Number)Matrix);
  }
 }
 return 2;
tolua_lerror:
 return tolua_sym_matrix_CMatrix4_new00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4_new01_local
static int tolua_sym_matrix_CMatrix4_new01_local(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"CMatrix4",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  float Matrix = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   CMatrix4* tolua_ret = (CMatrix4*)  new CMatrix4(&Matrix);
   tolua_pushusertype_and_takeownership(tolua_S,(void *)tolua_ret,"CMatrix4");
   tolua_pushnumber(tolua_S,(lua_Number)Matrix);
  }
 }
 return 2;
tolua_lerror:
 return tolua_sym_matrix_CMatrix4_new00_local(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4_new02
static int tolua_sym_matrix_CMatrix4_new02(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"CMatrix4",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"const CMatrix4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  const CMatrix4* Mat = ((const CMatrix4*)  tolua_tousertype(tolua_S,2,0));
  {
   CMatrix4* tolua_ret = (CMatrix4*)  new CMatrix4(*Mat);
   tolua_pushusertype(tolua_S,(void*)tolua_ret,"CMatrix4");
  }
 }
 return 1;
tolua_lerror:
 return tolua_sym_matrix_CMatrix4_new01(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4_new02_local
static int tolua_sym_matrix_CMatrix4_new02_local(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"CMatrix4",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"const CMatrix4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  const CMatrix4* Mat = ((const CMatrix4*)  tolua_tousertype(tolua_S,2,0));
  {
   CMatrix4* tolua_ret = (CMatrix4*)  new CMatrix4(*Mat);
   tolua_pushusertype_and_takeownership(tolua_S,(void *)tolua_ret,"CMatrix4");
  }
 }
 return 1;
tolua_lerror:
 return tolua_sym_matrix_CMatrix4_new01_local(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4_delete00
static int tolua_sym_matrix_CMatrix4_delete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CMatrix4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CMatrix4* self = (CMatrix4*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'",NULL);
#endif
  delete self;
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Translate of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4_Translate00
static int tolua_sym_matrix_CMatrix4_Translate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CMatrix4",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"CVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CMatrix4* self = (CMatrix4*)  tolua_tousertype(tolua_S,1,0);
  CVector3 Pos = *((CVector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Translate'",NULL);
#endif
  {
   self->Translate(Pos);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Translate'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Rotate of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4_Rotate00
static int tolua_sym_matrix_CMatrix4_Rotate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CMatrix4",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"CVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CMatrix4* self = (CMatrix4*)  tolua_tousertype(tolua_S,1,0);
  float Angle = ((float)  tolua_tonumber(tolua_S,2,0));
  CVector3 Axis = *((CVector3*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Rotate'",NULL);
#endif
  {
   self->Rotate(Angle,Axis);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Rotate'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: TransformVector of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4_TransformVector00
static int tolua_sym_matrix_CMatrix4_TransformVector00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CMatrix4",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"CVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CMatrix4* self = (CMatrix4*)  tolua_tousertype(tolua_S,1,0);
  CVector3 Vector = *((CVector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'TransformVector'",NULL);
#endif
  {
   CVector3 tolua_ret = (CVector3)  self->TransformVector(Vector);
   {
#ifdef __cplusplus
    void* tolua_obj = new CVector3(tolua_ret);
    tolua_pushusertype_and_takeownership(tolua_S,tolua_obj,"CVector3");
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CVector3));
    tolua_pushusertype_and_takeownership(tolua_S,tolua_obj,"CVector3");
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'TransformVector'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RotateVector of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4_RotateVector00
static int tolua_sym_matrix_CMatrix4_RotateVector00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CMatrix4",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"CVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CMatrix4* self = (CMatrix4*)  tolua_tousertype(tolua_S,1,0);
  CVector3 Vector = *((CVector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RotateVector'",NULL);
#endif
  {
   CVector3 tolua_ret = (CVector3)  self->RotateVector(Vector);
   {
#ifdef __cplusplus
    void* tolua_obj = new CVector3(tolua_ret);
    tolua_pushusertype_and_takeownership(tolua_S,tolua_obj,"CVector3");
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CVector3));
    tolua_pushusertype_and_takeownership(tolua_S,tolua_obj,"CVector3");
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RotateVector'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: UnrotateVector of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4_UnrotateVector00
static int tolua_sym_matrix_CMatrix4_UnrotateVector00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CMatrix4",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"CVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CMatrix4* self = (CMatrix4*)  tolua_tousertype(tolua_S,1,0);
  CVector3 Vector = *((CVector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'UnrotateVector'",NULL);
#endif
  {
   CVector3 tolua_ret = (CVector3)  self->UnrotateVector(Vector);
   {
#ifdef __cplusplus
    void* tolua_obj = new CVector3(tolua_ret);
    tolua_pushusertype_and_takeownership(tolua_S,tolua_obj,"CVector3");
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CVector3));
    tolua_pushusertype_and_takeownership(tolua_S,tolua_obj,"CVector3");
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'UnrotateVector'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTranslation of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4_GetTranslation00
static int tolua_sym_matrix_CMatrix4_GetTranslation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CMatrix4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CMatrix4* self = (CMatrix4*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTranslation'",NULL);
#endif
  {
   CVector3 tolua_ret = (CVector3)  self->GetTranslation();
   {
#ifdef __cplusplus
    void* tolua_obj = new CVector3(tolua_ret);
    tolua_pushusertype_and_takeownership(tolua_S,tolua_obj,"CVector3");
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CVector3));
    tolua_pushusertype_and_takeownership(tolua_S,tolua_obj,"CVector3");
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTranslation'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Reset of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4_Reset00
static int tolua_sym_matrix_CMatrix4_Reset00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CMatrix4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CMatrix4* self = (CMatrix4*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Reset'",NULL);
#endif
  {
   self->Reset();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Reset'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Identity of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4_Identity00
static int tolua_sym_matrix_CMatrix4_Identity00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CMatrix4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CMatrix4* self = (CMatrix4*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Identity'",NULL);
#endif
  {
   self->Identity();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Identity'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Transpose of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4_Transpose00
static int tolua_sym_matrix_CMatrix4_Transpose00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CMatrix4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CMatrix4* self = (CMatrix4*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Transpose'",NULL);
#endif
  {
   self->Transpose();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Transpose'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInverse of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4_GetInverse00
static int tolua_sym_matrix_CMatrix4_GetInverse00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CMatrix4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CMatrix4* self = (CMatrix4*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetInverse'",NULL);
#endif
  {
   CMatrix4& tolua_ret = (CMatrix4&)  self->GetInverse();
   tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CMatrix4");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInverse'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator* of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4__mul00
static int tolua_sym_matrix_CMatrix4__mul00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CMatrix4",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"CMatrix4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CMatrix4* self = (CMatrix4*)  tolua_tousertype(tolua_S,1,0);
  CMatrix4 Mat = *((CMatrix4*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator*'",NULL);
#endif
  {
   CMatrix4 tolua_ret = (CMatrix4)  self->operator*(Mat);
   {
#ifdef __cplusplus
    void* tolua_obj = new CMatrix4(tolua_ret);
    tolua_pushusertype_and_takeownership(tolua_S,tolua_obj,"CMatrix4");
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CMatrix4));
    tolua_pushusertype_and_takeownership(tolua_S,tolua_obj,"CMatrix4");
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.mul'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4_get00
static int tolua_sym_matrix_CMatrix4_get00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const CMatrix4",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const CMatrix4* self = (const CMatrix4*)  tolua_tousertype(tolua_S,1,0);
  const int col = ((const int)  tolua_tonumber(tolua_S,2,0));
  const int row = ((const int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get'",NULL);
#endif
  {
   float tolua_ret = (float)  self->get(col,row);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_sym_matrix_CMatrix4_set00
static int tolua_sym_matrix_CMatrix4_set00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CMatrix4",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CMatrix4* self = (CMatrix4*)  tolua_tousertype(tolua_S,1,0);
  const int col = ((const int)  tolua_tonumber(tolua_S,2,0));
  const int row = ((const int)  tolua_tonumber(tolua_S,3,0));
  const float val = ((const float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set'",NULL);
#endif
  {
   self->set(col,row,val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: _Matrix of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_get_sym_matrix_CMatrix4__Matrix
static int tolua_get_sym_matrix_CMatrix4__Matrix(lua_State* tolua_S)
{
 int tolua_index;
  CMatrix4* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (CMatrix4*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
  tolua_Error tolua_err;
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 tolua_index = (int)tolua_tonumber(tolua_S,2,0);
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=16)
  tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->_Matrix[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: _Matrix of class  CMatrix4 */
#ifndef TOLUA_DISABLE_tolua_set_sym_matrix_CMatrix4__Matrix
static int tolua_set_sym_matrix_CMatrix4__Matrix(lua_State* tolua_S)
{
 int tolua_index;
  CMatrix4* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (CMatrix4*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
  tolua_Error tolua_err;
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 tolua_index = (int)tolua_tonumber(tolua_S,2,0);
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=16)
  tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->_Matrix[tolua_index] = ((float)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_sym_matrix_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"CMatrix4","CMatrix4","",tolua_collect_CMatrix4);
  #else
  tolua_cclass(tolua_S,"CMatrix4","CMatrix4","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"CMatrix4");
   tolua_function(tolua_S,"new",tolua_sym_matrix_CMatrix4_new00);
   tolua_function(tolua_S,"new_local",tolua_sym_matrix_CMatrix4_new00_local);
   tolua_function(tolua_S,".call",tolua_sym_matrix_CMatrix4_new00_local);
   tolua_function(tolua_S,"new",tolua_sym_matrix_CMatrix4_new01);
   tolua_function(tolua_S,"new_local",tolua_sym_matrix_CMatrix4_new01_local);
   tolua_function(tolua_S,".call",tolua_sym_matrix_CMatrix4_new01_local);
   tolua_function(tolua_S,"new",tolua_sym_matrix_CMatrix4_new02);
   tolua_function(tolua_S,"new_local",tolua_sym_matrix_CMatrix4_new02_local);
   tolua_function(tolua_S,".call",tolua_sym_matrix_CMatrix4_new02_local);
   tolua_function(tolua_S,"delete",tolua_sym_matrix_CMatrix4_delete00);
   tolua_function(tolua_S,"Translate",tolua_sym_matrix_CMatrix4_Translate00);
   tolua_function(tolua_S,"Rotate",tolua_sym_matrix_CMatrix4_Rotate00);
   tolua_function(tolua_S,"TransformVector",tolua_sym_matrix_CMatrix4_TransformVector00);
   tolua_function(tolua_S,"RotateVector",tolua_sym_matrix_CMatrix4_RotateVector00);
   tolua_function(tolua_S,"UnrotateVector",tolua_sym_matrix_CMatrix4_UnrotateVector00);
   tolua_function(tolua_S,"GetTranslation",tolua_sym_matrix_CMatrix4_GetTranslation00);
   tolua_function(tolua_S,"Reset",tolua_sym_matrix_CMatrix4_Reset00);
   tolua_function(tolua_S,"Identity",tolua_sym_matrix_CMatrix4_Identity00);
   tolua_function(tolua_S,"Transpose",tolua_sym_matrix_CMatrix4_Transpose00);
   tolua_function(tolua_S,"GetInverse",tolua_sym_matrix_CMatrix4_GetInverse00);
   tolua_function(tolua_S,".mul",tolua_sym_matrix_CMatrix4__mul00);
   tolua_function(tolua_S,"get",tolua_sym_matrix_CMatrix4_get00);
   tolua_function(tolua_S,"set",tolua_sym_matrix_CMatrix4_set00);
   tolua_array(tolua_S,"_Matrix",tolua_get_sym_matrix_CMatrix4__Matrix,tolua_set_sym_matrix_CMatrix4__Matrix);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_sym_matrix (lua_State* tolua_S) {
 return tolua_sym_matrix_open(tolua_S);
};
#endif

