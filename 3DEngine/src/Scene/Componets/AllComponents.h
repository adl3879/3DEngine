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
    ComponentGroup<IDComponent, TagComponent, VisibilityComponent, TransformComponent, StaticMeshComponent, SkinnedMeshComponent, CameraComponent,
                   // lights
                   PointLightComponent, DirectionalLightComponent, SpotLightComponent,
                   // physics
                   RigidBodyComponent, BoxColliderComponent, SphereColliderComponent, ParentComponent,
                   NetScriptComponent, AnimationControllerComponent>;

using AllComponentsExceptIDAndTag =
    ComponentExceptIDAndTagGroup<VisibilityComponent, TransformComponent, StaticMeshComponent, SkinnedMeshComponent, CameraComponent,
                                 // lights
                                 PointLightComponent, DirectionalLightComponent, SpotLightComponent,
                                 // physics
                                 RigidBodyComponent, BoxColliderComponent, SphereColliderComponent, ParentComponent,
                                 NetScriptComponent, AnimationControllerComponent>;

using AllComponentsExceptIDAndTagAndParent =
    ComponentExceptIDAndTagGroup<VisibilityComponent, TransformComponent, StaticMeshComponent, SkinnedMeshComponent, CameraComponent,
                                 // lights
                                 PointLightComponent, DirectionalLightComponent, SpotLightComponent,
                                 // physics
                                 RigidBodyComponent, BoxColliderComponent, SphereColliderComponent, NetScriptComponent, AnimationControllerComponent>;
} // namespace Engine
