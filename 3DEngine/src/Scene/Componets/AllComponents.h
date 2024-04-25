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
                   PointLightComponent, DirectionalLightComponent, SpotLightComponent,
                   RigidBodyComponent, BoxColliderComponent, SphereColliderComponent, ParentComponent,
                   NetScriptComponent, AnimationControllerComponent, PrefabInstanceComponent>;

using AllComponentsExceptIDAndTag =
    ComponentExceptIDAndTagGroup<VisibilityComponent, TransformComponent, StaticMeshComponent, SkinnedMeshComponent, CameraComponent,
                                 PointLightComponent, DirectionalLightComponent, SpotLightComponent,
                                 RigidBodyComponent, BoxColliderComponent, SphereColliderComponent, ParentComponent,
                                 NetScriptComponent, AnimationControllerComponent, PrefabInstanceComponent>;

using AllComponentsExceptIDAndTagAndParent =
    ComponentExceptIDAndTagGroup<VisibilityComponent, TransformComponent, StaticMeshComponent, SkinnedMeshComponent, CameraComponent,
                                 PointLightComponent, DirectionalLightComponent, SpotLightComponent,
                                 RigidBodyComponent, BoxColliderComponent, SphereColliderComponent, NetScriptComponent, AnimationControllerComponent,
								 PrefabInstanceComponent>;
} // namespace Engine
