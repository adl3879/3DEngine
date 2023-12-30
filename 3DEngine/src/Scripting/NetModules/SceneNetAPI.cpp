#include "SceneNetAPI.h"

#include <Coral/NativeString.hpp>
#include "Components.h"
#include "PhysicsComponents.h"
#include "Entity.h"

#include "ScriptingEngineNet.h"

#include <Coral/NativeArray.h>

namespace Engine
{
enum ComponentTypes
{
    UNKNOWN = -1,
    TRANSFORM = 0,
    MESH,
    NET_SCRIPT,
    POINT_LIGHT,
    SPOT_LIGHT,
    DIRECTIONAL_LIGHT,
    CAMERA,
    BOX_COLLIDER,
    SPHERE_COLLIDER,
    CAPSULE_COLLIDER,
    RIGIDBODY,
};

bool EntityHasComponent(int entityID, int componentID)
{
    Entity entity(static_cast<entt::entity>(entityID), ScriptingEngineNet::Get().GetCurrentScene());

    if (componentID == TRANSFORM) return entity.HasComponent<TransformComponent>();
    if (componentID == MESH) return entity.HasComponent<MeshComponent>();
    if (componentID == NET_SCRIPT) return entity.HasComponent<NetScriptComponent>();
    if (componentID == POINT_LIGHT) return entity.HasComponent<PointLightComponent>();
    if (componentID == SPOT_LIGHT) return entity.HasComponent<SpotLightComponent>();
    if (componentID == DIRECTIONAL_LIGHT) return entity.HasComponent<DirectionalLightComponent>();
    if (componentID == CAMERA) return entity.HasComponent<CameraComponent>();
    if (componentID == BOX_COLLIDER) return entity.HasComponent<BoxColliderComponent>();
    if (componentID == SPHERE_COLLIDER) return entity.HasComponent<SphereColliderComponent>();
    if (componentID == CAPSULE_COLLIDER) return entity.HasComponent<CapsuleColliderComponent>();
    if (componentID == RIGIDBODY) return entity.HasComponent<RigidBodyComponent>();

    return false;
}

static void SetTranslation(int entityID, float x, float y, float z)
{
    Entity entity(static_cast<entt::entity>(entityID), ScriptingEngineNet::Get().GetCurrentScene());
    auto &transform = entity.GetComponent<TransformComponent>();
    transform.Translation = {x, y, z};
}

static Coral::NativeArray<float> GetTranslation(int entityID)
{
    Entity entity(static_cast<entt::entity>(entityID), ScriptingEngineNet::Get().GetCurrentScene());
    auto &transform = entity.GetComponent<TransformComponent>();
    return {transform.Translation.x, transform.Translation.y, transform.Translation.z};
}

static void SetRotation(int entityID, float x, float y, float z)
{
    Entity entity(static_cast<entt::entity>(entityID), ScriptingEngineNet::Get().GetCurrentScene());
    auto &transform = entity.GetComponent<TransformComponent>();
    transform.Rotation = {x, y, z};
}

static Coral::NativeArray<float> GetRotation(int entityID)
{
    Entity entity(static_cast<entt::entity>(entityID), ScriptingEngineNet::Get().GetCurrentScene());
    auto &transform = entity.GetComponent<TransformComponent>();
    return {transform.Rotation.x, transform.Rotation.y, transform.Rotation.z};
}

static void SetScale(int entityID, float x, float y, float z)
{
    Entity entity(static_cast<entt::entity>(entityID), ScriptingEngineNet::Get().GetCurrentScene());
    auto &transform = entity.GetComponent<TransformComponent>();
    transform.Scale = {x, y, z};
}

static Coral::NativeArray<float> GetScale(int entityID)
{
    Entity entity(static_cast<entt::entity>(entityID), ScriptingEngineNet::Get().GetCurrentScene());
    auto &transform = entity.GetComponent<TransformComponent>();
    return {transform.Scale.x, transform.Scale.y, transform.Scale.z};
}

static void Translate(int entityID, float x, float y, float z)
{
    Entity entity(static_cast<entt::entity>(entityID), ScriptingEngineNet::Get().GetCurrentScene());
    auto &transform = entity.GetComponent<TransformComponent>();
    transform.Translation.x += x;
    transform.Translation.y += y;
    transform.Translation.z += z;
}

static void Rotate(int entityID, float x, float y, float z)
{
    Entity entity(static_cast<entt::entity>(entityID), ScriptingEngineNet::Get().GetCurrentScene());
    auto &transform = entity.GetComponent<TransformComponent>();
    transform.Rotation.x += x;
    transform.Rotation.y += y;
    transform.Rotation.z += z;
}

static void Scale(int entityID, float x, float y, float z)
{
    Entity entity(static_cast<entt::entity>(entityID), ScriptingEngineNet::Get().GetCurrentScene());
    auto &transform = entity.GetComponent<TransformComponent>();
    transform.Scale.x *= x;
    transform.Scale.y *= y;
    transform.Scale.z *= z;
}

void SceneNetAPI::RegisterMethods()
{
    RegisterMethod("Entity.EntityHasComponentICall", reinterpret_cast<void *>(&EntityHasComponent));
    RegisterMethod("TransformComponent.SetTranslationICall", reinterpret_cast<void *>(&SetTranslation));
    RegisterMethod("TransformComponent.GetTranslationICall", reinterpret_cast<void *>(&GetTranslation));
    RegisterMethod("TransformComponent.SetRotationICall", reinterpret_cast<void *>(&SetRotation));
    RegisterMethod("TransformComponent.GetRotationICall", reinterpret_cast<void *>(&GetRotation));
    RegisterMethod("TransformComponent.SetScaleICall", reinterpret_cast<void *>(&SetScale));
    RegisterMethod("TransformComponent.GetScaleICall", reinterpret_cast<void *>(&GetScale));

    RegisterMethod("TransformComponent.TranslateICall", reinterpret_cast<void *>(&Translate));
    RegisterMethod("TransformComponent.RotateICall", reinterpret_cast<void *>(&Rotate));
    RegisterMethod("TransformComponent.ScaleICall", reinterpret_cast<void *>(&Scale));
}
} // namespace Engine