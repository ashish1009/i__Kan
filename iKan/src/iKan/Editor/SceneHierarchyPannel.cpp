// ******************************************************************************
// File         : SceneHierarchyPannel.cpp
// Description  : Class for having property pannel of each entity
// Project      : iKan : Editor
//
// Created by Ashish on 02/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "SceneHierarchyPannel.h"
#include <iKan/Scene/Component.h>
#include <iKan/Editor/ScenePropertyGrid.h>

namespace iKan {
    
    // ******************************************************************************
    // Draw the components in property pannel. Takes the function pointer in argument
    // ******************************************************************************
    template<typename T, typename UIFunction>
    static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
    {
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen |
                                                 ImGuiTreeNodeFlags_Framed |
                                                 ImGuiTreeNodeFlags_SpanAvailWidth |
                                                 ImGuiTreeNodeFlags_AllowItemOverlap |
                                                 ImGuiTreeNodeFlags_FramePadding;
        
        if (entity.HasComponent<T>())
        {
            auto& component               = entity.GetComponent<T>();
            ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
            
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImGui::Separator();
            bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
            ImGui::PopStyleVar(); // ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
            
            ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
            if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight }))
            {
                ImGui::OpenPopup("ComponentSettings");
            }
            
            bool removeComponent = false;
            if (ImGui::BeginPopup("ComponentSettings"))
            {
                if (ImGui::MenuItem("Remove component"))
                {
                    removeComponent = true;
                }
                ImGui::EndPopup();
            }
            
            if (open)
            {
                uiFunction(component);
                ImGui::TreePop();
            }
            
            if (removeComponent)
            {
                entity.RemoveComponent<T>();
            }
        }
    }

    // ******************************************************************************
    // Scene hirarchy pannel constructot
    // ******************************************************************************
    SceneHeirarchyPannel::SceneHeirarchyPannel(const Ref<Scene>& context)
    {
        IK_CORE_INFO("Scene hierarchy Pannel created");
        SetContext(context);
    }
    
    // ******************************************************************************
    // Scene hirarchy pannel Context is updated
    // ******************************************************************************
    void SceneHeirarchyPannel::SetContext(const Ref<Scene>& context)
    {
        IK_CORE_INFO("Context Updated in Scene Hierarchy pannel");
        m_Context = context;
    }
    
    // ******************************************************************************
    // Scene hirarchy pannel Imgui renderer
    // ******************************************************************************
    void SceneHeirarchyPannel::OnImguiender(bool* pIsOpen)
    {
        ImGui::Begin("Scene Hierarchy", pIsOpen);
        {
            PropertyGrid::String("Number of Entities in Scene", m_Context->GetNumEntities(), 200);
            ImGui::Separator();

            m_Context->OnImguiRenderer();
            
            m_Context->m_Registry.each([&](auto entityID)
            {
                // We are creating a local entity but it is containibg the same entity handle
                // or entt::entity, in the same scene (active)
                Entity entity( { entityID, m_Context.get() } );
                if (entity.GetComponent<SceneHierarchyPannelProp>().IsProp)
                {
                    DrawEntityNode(entity);
                }
            });
            
            // Reset the selected entity
            if (ImGui::IsMouseDown((int32_t)MouseCode::Button0) && ImGui::IsWindowHovered())
            {
                m_SelectedEntity = {};
            }
            
            // false -> Right-click on blank space
            if (ImGui::BeginPopupContextWindow(0, (int32_t)MouseCode::ButtonRight, false))
            {
                if (ImGui::MenuItem("Create Empty Entity"))
                {
                    m_Context->CreateEntity("Empty Entity");
                }
                ImGui::EndPopup();
            }
        }
        ImGui::End(); // ImGui::Begin("Scene Hierarchy", pIsOpen);
        
        ImGui::Begin("Properties", pIsOpen);
        {
            if (m_SelectedEntity)
            {
                DrawComponents(m_SelectedEntity);
            }
        }
        ImGui::End(); // ImGui::Begin("Properties", pIsOpen);
    }
    
    // ******************************************************************************
    // Scene hirarchy pannel Draw each entity. Print the name of each Enitity that
    // is selectable
    // ******************************************************************************
    void SceneHeirarchyPannel::DrawEntityNode(Entity entity)
    {
        auto& tag = entity.GetComponent<TagComponent>().Tag;
                
        ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
        if (ImGui::IsItemClicked())
        {
            m_SelectedEntity = entity;
        }
        
        bool entityDeleted = false;
        // Right click of mouse
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Entity"))
            {
                entityDeleted = true;
            }
            ImGui::EndPopup();
        }
        
        if (opened)
        {
            bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
            if (opened)
            {
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
        
        if (entityDeleted)
        {
            m_Context->DestroyEntity(entity);
            if (m_SelectedEntity == entity)
            {
                m_SelectedEntity = {};
            }
        }
    }
    
    // ******************************************************************************
    // Scene hirarchy pannel Draw component
    // ******************************************************************************
    void SceneHeirarchyPannel::DrawComponents(Entity entity)
    {
        if (entity.HasComponent<IDComponent>())
        {
            const auto& Id = entity.GetComponent<IDComponent>().ID;
            PropertyGrid::String("ID", (uint32_t )Id, 200.0f);
            ImGui::Separator();
        }
        
        if (entity.HasComponent<TagComponent>())
        {
            auto& tag = entity.GetComponent<TagComponent>().Tag;
            PropertyGrid::String("Tag", tag, 200.0f);
            ImGui::Separator();
        }

        std::string IsRigitString;
        if (entity.HasComponent<BoxCollider2DComponent>())
        {
            auto& IsRigid = entity.GetComponent<BoxCollider2DComponent>().IsRigid;
            IsRigitString = (IsRigid ? "Yes": "No");
        }
        PropertyGrid::String("Is RigiD", IsRigitString, 200.0f, " ", false); // No need to add any hind as string in non modifyiable String
        ImGui::Separator();

        ImGui::SameLine();
        
        ImGui::PushItemWidth(-1);
        if (ImGui::Button("Add Component"))
        {
            ImGui::OpenPopup("AddComponent");
        }
        
        if (ImGui::BeginPopup("AddComponent"))
        {
            AddComponent();
        }
        ImGui::PopItemWidth();
        
        DrawComponent<TransformComponent>("Transform", entity, [](auto& tc)
                                          {
            PropertyGrid::Float3("Translation", tc.Translation, nullptr, 0.25f);
            glm::vec3 rotation = glm::degrees(tc.Rotation);
            
            PropertyGrid::Float3("Rotation", rotation, nullptr, 0.25f);
            tc.Rotation = glm::radians(rotation);
            
            PropertyGrid::Float3("Scale", tc.Scale, nullptr, 0.25f, 1.0f);
        });
        
        DrawComponent<CameraComponent>("Camera", entity, [](auto& cc)
                                       {
            auto& camera = cc.Camera;
            
            PropertyGrid::CheckBox("Primary", cc.Primary, 100);
            ImGui::Separator();

            {
                ImGui::Columns(2);
                ImGui::Text("Projection Type");
                ImGui::NextColumn();
                ImGui::PushItemWidth(-1);
                
                const std::vector <const char*> projectionTypeSTring = { "Projection", "Orthographic" };
                const char* currentProjectionType = projectionTypeSTring[(int32_t)camera.GetProjectionType()];
                if (ImGui::BeginCombo("##Projection", currentProjectionType))
                {
                    for (int32_t i = 0; i < projectionTypeSTring.size(); i++)
                    {
                        bool bIsSelected = currentProjectionType == projectionTypeSTring[i];
                        if (ImGui::Selectable(projectionTypeSTring[i], bIsSelected))
                        {
                            currentProjectionType = projectionTypeSTring[i];
                            camera.SetProjectionType((SceneCamera::ProjectionType)i);
                        }
                        
                        if (bIsSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::PopItemWidth();
                ImGui::NextColumn();
                ImGui::Columns(1);
            }
            ImGui::Separator();

            if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
            {
                float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveFOV());
                if (PropertyGrid::Float("Vertical FOV", perspectiveVerticalFov, nullptr))
                    camera.SetPerspectiveFOV(glm::radians(perspectiveVerticalFov));
                
                float perspectiveNear = camera.GetPerspectiveNearClip();
                if (PropertyGrid::Float("Near", perspectiveNear, nullptr))
                    camera.SetPerspectiveNearClip(perspectiveNear);
                
                float perspectiveFar = camera.GetPerspectiveFarClip();
                if (PropertyGrid::Float("Far", perspectiveFar, nullptr))
                    camera.SetPerspectiveFarClip(perspectiveFar);
            }
            
            if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
            {
                float orthoSize = camera.GetOrthographicSize();
                if (PropertyGrid::Float("Size", orthoSize, nullptr))
                    camera.SetOrthographicSize(orthoSize);
                
                float orthoNear = camera.GetOrthographicNearClip();
                if (PropertyGrid::Float("Near", orthoNear, nullptr))
                    camera.SetOrthographicNearClip(orthoNear);
                
                float orthoFar = camera.GetOrthographicFarClip();
                if (PropertyGrid::Float("Far", orthoFar, nullptr))
                    camera.SetOrthographicFarClip(orthoFar);
            }
        });
        
        DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& src)
                                               {
            // Change the color of the Entity
            ImGui::ColorEdit4("Color", glm::value_ptr(src.ColorComp));

            // Upload New Texture Texture
            static std::string newTexturePath;
            if (ImGui::Button("Upload Texture") && newTexturePath != "")
            {
                // If texture is already created then delete the texture
                // (if shared with other entity then reduce the counter)
                if (src.TextureComp)
                {
                    src.TextureComp.reset();
                }
                src.TextureComp = Texture::Create(newTexturePath);

                // If texture is uploaded with invalid path So delete the texture
                if (!src.TextureComp->Uploaded())
                {
                    src.TextureComp.reset();
                }
            }
            ImGui::SameLine();
            PropertyGrid::String("", newTexturePath, 110.0f, "Enter Texture path here (Path should be absolute)");
            ImGui::Separator();

            if (src.TextureComp)
            {
                // Open the Texture component
                ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 1, 1 });
                float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y;

                static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;
                bool opened = ImGui::TreeNodeEx((void*)99298273, flags, "Texture Component");
                ImGui::PopStyleVar();

                ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
                if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
                {
                    ImGui::OpenPopup("TextureSetting");
                }

                // Texture Settings
                if (ImGui::BeginPopup("TextureSetting"))
                {
                    if (ImGui::MenuItem("Remove texture"))
                    {
                        // If texture is already created then delete the texture
                        // (if shared with other entity then reduce the counter)
                        if (src.TextureComp)
                        {
                            src.TextureComp.reset();
                        }
                        src.TextureComp = nullptr;
                    }
                    ImGui::EndPopup();
                }

                // If texture component is opened
                if (opened)
                {
                    // Print the current texture path
                    std::string texturePath = src.TextureComp->GetfilePath();
                    PropertyGrid::String("Texture path", texturePath, 100.0f, "", false);

                    // Chnage the tiling Factor of the enitty
                    if (src.TextureComp != nullptr)
                    {
                        PropertyGrid::CounterF("Tiling Factor", src.TilingFactor);
                        ImGui::Separator();
                    }
                    ImGui::TreePop();
                }
            } // if (src.TextureComp)

        });
        
    }
    
    // ******************************************************************************
    // add New component to the selected Entity
    // ******************************************************************************
    void SceneHeirarchyPannel::AddComponent()
    {
        if (ImGui::MenuItem("Camera"))
        {
            if (!m_SelectedEntity.HasComponent<CameraComponent>())
            {
                m_SelectedEntity.AddComponent<CameraComponent>();
            }
            else
            {
                IK_CORE_WARN("This entity already has the Camera Component!");
            }
            ImGui::CloseCurrentPopup();
        }
        
        if (ImGui::MenuItem("Sprite Renderer"))
        {
            if (!m_SelectedEntity.HasComponent<SpriteRendererComponent>())
            {
                m_SelectedEntity.AddComponent<SpriteRendererComponent>();
            }
            else
            {
                IK_CORE_WARN("This entity already has the Camera Component!");
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}
