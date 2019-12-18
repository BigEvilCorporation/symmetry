/*
** Lua binding: sym_vector
** Generated automatically by tolua++-1.0.92 on Mon Jul 14 21:08:18 2008.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_sym_vector_open (lua_State* tolua_S);

#include "vector.h"

/* function to release collected object via destructor */
#ifdef __cplusplus

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
 tolua_usertype(tolua_S,"CVector3");
}

/* get function: x of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_get_CVector3_x
static int tolua_get_CVector3_x(lua_State* tolua_S)
{
  CVector3* self = (CVector3*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'x'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->x);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: x of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_set_CVector3_x
static int tolua_set_CVector3_x(lua_State* tolua_S)
{
  CVector3* self = (CVector3*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'x'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->x = ((float)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: y of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_get_CVector3_y
static int tolua_get_CVector3_y(lua_State* tolua_S)
{
  CVector3* self = (CVector3*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'y'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->y);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: y of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_set_CVector3_y
static int tolua_set_CVector3_y(lua_State* tolua_S)
{
  CVector3* self = (CVector3*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'y'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->y = ((float)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: z of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_get_CVector3_z
static int tolua_get_CVector3_z(lua_State* tolua_S)
{
  CVector3* self = (CVector3*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'z'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->z);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: z of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_set_CVector3_z
static int tolua_set_CVector3_z(lua_State* tolua_S)
{
  CVector3* self = (CVector3*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'z'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->z = ((float)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_sym_vector_CVector3_new00
static int tolua_sym_vector_CVector3_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"CVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   CVector3* tolua_ret = (CVector3*)  new CVector3();
   tolua_pushusertype(tolua_S,(void*)tolua_ret,"CVector3");
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

/* method: new_local of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_sym_vector_CVector3_new00_local
static int tolua_sym_vector_CVector3_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"CVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   CVector3* tolua_ret = (CVector3*)  new CVector3();
   tolua_pushusertype_and_takeownership(tolua_S,(void *)tolua_ret,"CVector3");
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

/* method: new of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_sym_vector_CVector3_new01
static int tolua_sym_vector_CVector3_new01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"CVector3",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  float _x = ((float)  tolua_tonumber(tolua_S,2,0));
  float _y = ((float)  tolua_tonumber(tolua_S,3,0));
  float _z = ((float)  tolua_tonumber(tolua_S,4,0));
  {
   CVector3* tolua_ret = (CVector3*)  new CVector3(_x,_y,_z);
   tolua_pushusertype(tolua_S,(void*)tolua_ret,"CVector3");
  }
 }
 return 1;
tolua_lerror:
 return tolua_sym_vector_CVector3_new00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_sym_vector_CVector3_new01_local
static int tolua_sym_vector_CVector3_new01_local(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"CVector3",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  float _x = ((float)  tolua_tonumber(tolua_S,2,0));
  float _y = ((float)  tolua_tonumber(tolua_S,3,0));
  float _z = ((float)  tolua_tonumber(tolua_S,4,0));
  {
   CVector3* tolua_ret = (CVector3*)  new CVector3(_x,_y,_z);
   tolua_pushusertype_and_takeownership(tolua_S,(void *)tolua_ret,"CVector3");
  }
 }
 return 1;
tolua_lerror:
 return tolua_sym_vector_CVector3_new00_local(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_sym_vector_CVector3_new02
static int tolua_sym_vector_CVector3_new02(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"CVector3",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"const CVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  const CVector3* Vector = ((const CVector3*)  tolua_tousertype(tolua_S,2,0));
  {
   CVector3* tolua_ret = (CVector3*)  new CVector3(*Vector);
   tolua_pushusertype(tolua_S,(void*)tolua_ret,"CVector3");
  }
 }
 return 1;
tolua_lerror:
 return tolua_sym_vector_CVector3_new01(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_sym_vector_CVector3_new02_local
static int tolua_sym_vector_CVector3_new02_local(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"CVector3",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"const CVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  const CVector3* Vector = ((const CVector3*)  tolua_tousertype(tolua_S,2,0));
  {
   CVector3* tolua_ret = (CVector3*)  new CVector3(*Vector);
   tolua_pushusertype_and_takeownership(tolua_S,(void *)tolua_ret,"CVector3");
  }
 }
 return 1;
tolua_lerror:
 return tolua_sym_vector_CVector3_new01_local(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_sym_vector_CVector3_delete00
static int tolua_sym_vector_CVector3_delete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CVector3* self = (CVector3*)  tolua_tousertype(tolua_S,1,0);
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

/* method: Mul of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_sym_vector_CVector3_Mul00
static int tolua_sym_vector_CVector3_Mul00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CVector3",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"CVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CVector3* self = (CVector3*)  tolua_tousertype(tolua_S,1,0);
  CVector3 Vector = *((CVector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Mul'",NULL);
#endif
  {
   float tolua_ret = (float)  self->Mul(Vector);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Mul'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator* of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_sym_vector_CVector3__mul00
static int tolua_sym_vector_CVector3__mul00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CVector3",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CVector3* self = (CVector3*)  tolua_tousertype(tolua_S,1,0);
  float Scalar = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator*'",NULL);
#endif
  {
   CVector3 tolua_ret = (CVector3)  self->operator*(Scalar);
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
 tolua_error(tolua_S,"#ferror in function '.mul'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator/ of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_sym_vector_CVector3__div00
static int tolua_sym_vector_CVector3__div00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CVector3",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CVector3* self = (CVector3*)  tolua_tousertype(tolua_S,1,0);
  float Scalar = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator/'",NULL);
#endif
  {
   CVector3 tolua_ret = (CVector3)  self->operator/(Scalar);
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
 tolua_error(tolua_S,"#ferror in function '.div'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator* of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_sym_vector_CVector3__mul01
static int tolua_sym_vector_CVector3__mul01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CVector3",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"CVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  CVector3* self = (CVector3*)  tolua_tousertype(tolua_S,1,0);
  CVector3 Vector = *((CVector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator*'",NULL);
#endif
  {
   CVector3 tolua_ret = (CVector3)  self->operator*(Vector);
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
tolua_lerror:
 return tolua_sym_vector_CVector3__mul00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator- of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_sym_vector_CVector3__sub00
static int tolua_sym_vector_CVector3__sub00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CVector3",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"CVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CVector3* self = (CVector3*)  tolua_tousertype(tolua_S,1,0);
  CVector3 Vector = *((CVector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator-'",NULL);
#endif
  {
   CVector3 tolua_ret = (CVector3)  self->operator-(Vector);
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
 tolua_error(tolua_S,"#ferror in function '.sub'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator+ of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_sym_vector_CVector3__add00
static int tolua_sym_vector_CVector3__add00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CVector3",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"CVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CVector3* self = (CVector3*)  tolua_tousertype(tolua_S,1,0);
  CVector3 Vector = *((CVector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator+'",NULL);
#endif
  {
   CVector3 tolua_ret = (CVector3)  self->operator+(Vector);
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
 tolua_error(tolua_S,"#ferror in function '.add'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator== of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_sym_vector_CVector3__eq00
static int tolua_sym_vector_CVector3__eq00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CVector3",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"CVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CVector3* self = (CVector3*)  tolua_tousertype(tolua_S,1,0);
  CVector3 Vector = *((CVector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator=='",NULL);
#endif
  {
   bool tolua_ret = (bool)  self->operator==(Vector);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.eq'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Cross of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_sym_vector_CVector3_Cross00
static int tolua_sym_vector_CVector3_Cross00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CVector3",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"CVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CVector3* self = (CVector3*)  tolua_tousertype(tolua_S,1,0);
  CVector3* b = ((CVector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Cross'",NULL);
#endif
  {
   CVector3 tolua_ret = (CVector3)  self->Cross(*b);
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
 tolua_error(tolua_S,"#ferror in function 'Cross'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Dot of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_sym_vector_CVector3_Dot00
static int tolua_sym_vector_CVector3_Dot00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CVector3",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"CVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CVector3* self = (CVector3*)  tolua_tousertype(tolua_S,1,0);
  CVector3 Vector = *((CVector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Dot'",NULL);
#endif
  {
   float tolua_ret = (float)  self->Dot(Vector);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Dot'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Normalise of class  CVector3 */
#ifndef TOLUA_DISABLE_tolua_sym_vector_CVector3_Normalise00
static int tolua_sym_vector_CVector3_Normalise00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CVector3* self = (CVector3*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Normalise'",NULL);
#endif
  {
   self->Normalise();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Normalise'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_sym_vector_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"CVector3","CVector3","",tolua_collect_CVector3);
  #else
  tolua_cclass(tolua_S,"CVector3","CVector3","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"CVector3");
   tolua_variable(tolua_S,"x",tolua_get_CVector3_x,tolua_set_CVector3_x);
   tolua_variable(tolua_S,"y",tolua_get_CVector3_y,tolua_set_CVector3_y);
   tolua_variable(tolua_S,"z",tolua_get_CVector3_z,tolua_set_CVector3_z);
   tolua_function(tolua_S,"new",tolua_sym_vector_CVector3_new00);
   tolua_function(tolua_S,"new_local",tolua_sym_vector_CVector3_new00_local);
   tolua_function(tolua_S,".call",tolua_sym_vector_CVector3_new00_local);
   tolua_function(tolua_S,"new",tolua_sym_vector_CVector3_new01);
   tolua_function(tolua_S,"new_local",tolua_sym_vector_CVector3_new01_local);
   tolua_function(tolua_S,".call",tolua_sym_vector_CVector3_new01_local);
   tolua_function(tolua_S,"new",tolua_sym_vector_CVector3_new02);
   tolua_function(tolua_S,"new_local",tolua_sym_vector_CVector3_new02_local);
   tolua_function(tolua_S,".call",tolua_sym_vector_CVector3_new02_local);
   tolua_function(tolua_S,"delete",tolua_sym_vector_CVector3_delete00);
   tolua_function(tolua_S,"Mul",tolua_sym_vector_CVector3_Mul00);
   tolua_function(tolua_S,".mul",tolua_sym_vector_CVector3__mul00);
   tolua_function(tolua_S,".div",tolua_sym_vector_CVector3__div00);
   tolua_function(tolua_S,".mul",tolua_sym_vector_CVector3__mul01);
   tolua_function(tolua_S,".sub",tolua_sym_vector_CVector3__sub00);
   tolua_function(tolua_S,".add",tolua_sym_vector_CVector3__add00);
   tolua_function(tolua_S,".eq",tolua_sym_vector_CVector3__eq00);
   tolua_function(tolua_S,"Cross",tolua_sym_vector_CVector3_Cross00);
   tolua_function(tolua_S,"Dot",tolua_sym_vector_CVector3_Dot00);
   tolua_function(tolua_S,"Normalise",tolua_sym_vector_CVector3_Normalise00);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_sym_vector (lua_State* tolua_S) {
 return tolua_sym_vector_open(tolua_S);
};
#endif

