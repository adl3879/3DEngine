#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include "Light.h"
#include "MaterialEditorPanel.h"
#include "ImGuiHelpers.h"
#include "PhysicsComponents.h"
#include "MeshImporter.h"
#include "ScriptEngine.h"

#include <IconsFontAwesome5.h>

#define ADD_COMPONENT_MENU(componentType, componentName)                                                               \
    if (!m_SelectionContext.HasComponent<componentType>())                                                             \
    {                                                                                                                  \
        if (ImGui::MenuItem(componentName))                                                                            \
        {                                                                                                              \
            m_SelectionContext.AddComponent<componentType>();                                                          \
            ImGui::CloseCurrentPopup();                                                                                \
        }                                                                                                              \
    }

#define CREATE_BUILTIN_MESH(name, filepath)                                                                            \
    entity = m_Context->CreateEntity(name);                                                                            \
    entity.AddComponent<MeshComponent>();                                                                              \
    auto &model = entity.GetComponent<MeshComponent>();                                                                \
    model.ModelResource = MeshImporter::LoadModel(filepath);                                                           \
    m_SelectionContext = entity;                                                                                       \
	return entity;                                                                                                     \

#define REMOVABLE_COMPONENT                                                                                            \
    if (ImGui::BeginPopupContextItem())                                                                                \
    {                                                                                                                  \
        if (ImGui::MenuItem("Remove")) removeComponent = true;                                                         \
        ImGui::EndPopup();                                                                                             \
    }

#define SET_SCRIPT_FIELD_VALUE(Type, PrimitiveType, Visual)															   \
	case ScriptFieldType::Type:																						   \
	{																												   \
		PrimitiveType data = scriptInstance->GetFieldValue<PrimitiveType>(name);									   \
		if (ImGui::Visual(_labelPrefix(name.c_str()), &data)) scriptInstance->SetFieldValue(name, data);			   \
		break;																										   \
	}																												   \

#define SET_SCRIPT_FIELD_VALUE_SCALAR(Type, PrimitiveType, Type2)													   \
	case ScriptFieldType::Type:																						   \
	{																												   \
		PrimitiveType data = scriptInstance->GetFieldValue<PrimitiveType>(name);									   \
		if (ImGui::DragScalar(_labelPrefix(name.c_str()), ImGuiDataType_##Type2, &data))						       \
			scriptInstance->SetFieldValue(name, data);																   \
		break;																										   \
	}																												   \

#define SET_SCRIPT_FIELD_VALUE_VECTOR(Type, PrimitiveType, Visual)													   \
	case ScriptFieldType::Type:																						   \
	{																												   \
		PrimitiveType data = scriptInstance->GetFieldValue<PrimitiveType>(name);									   \
		if (ImGui::Visual(_labelPrefix(name.c_str()), glm::value_ptr(data)))                                           \
			scriptInstance->SetFieldValue(name, data);                                                                 \
		break;																										   \
	}																												   \

#define SET_EDITOR_SCRIPT_FIELD_VALUE(Type, PrimitiveType, Visual)													   \
	case ScriptFieldType::Bool:                                                                                        \
	{                                                                                                                  \
		bool data = scriptField.GetValue<bool>();                                                                      \
		if (ImGui::Checkbox(_labelPrefix(name.c_str()), &data)) scriptField.SetValue(data);                            \
		break;                                                                                                         \
	}																											       \


namespace Engine
{
SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene> &context)
{
    SetContext(context);
}

void SceneHierarchyPanel::DrawEntityNode(Entity entity)
{
    const auto &tag = entity.GetComponent<TagComponent>().Tag;
	auto &parentComponent = entity.GetComponent<ParentComponent>();

	auto flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding |
                ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth | 
				ImGuiTreeNodeFlags_DefaultOpen;

    if (m_SelectionContext == entity) flags |= ImGuiTreeNodeFlags_Selected;
	if (parentComponent.Children.size() <= 0) flags |= ImGuiTreeNodeFlags_Leaf;

	std::string icon = "";
    if (entity.HasComponent<PointLightComponent>()) icon = ICON_FA_LIGHTBULB;
    else if (entity.HasComponent<SpotLightComponent>()) icon = ICON_FA_LIGHTBULB;
	else if (entity.HasComponent<DirectionalLightComponent>()) icon = ICON_FA_SUN;
	else if (entity.HasComponent<CameraComponent>()) icon = ICON_FA_VIDEO;
	else icon = ICON_FA_CUBE;
    icon.append("  ");

    bool open = ImGui::TreeNodeEx((void *)(uint64_t)(uint32_t)entity, flags, "%s", (icon + tag).c_str());
	// select when clicked
    if (ImGui::IsItemClicked()) m_SelectionContext = entity;

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
        ImGui::SetDragDropPayload("ENTITY", (void *)&entity, sizeof(Entity));
		ImGui::Text(ICON_FA_CUBE " %s", entity.GetComponent<TagComponent>().Tag.c_str());
        ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
		{
			Entity payloadEntity = *(const Entity *)payload->Data;
			ParentComponent &pc = payloadEntity.GetComponent<ParentComponent>(); 
            auto payloadId = payloadEntity.GetComponent<IDComponent>().ID;
		}
        ImGui::EndDragDropTarget();
	}

    bool entityDeleted = false;
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::BeginMenu(ICON_FA_PLUS "  Add Child"))
		{
            auto childEntity = CreateEntityPopup();
            if (childEntity)
            {
                entity.AddChild(childEntity);
                m_SelectionContext = childEntity;
            }

			ImGui::EndPopup();
		}
        ImGui::Separator();
		if (parentComponent.Children.empty() && ImGui::MenuItem("Duplicate Entity"))
        {
			auto newEntity = m_Context->DuplicateEntity(entity);
			m_SelectionContext = newEntity;
		}

		if (parentComponent.HasParent && ImGui::MenuItem("Move to root"))
		{
            if (parentComponent.HasParent)
            {
                auto p = m_Context->GetEntityByUUID(parentComponent.Parent);
                p.GetComponent<ParentComponent>().RemoveChild(entity.GetComponent<IDComponent>().ID);
				parentComponent.HasParent = false;
            }
		}
        if (ImGui::MenuItem("Delete Entity"))
        {
            m_SelectionContext = entity;
            entityDeleted = true;
        }
        ImGui::EndPopup();
    }

    if (entityDeleted)
    {
        if (entity.HasComponent<DirectionalLightComponent>())
        {
            auto &light = entity.GetComponent<DirectionalLightComponent>();
            m_Context->GetLights()->RemoveDirectionalLight();
        }
        // if entity is point light or spot light, remove it from light list
        if (entity.HasComponent<PointLightComponent>())
        {
            auto &pointLight = entity.GetComponent<PointLightComponent>();
            m_Context->GetLights()->RemovePointLight(pointLight.Index);
        }
        if (entity.HasComponent<SpotLightComponent>())
        {
            auto &spotLight = entity.GetComponent<SpotLightComponent>();
            m_Context->GetLights()->RemoveSpotLight(spotLight.Index);
        }

		// remove from parent, if it is a child component
		auto parent = entity.GetComponent<ParentComponent>();
		if (parent.HasParent)
		{
			auto &pc = m_Context->GetEntityByUUID(parent.Parent).GetComponent<ParentComponent>();
			pc.RemoveChild(entity.GetComponent<IDComponent>().ID);
		}

        if (m_SelectionContext == entity) m_SelectionContext = {};
		// delete entity recursively
        m_Context->DestroyEntityRecursive(entity);
    }

	if (open)
    {
        std::vector<UUID> children = parentComponent.Children;
        for (auto &child : children)
        {
			auto entity = m_Context->GetEntityByUUID(child);
            DrawEntityNode(entity);
        }
        ImGui::TreePop();
    };
}

void SceneHierarchyPanel::DrawComponents(Entity entity)
{
    if (m_SelectionContext.HasComponent<TagComponent>())
    {
        auto &tag = m_SelectionContext.GetComponent<TagComponent>().Tag;
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, tag.c_str());

        if (ImGui::InputText(_labelPrefix("Tag"), buffer, sizeof(buffer)))
        {
            tag = std::string(buffer);
        }
    }

    if (entity.HasComponent<TransformComponent>())
    {
        _collapsingHeaderStyle();
        if (ImGui::CollapsingHeader("Transform"))
        {
			auto &transform = entity.GetComponent<TransformComponent>();
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Reset Transform"))
				{
					transform.Translation = glm::vec3(0.0f);
					transform.Rotation = glm::vec3(0.0f);
					transform.Scale = glm::vec3(1.0f);
				}
				ImGui::EndPopup();
			}

            _drawVec3Control("Position", transform.Translation, 0.0f);
            _drawVec3Control("Rotation", transform.Rotation, 0.0f);
            _drawVec3Control("Scale", transform.Scale, 1.0f);
        }
    };

    if (entity.HasComponent<CameraComponent>())
    {
        bool removeComponent = false;
        _collapsingHeaderStyle();
        if (ImGui::CollapsingHeader("Camera"))
        {
            REMOVABLE_COMPONENT
            auto &cameraComponent = entity.GetComponent<CameraComponent>();
            auto &camera = cameraComponent.Camera;

            if (ImGui::Checkbox(_labelPrefix("Primary"), &cameraComponent.Primary))
            {
                auto view = m_Context->m_Registry.view<CameraComponent>();
                for (auto entity : view)
                {
                    auto &otherCameraComponent = view.get<CameraComponent>(entity);
                    if (&otherCameraComponent != &cameraComponent) otherCameraComponent.Primary = false;
                }
            }

            float perspectiveVerticalFOV = glm::degrees(camera->GetPerspectiveVerticalFOV());
            if (ImGui::DragFloat(_labelPrefix("Vertical FOV"), &perspectiveVerticalFOV))
                camera->SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFOV));

            float perspectiveNearClip = camera->GetPerspectiveNearClip();
            if (ImGui::DragFloat(_labelPrefix("Near Clip"), &perspectiveNearClip))
                camera->SetPerspectiveNearClip(perspectiveNearClip);

            float perspectiveFarClip = camera->GetPerspectiveFarClip();
            if (ImGui::DragFloat(_labelPrefix("Far Clip"), &perspectiveFarClip))
                camera->SetPerspectiveFarClip(perspectiveFarClip);
        }

        if (removeComponent) entity.RemoveComponent<CameraComponent>();
    }

	if (entity.HasComponent<MeshComponent>())
    {
        bool removeComponent = false;
        auto &entityComponent = entity.GetComponent<MeshComponent>();
        _collapsingHeaderStyle();
        if (ImGui::CollapsingHeader("Mesh"))
        {
            auto meshName = AssetManager::GetAssetName(entityComponent.Handle);
            REMOVABLE_COMPONENT
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
            ImGui::Button(_labelPrefix("Mesh", meshName.c_str()), ImVec2(-1, 0));
            ImGui::PopStyleVar();
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                {
                    const char *handle = (const char *)payload->Data;
                    entityComponent.Handle = std::stoull(handle);
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));

            auto modelEntity = entity.GetComponent<MeshComponent>();
            const auto &model = modelEntity.ModelResource ? modelEntity.ModelResource
                                                          : AssetManager::GetAsset<Model>(entityComponent.Handle);
            const auto &material = AssetManager::GetAsset<Material>(entityComponent.MaterialHandle);
            if (model != nullptr)
            {
                for (const auto &mesh : model->GetMeshes())
                {
                    bool openModal = false;
                    auto materialName = AssetManager::GetAssetName(entityComponent.MaterialHandle);
                    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
                    ImGui::Button(_labelPrefix("Material", materialName.c_str()), ImVec2(-1, 0));
                    ImGui::PopStyleVar();

                    if (ImGui::BeginPopupContextItem())
                    {
                        // TODO: check if payload is a handle or a path, if path, convert to handle by loading asset
                        if (ImGui::MenuItem(ICON_FA_TRASH_RESTORE "   Remove")) entityComponent.MaterialHandle = 0;
                        if (ImGui::MenuItem(ICON_FA_EDIT "   Open Material Editor"))
                        {
                            auto handle = entityComponent.MaterialHandle ? entityComponent.MaterialHandle
                                                                         : model->GetDefaultMaterialHandle();
                            auto isDefault = handle == model->GetDefaultMaterialHandle();
                            MaterialEditorPanel::OpenMaterialEditor(handle, isDefault);
                        }
                        ImGui::EndPopup();
                    }

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                        {
                            const char *handle = (const char *)payload->Data;
                            // convert handle to uint64_t(AssetHandle)
                            entityComponent.MaterialHandle = std::stoull(handle);
                        }
                        ImGui::EndDragDropTarget();
                    }
                }
            }

            if (removeComponent) entity.RemoveComponent<MeshComponent>();
            ImGui::PopStyleColor(2);
        }

        if (entity.HasComponent<VisibilityComponent>())
        {
            _collapsingHeaderStyle();
            if (ImGui::CollapsingHeader("Visibility"))
            {
                auto &entityComponent = entity.GetComponent<VisibilityComponent>();
                ImGui::Checkbox(_labelPrefix("Visibility"), &entityComponent.IsVisible);
            }
        }
    }

	if (entity.HasComponent<ScriptComponent>())
	{
		bool removeComponent = false;
		auto &scriptComponent = entity.GetComponent<ScriptComponent>();
        bool scriptClassExists = ScriptEngine::EntityClassExists(scriptComponent.ClassName);

		_collapsingHeaderStyle();
		if (ImGui::CollapsingHeader("Script"))
		{
            const auto& classes = ScriptEngine::GetEntityClasses();

			ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
            if (ImGui::Button(_labelPrefix("Script", scriptComponent.ClassName.c_str()), ImVec2(-1, 0)))
            {
                ImGui::OpenPopup("my_select_popup");
            }
			ImGui::PopStyleVar();

            if (ImGui::BeginPopup("my_select_popup"))
            {
                // search bar
                static char buf[128] = "";
                ImGui::InputTextWithHint("##Search", "Search", buf, IM_ARRAYSIZE(buf));

                ImGui::Separator();

                for (const auto &[className, classRef] : classes)
                {
					if (strlen(buf) > 0 && className.find(buf) == std::string::npos)
						continue;

                    if (ImGui::Selectable(className.c_str()))
                    {
                        scriptComponent.ClassName = className;
                    }
                }
                ImGui::EndPopup();
            }

			if (m_Context->IsPlaying())
            {
                ScriptInstanceRef scriptInstance =
                    ScriptEngine::GetEntityScriptInstance(entity.GetComponent<IDComponent>().ID);
                if (scriptInstance)
                {
                    // Fields
                    if (ImGui::TreeNodeEx((void *)typeid(ScriptComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
                                          "Fields"))
                    {
                        const auto &fields = scriptInstance->GetScriptClass()->GetFields();
                        for (const auto &[name, field] : fields)
                        {
                            switch (field.Type)
                            {
                                SET_SCRIPT_FIELD_VALUE(Bool, bool, Checkbox);
								SET_SCRIPT_FIELD_VALUE_SCALAR(Double, double, Double);
								SET_SCRIPT_FIELD_VALUE_SCALAR(Float, float, Float);
								SET_SCRIPT_FIELD_VALUE_SCALAR(Byte, uint8_t, U8);
								SET_SCRIPT_FIELD_VALUE_SCALAR(Int, int32_t, S32);
								SET_SCRIPT_FIELD_VALUE_SCALAR(UInt, uint32_t, U32);
								SET_SCRIPT_FIELD_VALUE_SCALAR(Long, int64_t, S64);
								SET_SCRIPT_FIELD_VALUE_SCALAR(ULong, uint64_t, U64);
								SET_SCRIPT_FIELD_VALUE_SCALAR(Short, int16_t, S16);
								SET_SCRIPT_FIELD_VALUE_SCALAR(UShort, uint16_t, U16);
								SET_SCRIPT_FIELD_VALUE_SCALAR(Char, char, S8);
								SET_SCRIPT_FIELD_VALUE_VECTOR(Vector2, glm::vec2, DragFloat2);
								SET_SCRIPT_FIELD_VALUE_VECTOR(Vector3, glm::vec3, DragFloat3);
                                SET_SCRIPT_FIELD_VALUE_VECTOR(Vector4, glm::vec4, DragFloat4);
                            }
                        }
                        ImGui::TreePop();
                    }
                }
            }
			else
			{
                if (scriptClassExists)
                {
                    if (ImGui::TreeNodeEx((void *)typeid(ScriptComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
                                          "Fields"))
                    {
                        ScriptClassRef entityClass = ScriptEngine::GetEntityClass(scriptComponent.ClassName);
                        const auto &fields = entityClass->GetFields();

                        auto &entityFields = ScriptEngine::GetScriptFieldMap(entity);
                        for (const auto &[name, field] : fields)
                        {
                            // Field has been set in editor
                            if (entityFields.find(name) != entityFields.end())
                            {
                                ScriptFieldInstance &scriptField = entityFields.at(name);

                                // Display control to set it maybe
								switch (field.Type)
								{
									case ScriptFieldType::Bool:
									{
										bool data = scriptField.GetValue<bool>();
										if (ImGui::Checkbox(_labelPrefix(name.c_str()), &data))
											scriptField.SetValue(data);
										break;
									}
									case ScriptFieldType::Float:
									{
										float data = scriptField.GetValue<float>();
										if (ImGui::DragFloat(_labelPrefix(name.c_str()), &data))
											scriptField.SetValue(data);
										break;
									}
									case ScriptFieldType::Int:
									{
										int32_t data = scriptField.GetValue<int32_t>();
										if (ImGui::DragInt(_labelPrefix(name.c_str()), &data))
											scriptField.SetValue(data);
										break;
									}
									case ScriptFieldType::Vector2:
									{
										glm::vec2 data = scriptField.GetValue<glm::vec2>();
										if (ImGui::DragFloat2(_labelPrefix(name.c_str()), glm::value_ptr(data)))
											scriptField.SetValue(data);
										break;
									}
									case ScriptFieldType::Vector3:
									{
										glm::vec3 data = scriptField.GetValue<glm::vec3>();
										if (ImGui::DragFloat3(_labelPrefix(name.c_str()), glm::value_ptr(data)))
											scriptField.SetValue(data);
										break;
									}
								}
                            }
                            else
                            {
								switch (field.Type)
								{
									case ScriptFieldType::Bool:
									{
										bool data = false;
										if (ImGui::Checkbox(_labelPrefix(name.c_str()), &data))
										{
											ScriptFieldInstance &fieldInstance = entityFields[name];
											fieldInstance.Field = field;
											fieldInstance.SetValue(data);
										}
										break;
									}
									case ScriptFieldType::Float:
									{
										float data = 0.0f;
										if (ImGui::DragFloat(_labelPrefix(name.c_str()), &data))
										{
											ScriptFieldInstance &fieldInstance = entityFields[name];
											fieldInstance.Field = field;
											fieldInstance.SetValue(data);
										}
										break;
									}
									case ScriptFieldType::Int:
									{
										int32_t data = 0;
										if (ImGui::DragInt(_labelPrefix(name.c_str()), &data))
										{
											ScriptFieldInstance &fieldInstance = entityFields[name];
											fieldInstance.Field = field;
											fieldInstance.SetValue(data);
										}
										break;
									}
									case ScriptFieldType::Vector2:
									{
										glm::vec2 data = glm::vec2(0.0f);
										if (ImGui::DragFloat2(_labelPrefix(name.c_str()), glm::value_ptr(data)))
										{
											ScriptFieldInstance &fieldInstance = entityFields[name];
											fieldInstance.Field = field;
											fieldInstance.SetValue(data);
										}
										break;
									}
									case ScriptFieldType::Vector3:
									{
										glm::vec3 data = glm::vec3(0.0f);
										if (ImGui::DragFloat3(_labelPrefix(name.c_str()), glm::value_ptr(data)))
										{
											ScriptFieldInstance &fieldInstance = entityFields[name];
											fieldInstance.Field = field;
											fieldInstance.SetValue(data);
										}
										break;
									}
								}
                            }
                        }
                        ImGui::TreePop();
                    }
                }
			}
		}
	}

    if (entity.HasComponent<DirectionalLightComponent>())
    {
        bool removeComponent = false;
        _collapsingHeaderStyle();
        if (ImGui::CollapsingHeader("DirectionalLight"))
        {
            REMOVABLE_COMPONENT
            auto &entityComponent = entity.GetComponent<DirectionalLightComponent>();
            auto &transform = entity.GetComponent<TransformComponent>();
            entityComponent.Light.Direction = transform.Rotation;
            ImGui::Checkbox(_labelPrefix("Enabled"), &entityComponent.Enabled);
            ImGui::ColorEdit3(_labelPrefix("Color"), glm::value_ptr(entityComponent.Light.Color));
            ImGui::DragFloat(_labelPrefix("Intensity"), &entityComponent.Light.Intensity, 0.1f, 0.0f, 10000.0f);
        }
        if (removeComponent) entity.RemoveComponent<DirectionalLightComponent>();
    }

    if (entity.HasComponent<PointLightComponent>())
    {
        bool removeComponent = false;
        _collapsingHeaderStyle();
        if (ImGui::CollapsingHeader("PointLight"))
        {
            REMOVABLE_COMPONENT
            auto &entityComponent = entity.GetComponent<PointLightComponent>();
            auto &transform = entity.GetComponent<TransformComponent>();
            entityComponent.Light.Position = transform.Translation;
            ImGui::Checkbox(_labelPrefix("Enabled"), &entityComponent.Enabled);
            ImGui::ColorEdit3(_labelPrefix("Color"), glm::value_ptr(entityComponent.Light.Color));
            ImGui::DragFloat(_labelPrefix("Intensity"), &entityComponent.Light.Intensity, 0.1f, 0.0f, 10000.0f);
        }
        if (removeComponent) entity.RemoveComponent<PointLightComponent>();
    }

    if (entity.HasComponent<SpotLightComponent>())
    {
        bool removeComponent = false;
        _collapsingHeaderStyle();
        if (ImGui::CollapsingHeader("SpotLight"))
        {
            REMOVABLE_COMPONENT
            auto &entityComponent = entity.GetComponent<SpotLightComponent>();
            auto &transform = entity.GetComponent<TransformComponent>();
            entityComponent.Light.Position = transform.Translation;
            entityComponent.Light.Direction = transform.Rotation;
            ImGui::Checkbox(_labelPrefix("Enabled"), &entityComponent.Enabled);
            ImGui::ColorEdit3(_labelPrefix("Color"), glm::value_ptr(entityComponent.Light.Color));
            ImGui::DragFloat(_labelPrefix("Cutoff"), &entityComponent.Light.Cutoff, 0.1f, 0.0f, 90.0f);
            ImGui::DragFloat(_labelPrefix("Outer Cutoff"), &entityComponent.Light.OuterCutoff, 0.1f, 0.0f,
                             90.0f);
            ImGui::DragFloat(_labelPrefix("Intensity"), &entityComponent.Light.Intensity, 0.1f, 0.0f, 10000.0f);
        }
        if (removeComponent) entity.RemoveComponent<SpotLightComponent>();
    }

    // Physics Stuff
    if (entity.HasComponent<RigidBodyComponent>())
    {
        bool removeComponent = false;
        _collapsingHeaderStyle();
        if (ImGui::CollapsingHeader("Rigid Body"))
        {
            REMOVABLE_COMPONENT
            // drop down menu for motion type
            auto &entityComponent = entity.GetComponent<RigidBodyComponent>();
            auto &transform = entity.GetComponent<TransformComponent>();
            if (ImGui::BeginCombo(_labelPrefix("Motion Type"),
                                  Physics::MotionTypeToString(entityComponent.MotionType).c_str()))
            {
                for (int i = 0; i < 2; i++)
                {
                    bool isSelected = Physics::MotionTypeToString(entityComponent.MotionType) ==
                                      Physics::MotionTypeToString((Physics::MotionType)i);
                    if (ImGui::Selectable(Physics::MotionTypeToString((Physics::MotionType)i).c_str(), isSelected))
                    {
                        entityComponent.MotionType = (Physics::MotionType)i;
                    }
                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            if (entityComponent.MotionType == Physics::MotionType::Dynamic)
            {
                ImGui::DragFloat(_labelPrefix("Mass"), &entityComponent.Mass, 0.1f, 0.0f, 10000.0f);
                ImGui::DragFloat(_labelPrefix("Linear Damping"), &entityComponent.LinearDamping, 0.01f, 0.0f,
                                 1.0f);
                ImGui::DragFloat(_labelPrefix("Angular Damping"), &entityComponent.AngularDamping, 0.01f, 0.0f,
                                 1.0f);
                ImGui::Checkbox(_labelPrefix("Use Gravity"), &entityComponent.UseGravity);
                ImGui::Checkbox(_labelPrefix("Is Kinematic"), &entityComponent.IsKinematic);
                // constraints tree node
                if (ImGui::TreeNodeEx((void *)typeid(RigidBodyComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
                                      "Constraints"))
                {
                    ImGui::TreePop();
                }
            }
        }
        if (removeComponent) entity.RemoveComponent<RigidBodyComponent>();
    }

    if (entity.HasComponent<BoxColliderComponent>())
    {
        bool removeComponent = false;
        _collapsingHeaderStyle();
        if (ImGui::CollapsingHeader("Box Collider"))
        {
            REMOVABLE_COMPONENT
            auto &entityComponent = entity.GetComponent<BoxColliderComponent>();
            _drawVec3Control("Size", entityComponent.Size, 0.0f);
            ImGui::Checkbox(_labelPrefix("Is Trigger"), &entityComponent.IsTrigger);
        }
        if (removeComponent) entity.RemoveComponent<BoxColliderComponent>();
    }
}

Entity SceneHierarchyPanel::CreateEntityPopup() 
{
    Entity entity;
    if (ImGui::MenuItem(ICON_FA_CUBE "   Create Empty Entity"))
    {
        entity = m_Context->CreateEntity("Empty Entity");
        m_SelectionContext = entity;
        return entity;
    }

	ImGui::Spacing();
    ImGui::Separator();
   
    if (ImGui::MenuItem(ICON_FA_VIDEO "  Camera"))
    {
        entity = m_Context->CreateEntity("Camera");
        entity.AddComponent<CameraComponent>();
        m_SelectionContext = entity;
        return entity;
    }
    ImGui::Spacing();
    ImGui::Separator();

    // check if scene does not contain directional light entity
    auto dLight = m_Context->GetEntity("Directional Light");
    if (dLight == nullptr)
    {
        if (ImGui::MenuItem(ICON_FA_SUN "  Directional Light"))
        {
            entity = m_Context->CreateEntity("Directional Light");
            entity.AddComponent<DirectionalLightComponent>();
            m_SelectionContext = entity;
            return entity;
        }
    }
    if (ImGui::MenuItem(ICON_FA_LIGHTBULB "  Point Light"))
    {
        auto num = m_Context->GetLights()->GetNumPointLights() + 1;
        entity = m_Context->CreateEntity("Point Light " + std::to_string(num));
        entity.AddComponent<PointLightComponent>();
        m_SelectionContext = entity;
        return entity;
    }
    if (ImGui::MenuItem(ICON_FA_LIGHTBULB "  Spot Light"))
    {
        auto num = m_Context->GetLights()->GetNumSpotLights() + 1;
        entity = m_Context->CreateEntity("Spot Light " + std::to_string(num));
        entity.AddComponent<SpotLightComponent>();
        m_SelectionContext = entity;
        return entity;
    }
    ImGui::Spacing();
    ImGui::Separator();

    if (ImGui::BeginMenu(ICON_FA_CUBE "  Mesh"))
    {
        if (ImGui::MenuItem(ICON_FA_CUBE "  Empty Mesh"))
        {
            entity = m_Context->CreateEntity("Mesh");
            entity.AddComponent<MeshComponent>();
            m_SelectionContext = entity;
            ImGui::EndPopup();
            return entity;
        }
        if (ImGui::MenuItem(ICON_FA_SQUARE "  Cube"))
        {
            CREATE_BUILTIN_MESH("Cube", "/Resources/Models/Cube/scene.gltf");
            ImGui::EndPopup();
        }
        if (ImGui::MenuItem(ICON_FA_CIRCLE "  Sphere"))
        {
            CREATE_BUILTIN_MESH("Cube", "/Resources/Models/Sphere/untitled.fbx");
            ImGui::EndPopup();
        }
        if (ImGui::MenuItem(ICON_FA_CAPSULES "  Capsule"))
        {
            CREATE_BUILTIN_MESH("Cube", "/Resources/Models/Capsule/untitled.fbx");
			ImGui::EndPopup();
        }
        ImGui::EndPopup();
    }

	return Entity{entt::null, m_Context.get()};
}

void SceneHierarchyPanel::SetContext(const std::shared_ptr<Scene> &context)
{
    m_Context = context;
    m_SelectionContext = {};
}

void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
{
    m_SelectionContext = entity;
    m_Context->SetSelectedEntity(entity);
}

void SceneHierarchyPanel::OnImGuiRender()
{
    ImGui::Begin("Scene Hierarchy");
    if (m_Context)
    {
        // Right-click on blank space
        if (ImGui::BeginPopupContextWindow())
        {
            CreateEntityPopup();
            ImGui::EndPopup();
        }

        // Add padding around the list
		m_Context->m_Registry.each(
        [this](auto entityId)
        {
            Entity entity{entityId, m_Context.get()};
            if (!entity.GetComponent<ParentComponent>().HasParent)
				DrawEntityNode(entity);
        });
    }
    ImGui::End();

    ImGui::Begin("Properties");
    if (m_SelectionContext)
    {
        DrawComponents(m_SelectionContext);

		ImGui::Spacing();
        ImGui::Spacing();
		ImGui::Spacing();

        float buttonWidth = 260.0f; // Adjust padding as needed
        ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x / 2.0f + buttonWidth / 2.0f) - buttonWidth);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.6f));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 5.0f));
        if (ImGui::Button(ICON_FA_PLUS "   Add Component", ImVec2(buttonWidth, 0))) ImGui::OpenPopup("AddComponent");
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(2);

        // TODO: refactor this so you dont have to manually list the components
        if (ImGui::BeginPopup("AddComponent"))
        {
            ADD_COMPONENT_MENU(ScriptComponent, ICON_FA_CODE "   Script");
            ADD_COMPONENT_MENU(RigidBodyComponent, ICON_FA_CUBE "   Rigid Body");
            ADD_COMPONENT_MENU(BoxColliderComponent, ICON_FA_CUBE "   Box Collider");
			ADD_COMPONENT_MENU(CameraComponent, ICON_FA_VIDEO "   Camera");
			ADD_COMPONENT_MENU(MeshComponent, ICON_FA_CUBE "   Mesh");
			ADD_COMPONENT_MENU(DirectionalLightComponent, ICON_FA_SUN "   Directional Light");
			ADD_COMPONENT_MENU(PointLightComponent, ICON_FA_LIGHTBULB "   Point Light");
			ADD_COMPONENT_MENU(SpotLightComponent, ICON_FA_LIGHTBULB "   Spot Light");

            ImGui::EndPopup();
        }
    }
    ImGui::End();
}
} // namespace Engine
