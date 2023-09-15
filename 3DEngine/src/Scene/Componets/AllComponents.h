#pragma once

#include "Components.h"
#include "PhysicsComponents.h"

namespace Engine
{
template <typename... Component> struct ComponentGroup
{
};
template <typename... Component> struct ComponentExceptIDAndTagGroup
{
};

using AllComponents =
    ComponentGroup<IDComponent, TagComponent, VisibilityComponent,
                   // scripting
					ScriptComponent,	
                   TransformComponent, MeshComponent, CameraComponent,
                   // lights
                   PointLightComponent, DirectionalLightComponent, SpotLightComponent,
                   // physics
                   RigidBodyComponent, BoxColliderComponent, ParentComponent>;

using AllComponentsExceptIDAndTag =
    ComponentExceptIDAndTagGroup<VisibilityComponent, TransformComponent, MeshComponent,
								 // scripting	
								 ScriptComponent,
                                 CameraComponent,
                                 // lights
                                 PointLightComponent, DirectionalLightComponent, SpotLightComponent,
                                 // physics
                                 RigidBodyComponent, BoxColliderComponent, ParentComponent>;
} // namespace Engine
