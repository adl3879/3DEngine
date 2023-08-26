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
                   LuaScriptComponent, TransformComponent, MeshComponent, CameraComponent,
                   // lights
                   PointLightComponent, DirectionalLightComponent, SpotLightComponent,
                   // physics
                   RigidBodyComponent, BoxColliderComponent>;

using AllComponentsExceptIDAndTag =
    ComponentExceptIDAndTagGroup<VisibilityComponent, LuaScriptComponent, TransformComponent, MeshComponent,
                                 CameraComponent,
                                 // lights
                                 PointLightComponent, DirectionalLightComponent, SpotLightComponent,
                                 // physics
                                 RigidBodyComponent, BoxColliderComponent>;
} // namespace Engine
