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
#include <iKan/Scene/Viewport.h>
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
    SceneHeirarchyPannel::SceneHeirarchyPannel()
    {
        Init(nullptr);
    }

    // ******************************************************************************
    // Scene hirarchy pannel constructot
    // ******************************************************************************
    SceneHeirarchyPannel::SceneHeirarchyPannel(const Ref<Scene>& context)
    {
        Init(context);
    }

    // ******************************************************************************
    // Scene hirarchy pannel constructot
    // ******************************************************************************
    void SceneHeirarchyPannel::Init(const Ref<Scene>& context)
    {
        SetContext(context);
        IK_CORE_INFO("Scene hierarchy Pannel created");
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
        pIsOpen = &isSceneHeirarchypanel;

        // no rendering if flag is false
        if (!isSceneHeirarchypanel || !m_Context)
            return;

        m_Context->OnImguiRenderer();

        ImGui::Begin("Scene Hierarchy", pIsOpen);
        {
            bool &isSceneEdititng = m_Context->GetDataRef().Editing;
            uint32_t pauseTexId = m_PauseTexture->GetRendererID(), playTexId = m_PlayeTexture->GetRendererID();
            ImGui::Indent(ImGui::GetWindowContentRegionWidth() / 2.0f);

            if (isSceneEdititng)
            {
                if (PropertyGrid::ImageButton("Pause", playTexId, ImVec2(16.0f, 16.0f)))
                    isSceneEdititng = false;
            }
            else
            {
                if (PropertyGrid::ImageButton("Pause", pauseTexId, ImVec2(16.0f, 16.0f)))
                    isSceneEdititng = true;
            }
            ImGui::Unindent(ImGui::GetWindowContentRegionWidth() / 2.0f);
            ImGui::Separator();

            PropertyGrid::String("Active Scene", m_Context->GetFileName(), "To Open a scene Drop the file here fron content browser pannel", 200);
            PropertyGrid::DropConent([](const std::string& path)
                                     {
                Viewport::Get().OpenScene(path);
            });

            PropertyGrid::String("Number of Entities in Scene", m_Context->GetNumEntities(), 200);
            ImGui::Separator();
     
            m_Context->m_Registry.each([&](auto entityID)
            {
                // We are creating a local entity but it is containibg the same entity handle
                // or entt::entity, in the same scene (active)
                Entity entity( { entityID, m_Context.get() } );
                if (entity.GetComponent<SceneHierarchyPannelProp>().IsProp)
                    DrawEntityNode(entity);
            });
            
            // Reset the selected entity
            if (ImGui::IsMouseDown((int32_t)MouseCode::Button0) && ImGui::IsWindowHovered())
                m_SelectedEntity = {};

            // false -> Right-click on blank space
            if (ImGui::BeginPopupContextWindow(0, (int32_t)MouseCode::ButtonRight, false))
            {
                if (ImGui::MenuItem("Create Empty Entity"))
                    m_Context->CreateEntity("Empty Entity");
                if (m_Context->GetEditorCamera())
                {
                    if (ImGui::MenuItem("Remove Editor Camera"))
                        m_Context->DeleteEditorCamera();
                }
                else
                {
                    if (ImGui::MenuItem("Add Editor Camera"))
                        m_Context->SetEditorCamera();
                }
                ImGui::EndPopup();
            }
        }
        ImGui::End(); // ImGui::Begin("Scene Hierarchy", pIsOpen);
        
        ImGui::Begin("Properties", pIsOpen);
        {
            if (m_SelectedEntity)
                DrawComponents(m_SelectedEntity);
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
            m_SelectedEntity = entity;

        bool entityDeleted = false;
        // Right click of mouse
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Entity"))
                entityDeleted = true;
            ImGui::EndPopup();
        }
        
        if (opened)
        {
            bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
            if (opened)
                ImGui::TreePop();

            ImGui::TreePop();
        }
        
        if (entityDeleted)
        {
            m_Context->DestroyEntity(entity);
            if (m_SelectedEntity == entity)
                m_SelectedEntity = {};
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
            PropertyGrid::String("ID", (uint32_t )Id, 100.0f);
            ImGui::Separator();
        }
        
        if (entity.HasComponent<TagComponent>())
        {
            auto& tag = entity.GetComponent<TagComponent>().Tag;
            PropertyGrid::String("Tag", tag, 100.0f);
            ImGui::Separator();
        }

        if (entity.HasComponent<BoxCollider2DComponent>())
        {
            auto& isRigid = entity.GetComponent<BoxCollider2DComponent>().IsRigid;
            PropertyGrid::CheckBox("Is Rigid", isRigid, 100.0f);
            ImGui::Separator();
        }

        ImGui::SameLine();
        
        ImGui::PushItemWidth(-1);
        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("AddComponent");

        if (ImGui::BeginPopup("AddComponent"))
            AddComponent();

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
                
                const std::vector <const char*> projectionTypeSTring = { "Perspective", "Orthographic" };
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
                            ImGui::SetItemDefaultFocus();
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
        
        DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [this](auto& src)
                                               {
            // Change the color of the Entity
            ImGui::PushID("Texture");
            ImGui::Columns(2);

            ImGui::SetColumnWidth(0, 100);

            size_t texId = ((src.Texture.Component) ? src.Texture.Component->GetRendererID() : ((src.SubTexComp) ? src.SubTexComp->GetTexture()->GetRendererID() : m_DefaultTexture->GetRendererID()));
            ImGui::Image((void*)texId, ImVec2(64.0f, 64.0f), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1.0f,1.0f,1.0f,1.0f), ImVec4(1.0f,1.0f,1.0f,0.5f));
            PropertyGrid::DropConent([&src, this](const std::string& path)
                                     {
                src.UploadTexture(m_Context->AddTextureToScene(path));
            });

            ImGui::SameLine(); PropertyGrid::HelpMarker("Drop the Texture file in the Image Button to upload the texture or Uplaod already stored texture from the scene");

            ImGui::NextColumn();

            ImGui::SameLine(); ImGui::Checkbox("Use", &src.Texture.Use);
            ImGui::SameLine(); ImGui::ColorEdit4("Color", glm::value_ptr(src.ColorComp), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

            ImGui::SameLine();
            // Select the texture from already uploaded textures
            const auto& textureMap = m_Context->GetDataRef().TextureMap;
            const char* currentTextureSelected = "Select Texture";
            if (ImGui::BeginCombo("##Texture", currentTextureSelected))
            {
                for (auto texMap : textureMap)
                {
                    const auto& filename = Utils::GetNameFromFilePath(texMap.first);
                    bool bIsSelected = currentTextureSelected == filename;
                    if (ImGui::Selectable(filename.c_str(), bIsSelected))
                    {
                        currentTextureSelected = filename.c_str();
                        src.UploadTexture(texMap.second);
                    }

                    if (bIsSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::Separator();
            ImGui::Columns(1);
            ImGui::PopID();

            if (src.Texture.Component || src.SubTexComp)
            {
                // Open the Texture component
                ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 1, 1 });
                float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y;

                static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;

                std::string treeNodeString = (src.Texture.Component) ? "Texture Component" : "Sub Texture Compinent";

                bool opened = ImGui::TreeNodeEx((void*)99298273, flags, treeNodeString.c_str());
                ImGui::PopStyleVar();

                ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
                if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
                {
                    ImGui::OpenPopup(treeNodeString.c_str());
                }

                // Texture Settings
                if (ImGui::BeginPopup(treeNodeString.c_str()))
                {
                    if (ImGui::MenuItem("Remove texture"))
                    {
                        src.ResetAllComponents();
                    }

                    if (!src.SubTexComp && src.Texture.Component)
                    {
                        if (ImGui::MenuItem("Add Subtexture"))
                        {
                            // Add the subtexture component
                            src.SubTexComp = SubTexture::CreateFromCoords(src.Texture.Component, glm::vec2(1.0f));

                            // If texture is already created then delete the texture
                            // (if shared with other entity then reduce the counter)
                            if (src.Texture.Component)
                            {
                                src.Texture.Component.reset();
                            }
                            src.Texture.Component = nullptr;
                        }
                    }
                    ImGui::EndPopup();
                } // if (ImGui::BeginPopup("treeNodeString.c_str()"))

                // If texture component is opened
                if (opened)
                {
                    // Print the current texture path
                    std::string texturePath;
                    if (src.Texture.Component)
                    {
                        texturePath = src.Texture.Component->GetfilePath();
                    }
                    if (src.SubTexComp)
                    {
                        texturePath = src.SubTexComp->GetTexture()->GetfilePath();
                    }
                    PropertyGrid::String("Texture path", texturePath, 100.0f, 300.0f, "", false);
                    ImGui::Separator();

                    // From here Component specific prperties

                    // Chnage the tiling Factor of the enitty if Texture component is present
                    if (src.Texture.Component != nullptr)
                    {
                        PropertyGrid::CounterF("Tiling Factor", src.TilingFactor);
                        ImGui::Separator();
                    }

                    // Chnage the Subtexture properties
                    if (src.SubTexComp != nullptr)
                    {
                        // Subtexture property
                        glm::vec2& coords     = src.SubTexComp->GetCoords();
                        glm::vec2& spriteSize = src.SubTexComp->GetSpriteSize();
                        glm::vec2& cellSize   = src.SubTexComp->GetCellSize();

                        // Sprite texture
                        ImGui::TextWrapped("Texture Sprite: Hover the mouse over the Sprite, Click ion the Tile to add the tile in selected Entity (Note: Change the Zoom value below)");
                        if (ImGui::TreeNode("Images"))
                        {
                            ImGuiIO& io = ImGui::GetIO();

                            static float zoom = 6.0f;
                            PropertyGrid::Float("Zoom", zoom, nullptr);

                            size_t texId    = src.SubTexComp->GetTexture()->GetRendererID();
                            float texWidth  = (float)src.SubTexComp->GetTexture()->GetWidth();
                            float texHeight = (float)src.SubTexComp->GetTexture()->GetHeight();

                            ImGui::Image((void*)texId, ImVec2(texWidth, texHeight), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1.0f,1.0f,1.0f,1.0f), ImVec4(1.0f,1.0f,1.0f,0.5f));

                            ImVec2 pos = ImGui::GetCursorScreenPos();
                            if (ImGui::IsItemHovered())
                            {
                                ImGui::BeginTooltip();

                                float regionFixedX = spriteSize.x * cellSize.x;
                                float regionFixedY = spriteSize.y * cellSize.y;

                                float regionX = io.MousePos.x - pos.x - regionFixedX * 0.5f;
                                if (regionX < 0.0f)
                                    regionX = 0.0f;

                                else if (regionX > texWidth - regionFixedX)
                                    regionX = texWidth - regionFixedX;

                                float regionY = pos.y - io.MousePos.y - regionFixedY * 0.5f;
                                if (regionY < 0.0f)
                                    regionY = 0.0f;

                                else if (regionY > texHeight - regionFixedY)
                                    regionY = texHeight - regionFixedY;

                                ImGui::Text("Min: (%.2f, %.2f)", regionX, regionY);
                                ImGui::Text("Max: (%.2f, %.2f)", regionX + regionFixedX, regionY + regionFixedX);

                                ImVec2 uv0 = ImVec2((regionX) / texWidth, (regionY + regionFixedY) / texHeight);
                                ImVec2 uv1 = ImVec2((regionX + regionFixedX) / texWidth, (regionY) / texHeight);

                                ImGui::Image((void*)texId, ImVec2(regionFixedX * zoom, regionFixedY * zoom), uv0, uv1, ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 0.5f));

                                if (ImGui::IsMouseClicked(0))
                                {
                                    coords.x = (((regionX + regionFixedX)) / cellSize.x) - 1;
                                    coords.y = (((regionY + regionFixedX)) / cellSize.y) - 1;

                                    src.SubTexComp = SubTexture::CreateFromCoords(src.SubTexComp->GetTexture(), coords, spriteSize, cellSize);
                                }
                                ImGui::EndTooltip();
                            }
                            ImGui::TreePop();
                        }
                        ImGui::Separator();
                        
                        bool modCoord      = PropertyGrid::Float2("Coordinates", coords, nullptr);
                        bool modSpriteSize = PropertyGrid::Float2("Sprite Size", spriteSize, nullptr);
                        bool modCellSize   = PropertyGrid::Float2("Cell Size", cellSize, nullptr);

                        if (modCoord || modSpriteSize || modCellSize)
                        {
                            src.SubTexComp = SubTexture::CreateFromCoords(src.SubTexComp->GetTexture(), coords, spriteSize, cellSize);
                        }
                        ImGui::Separator();
                    }

                    ImGui::TreePop();
                } // if (opened)
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
