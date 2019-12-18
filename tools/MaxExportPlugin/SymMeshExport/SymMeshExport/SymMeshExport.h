#pragma once

//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Includes for Plugins
// AUTHOR: 
//***************************************************************************/

#include "3dsmaxsdk_preinclude.h"
#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
//SIMPLE TYPE

#include "SYM_Mesh.h"

#include <Igame/Igame.h>
#include <Igame/IgameModifier.h>
#include <IGame/IConversionManager.h>

const float PI_OVER_ONE_EIGHTY = 0.0174532925f;
const float ONE_EIGHTY_OVER_PI = 57.29577951308f;

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

SYM::SYM_MESHSUBSET MaxToSymSubset(IGameNode *Node);
SYM::SYM_BONE MaxToSymBone(IGameNode *Node);
