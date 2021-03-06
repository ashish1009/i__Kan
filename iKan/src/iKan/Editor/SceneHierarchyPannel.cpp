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
#include <iKan/Scene/ScriptableEntity.h>
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
        if (!isSceneHeirarchypanel)
            return;

        m_Context->OnImguiRenderer();

        ImGui::Begin("Scene Hierarchy", pIsOpen);
        {
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

        if (entity.HasComponent<AABBColloiderComponent>())
        {
            auto& isRigid = entity.GetComponent<AABBColloiderComponent>().IsRigid;
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
            enum CameraType { Primary, Editor };
            int32_t type = (cc.GetPrimary()) ? (int32_t)CameraType::Primary : (int32_t)CameraType::Editor;
            ImGui::RadioButton("Primary", &type, (int32_t)CameraType::Primary); ImGui::SameLine();
            ImGui::RadioButton("Editor", &type, (int32_t)CameraType::Editor);
            ImGui::Separator();
            
            if (type == (int32_t)CameraType::Primary)
                cc.MakePrimary();
            else
                cc.MakeEditor();

            auto& camera = cc.Camera;
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
            ImGui::Image((void*)texId, ImVec2(40.0f, 40.0f), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1.0f,1.0f,1.0f,1.0f), ImVec4(1.0f,1.0f,1.0f,0.5f));
            if (ImGui::IsItemClicked((int32_t)MouseCode::ButtonRight))
                ImGui::OpenPopup("Select Texture");

            // Sub menue ofption if mini image is right clicked
            if (ImGui::BeginPopup("Select Texture"))
            {
                const auto& textureMap = m_Context->GetDataRef().TextureMap;
                for (auto texMap : textureMap)
                {
                    const auto& filename = Utils::GetNameFromFilePath(texMap.first);
                    if (ImGui::BeginMenu("Textures"))
                    {
                        ImGui::Bullet();
                        if (ImGui::MenuItem(filename.c_str()))
                        {
                            ImGui::CloseCurrentPopup();
                            src.UploadTexture(texMap.second);
                        }
                        ImGui::EndMenu();
                    }
                }
                if (!src.SubTexComp)
                {
                    if (ImGui::MenuItem("Add Sub Texture", nullptr, false, (src.Texture.Component != nullptr)))
                    {
                        src.AddSubTexture();
                    }
                }
                else
                {
                    if (ImGui::MenuItem("Remove Sub Texture"))
                    {
                        src.RemoveSubTexture();
                    }
                }

                if (ImGui::MenuItem("Remove Texture", nullptr, false, ((src.Texture.Component != nullptr) || (src.SubTexComp != nullptr))))
                {
                    src.ResetAllComponents();
                }
            
                ImGui::EndPopup();
            }

            PropertyGrid::DropConent([&src, this](const std::string& path)
                                     {
                src.UploadTexture(m_Context->AddTextureToScene(path));
            });

            ImGui::SameLine(); PropertyGrid::HelpMarker("Drop the Texture file in the Image Button to upload the texture or Select already stored texture from the scene (Option can be available by right click on image)");
            ImGui::NextColumn();

            ImGui::Checkbox("Use", &src.Texture.Use);
            ImGui::SameLine(); ImGui::ColorEdit4("Color", glm::value_ptr(src.ColorComp), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

            if (src.Texture.Use)
                ImGui::SameLine(); PropertyGrid::CounterF("Tiling Factor", src.TilingFactor);

            ImGui::Columns(1);
            ImGui::PopID();

            ImGui::Separator();

            if (src.SubTexComp)
            {
                // Open the Texture component
                static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;
                if (ImGui::TreeNodeEx((void*)99298273, flags, "Sub Texture Compinent"))
                {
                    // Subtexture property
                    glm::vec2& coords     = src.SubTexComp->GetCoords();
                    glm::vec2& spriteSize = src.SubTexComp->GetSpriteSize();
                    glm::vec2& cellSize   = src.SubTexComp->GetCellSize();

                    // Sprite texture
                    bool opened = ImGui::TreeNodeEx((void*)99298273, ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap, "Tile Image");
                    ImGui::SameLine();
                    PropertyGrid::HelpMarker("Texture Sprite: Hover the mouse over the Sprite, Click ion the Tile to add the tile in selected Entity (Note: Change the Zoom value below)");
                    if (opened)
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
                        src.SubTexComp = SubTexture::CreateFromCoords(src.SubTexComp->GetTexture(), coords, spriteSize, cellSize);

                    ImGui::Separator();
                    ImGui::TreePop();
                } // if (opened)
            } // if (src.TextureComp)

        });
        
        DrawComponent<RigidBody2DComponent>("RigidBody", entity, [](auto& cc)
                                       {
            ImGui::Columns(2);
            ImGui::Text("Body Type");
            ImGui::NextColumn();
            ImGui::PushItemWidth(-1);
            
            const std::vector <const char*> bodyTypeString = { "Static", "Dynamic", "Kinematic" };
            const char* currentBodyType = bodyTypeString[(int32_t)cc.Type];
            if (ImGui::BeginCombo("##BodyType", currentBodyType))
            {
                for (int32_t i = 0; i < bodyTypeString.size(); i++)
                {
                    bool bIsSelected = currentBodyType == bodyTypeString[i];
                    if (ImGui::Selectable(bodyTypeString[i], bIsSelected))
                    {
                        currentBodyType = bodyTypeString[i];
                        cc.Type = ((RigidBody2DComponent::BodyType)i);
                    }
                    
                    if (bIsSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();
            ImGui::NextColumn();
            ImGui::Columns(1);

            ImGui::Separator();

            PropertyGrid::CheckBox("Fixed Rotation", cc.FixedRotation, 100.0f);
        });
        
        DrawComponent<BoxColloider2DComponent>("Box2DComponent", entity, [](auto& cc)
                                            {
            PropertyGrid::Float2("Offser", cc.Offset, nullptr, 0.25f);
            PropertyGrid::Float2("Size", cc.Size, nullptr, 0.25f);
            PropertyGrid::Float("Density", cc.Density, nullptr, 0.01f);
            PropertyGrid::Float("Friction", cc.Friction, nullptr, 0.01f);
            PropertyGrid::Float("Restitution", cc.Restitution, nullptr, 0.01f);
            PropertyGrid::Float("RestitutionThreshold", cc.RestitutionThreshold, nullptr, 0.01f);
        });
        
        DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& cc)
                                               {
            ImGui::ColorEdit4("Color", glm::value_ptr(cc.ColorComp), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            PropertyGrid::Float("Thickness", cc.Thickness, nullptr, 0.01f);
            PropertyGrid::Float("Fade", cc.Fade, nullptr, 0.0001f);
        });
        
    }
    
    // ******************************************************************************
    // add New component to the selected Entity
    // ******************************************************************************
    void SceneHeirarchyPannel::AddComponent()
    {
        if (!m_SelectedEntity.HasComponent<CameraComponent>())
        {
            if (ImGui::MenuItem("Camera"))
            {
                m_SelectedEntity.AddComponent<CameraComponent>();
                ImGui::CloseCurrentPopup();
            }
        }
        
        if (!m_SelectedEntity.HasComponent<SpriteRendererComponent>())
        {
            if (ImGui::MenuItem("Sprite Renderer"))
            {
                m_SelectedEntity.AddComponent<SpriteRendererComponent>();
                ImGui::CloseCurrentPopup();
            }
        }
        
        if (!m_SelectedEntity.HasComponent<RigidBody2DComponent>())
        {
            if (ImGui::MenuItem("Rigid Body 2D"))
            {
                m_SelectedEntity.AddComponent<RigidBody2DComponent>();
                ImGui::CloseCurrentPopup();
            }
        }
        
        if (!m_SelectedEntity.HasComponent<BoxColloider2DComponent>())
        {
            if (ImGui::MenuItem("Box Colloider"))
            {
                m_SelectedEntity.AddComponent<BoxColloider2DComponent>();
                ImGui::CloseCurrentPopup();
            }
        }
        
        if (!m_SelectedEntity.HasComponent<NativeScriptComponent>())
        {
            if (ImGui::MenuItem("Native Script"))
            {
                m_SelectedEntity.AddComponent<NativeScriptComponent>().Bind<EntityController>();
                ImGui::CloseCurrentPopup();
            }
        }
        
        if (!m_SelectedEntity.HasComponent<CircleRendererComponent>())
        {
            if (ImGui::MenuItem("Circle Component Script"))
            {
                m_SelectedEntity.AddComponent<CircleRendererComponent>();
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }
}
