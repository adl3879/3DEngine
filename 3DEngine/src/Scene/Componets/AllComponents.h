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
    ComponentGroup<IDComponent, TagComponent, VisibilityComponent, TransformComponent, MeshComponent, CameraComponent,
                   // lights
                   PointLightComponent, DirectionalLightComponent, SpotLightComponent,
                   // physics
                   RigidBodyComponent, BoxColliderComponent, SphereColliderComponent, ParentComponent,
                   NetScriptComponent>;

using AllComponentsExceptIDAndTag =
    ComponentExceptIDAndTagGroup<VisibilityComponent, TransformComponent, MeshComponent, CameraComponent,
                                 // lights
                                 PointLightComponent, DirectionalLightComponent, SpotLightComponent,
                                 // physics
                                 RigidBodyComponent, BoxColliderComponent, SphereColliderComponent, ParentComponent,
                                 NetScriptComponent>;

using AllComponentsExceptIDAndTagAndParent =
    ComponentExceptIDAndTagGroup<VisibilityComponent, TransformComponent, MeshComponent, CameraComponent,
                                 // lights
                                 PointLightComponent, DirectionalLightComponent, SpotLightComponent,
                                 // physics
                                 RigidBodyComponent, BoxColliderComponent, SphereColliderComponent, NetScriptComponent>;
} // namespace Engine
