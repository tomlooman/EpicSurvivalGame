// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"

// This is NOT included by default in an empty project! It's required for replication and setting of the GetLifetimeReplicatedProps
#include "Net/UnrealNetwork.h"

/** when you modify this, please note that this information can be saved with instances
* also DefaultEngine.ini [/Script/Engine.CollisionProfile] should match with this list **/
#define COLLISION_WEAPON				ECC_GameTraceChannel1


/** when you modify this, please note that this information can be saved with instances
* also DefaultEngine.ini [/Script/Engine.PhysicsSettings] should match with this list **/
#define SURFACE_DEFAULT				SurfaceType_Default
#define SURFACE_FLESH				SurfaceType1
#define SURFACE_ZOMBIE				SurfaceType2