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
            
            std::string idStr;
            std::stringstream ss;
            ss << Id;
            ss >> idStr;
            
            PropertyGrid::String("ID", idStr, false);
            ImGui::Separator();
        }
        
        if (entity.HasComponent<TagComponent>())
        {
            auto& tag = entity.GetComponent<TagComponent>().Tag;
            PropertyGrid::String("Tag", tag);
            ImGui::Separator();
        }
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
            
            PropertyGrid::CheckBox("Primary", cc.Primary);
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
            ImGui::ColorEdit4("Color", glm::value_ptr(src.ColorComp));
            if (src.TextureComp || src.SubTexComp)
                PropertyGrid::CounterF("Tiling Factor", src.TilingFactor);
            ImGui::Separator();
            
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, 100);
            ImGui::SetColumnWidth(1, 300);
            ImGui::Text("File Path");
            ImGui::NextColumn();
            ImGui::PushItemWidth(-1);
            {
                std::string currentTexture;
                {
                    if (src.TextureComp)
                    {
                        // Extract the name of current Mesh
                        auto path      = src.TextureComp->GetfilePath();
                        auto lastSlash = path.find_last_of("/\\");
                        lastSlash      = lastSlash == std::string::npos ? 0 : lastSlash + 1;
                        
                        auto lastDot = path.find_last_of('.');
                        auto count   = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;
                        
                        currentTexture = path.substr(lastSlash, count);
                        ImGui::InputText("##Texturefilepath", (char*)path.c_str(), 256, ImGuiInputTextFlags_ReadOnly);
                    }
                    else
                    {
                        currentTexture = "Null";
                        ImGui::InputText("##Texturefilepath", (char*)"Null", 256, ImGuiInputTextFlags_ReadOnly);
                        
                    }
                    ImGui::PopItemWidth();
                }
                
                ImGui::Columns(1);
                
                ImGui::Columns(2);
                ImGui::Text("Available Textures");
                ImGui::NextColumn();
                ImGui::PushItemWidth(-1);
                
                const std::vector<const char*> availabelTextureString = { "checkerboard", "container", "grass", "marble", "matrix", "metal", "smile", "window", "wood" };
                if (ImGui::BeginCombo("##Type", currentTexture.c_str()))
                {
                    for (int32_t i = 0; i < availabelTextureString.size(); i++)
                    {
                        bool bIsSelected = currentTexture == availabelTextureString[i];
                        if (ImGui::Selectable(availabelTextureString[i], bIsSelected))
                        {
                            currentTexture = availabelTextureString[i];
                            // TODO: for now path is hard coded will fix this in future
                            // Make sure file name is same as the foler name
                            std::string filePath = "/Users/ashish./iKan/GitHub/iKan/Editor/assets/resources/texture/" + currentTexture + ".png";
                            src.TextureComp = Texture::Create(filePath);
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
