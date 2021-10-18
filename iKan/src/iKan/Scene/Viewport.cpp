// ******************************************************************************
// File         : Viewport.cpp
// Description  : stores all the information of Viewport
// Project      : iKan : Scene
//
// Created by Ashish on 02/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "Viewport.h"
#include <iKan/Core/Application.h>
#include <iKan/Core/Input.h>
#include <iKan/Scene/Component.h>
#include <iKan/Editor/ScenePropertyGrid.h>
#include <iKan/Imgui/ImguiAPI.h>
#include <iKan/Renderer/RendererStats.h>
#include <iKan/Renderer/SceneRenderer.h>
#include "ImGuizmo.h"

namespace iKan {

    Viewport* Viewport::s_Instance = nullptr;

    // ******************************************************************************
    // Viewport Default Constructor
    // ******************************************************************************
    Viewport::Viewport(const std::string& contentBrowserPath)
    : m_ContentBrowserPannel(contentBrowserPath)
    {
        // Frame buffer specifications
        Framebuffer::Specification specs;
        specs.Attachments = { Framebuffer::TextureSpecification::TextureFormat::RGBA8,
            Framebuffer::TextureSpecification::TextureFormat::DEPTH24STENCIL8,
            Framebuffer::TextureSpecification::TextureFormat::R32I };

        m_Data.FrameBuffer = Framebuffer::Create(specs);
    }
    
    // ******************************************************************************
    // Update the view port bouds
    // ******************************************************************************
    void Viewport::UpdateBounds()
    {
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 minBound   = ImGui::GetWindowPos();
        
        minBound.x += m_Data.CursorPos.x;
        minBound.y += m_Data.CursorPos.y;

        ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
        m_Data.Bounds[0] = { minBound.x, minBound.y };
        m_Data.Bounds[1] = { maxBound.x, maxBound.y };
    }
    
    // ******************************************************************************
    // Update the Viewport
    // ******************************************************************************
    void Viewport::OnUpdateImGui()
    {
        // If viewport is not present then return
        if (!m_Flags.Present)
            return;

        if (m_SaveFile || m_SaveFileAs)
            SaveScene();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Viewport", &m_Flags.Present, ImGuiWindowFlags_NoTitleBar);
        {
            m_Data.CursorPos = ImGui::GetCursorPos();

            m_Data.Focused = ImGui::IsWindowFocused();
            m_Data.Hovered = ImGui::IsWindowHovered();
            Application::Get().GetImGuiLayer()->BlockEvents(!m_Data.Focused && !m_Data.Hovered);

            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            m_Data.Size = { viewportPanelSize.x, viewportPanelSize.y };

            size_t textureID = m_Data.FrameBuffer->GetColorAttachmentRendererID();
            ImGui::Image((void*)textureID, ImVec2{ m_Data.Size.x, m_Data.Size.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
            
            PropertyGrid::DropConent([](const std::string& path)
                                     {
                Viewport::Get().OpenScene(path);
            });
            
            OnImguizmoUpdate();
            UpdateBounds();
        }

        ImGui::End(); // ImGui::Begin("Viewport");
        ImGui::PopStyleVar(); // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    }
    
    // ******************************************************************************
    // Update Imguizmo for selected entity
    // ******************************************************************************
    void Viewport::OnImguizmoUpdate()
    {
        Entity selectedEntity = m_SceneHierarchyPannel.GetSelectedEntity();
        if (selectedEntity && m_Data.GizmoType != -1)
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
	            
            float windowWidth = (float)ImGui::GetWindowWidth();
            float windowHeight = (float)ImGui::GetWindowHeight();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
            
            // Camera
            glm::mat4 cameraProjection;
            glm::mat4 cameraView;
            
            if (m_ActiveScene->GetSceneType() == Scene::NativeData::Type::Scene3D)
            {
                const Ref<EditorCamera>& editorCamera = m_ActiveScene->GetNativeDataRef().EditorCamera;
                cameraProjection = editorCamera->GetProjection();
                cameraView = editorCamera->GetViewMatrix();
            }
            else
            {
                // TODO: Not working right now
                auto cameraEntity = m_ActiveScene->GetEditorCameraEntity();
                const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
                cameraProjection = camera.GetProjection();
                cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());
            }

            // Entity transform
            auto& tc = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = tc.GetTransform();
            
            // Snapping
            bool snap = Input::IsKeyPressed(KeyCode::LeftControl);
            float snapValue = 0.5f; // Snap to 0.5m for translation/scale
            
            // Snap to 45 degrees for rotation
            if (m_Data.GizmoType == ImGuizmo::OPERATION::ROTATE)
                snapValue = 45.0f;
            
            float snapValues[3] = { snapValue, snapValue, snapValue };
            
            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                                 (ImGuizmo::OPERATION)m_Data.GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
                                 nullptr, snap ? snapValues : nullptr);
            
            if (ImGuizmo::IsUsing())
            {
                glm::vec3 translation, rotation, scale;
                Math::DecomposeTransform(transform, translation, rotation, scale);
                
                glm::vec3 deltaRotation = rotation - tc.Rotation;
                tc.Translation = translation;
                tc.Rotation += deltaRotation;
                tc.Scale = scale;
            }
        }
    }

    // ******************************************************************************
    // Update mouse position of viewport
    // ******************************************************************************
    void Viewport::OnUpdate(Timestep ts)
    {
        // No update in case Scene is not created
        if (!m_ActiveScene)
            return;

        // If resize the window call the update the Scene View port and Frame buffer should be resized
        if (Framebuffer::Specification spec = m_Data.FrameBuffer->GetSpecification();
            m_Data.Size.x > 0.0f && m_Data.Size.y > 0.0f && // zero sized framebuffer is invalid
            (spec.Width != m_Data.Size.x || spec.Height != m_Data.Size.y))
        {
            m_Data.FrameBuffer->Resize((uint32_t)m_Data.Size.x, (uint32_t)m_Data.Size.y);
            m_ActiveScene->OnViewportResize((uint32_t)m_Data.Size.x, (uint32_t)m_Data.Size.y);
        }

        RendererStatistics::Reset();

        m_Data.FrameBuffer->Bind();
        {
            Renderer::Clear(m_Data.BgColor);
            
            Scene::NativeData::State state = m_ActiveScene->GetNativeDataRef().SceneState;

            switch (state)
            {
                case Scene::NativeData::State::Edit:
                {
                    m_ActiveScene->OnUpdateEditor(ts);
                                    
                    // Update Viewprt entities
                    UpdateMousePos();
                    UpdateHoveredEntity();
                    
                    break;
                }
                case Scene::NativeData::State::Play:
                {
                    m_ActiveScene->OnUpdateRuntime(ts);
                    break;
                }
                default:
                    break;
            }
        }
        m_Data.FrameBuffer->Unbind();
    }
    
    // ******************************************************************************
    // Update mouse position of viewport
    // ******************************************************************************
    void Viewport::UpdateMousePos()
    {
        auto [mx, my] = ImGui::GetMousePos();
        mx -= m_Data.Bounds[0].x;
        my -= m_Data.Bounds[0].y;

        m_Data.Height = m_Data.Bounds[1].y - m_Data.Bounds[0].y;
        m_Data.Width  = m_Data.Bounds[1].x - m_Data.Bounds[0].x;

        my = m_Data.Height - my;

        m_Data.MousePosX = (int32_t)mx;
        m_Data.MousePosY = (int32_t)my;
    }

    // ******************************************************************************
    // Update the Hovered entity in the view port
    // ******************************************************************************
    void Viewport::UpdateHoveredEntity()
    {
        if (m_Data.MousePosX >= 0 && m_Data.MousePosY >= 0 && m_Data.MousePosX <= m_Data.Width && m_Data.MousePosY <= m_Data.Height )
        {
            m_ActiveScene->GetEntityIdFromPixels(m_Data.MousePosX, m_Data.MousePosY, m_Data.HoveredEntityID);
            m_Data.HoveredEntity = (m_Data.HoveredEntityID >= m_ActiveScene->GetMaxEntityId()) ? Entity() : Entity((entt::entity)m_Data.HoveredEntityID, m_ActiveScene.get());
        }
    }

    // ******************************************************************************
    // Scene Editor Events
    // ******************************************************************************
    void Viewport::OnEvent(Event& event)
    {
        if (m_ActiveScene)
            m_ActiveScene->OnEvent(event);

        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseButtonPressedEvent>(IK_BIND_EVENT_FN(Viewport::OnMouseButtonPressed));
        dispatcher.Dispatch<KeyPressedEvent>(IK_BIND_EVENT_FN(Viewport::OnKeyPressed));
    }

    // ******************************************************************************
    // Mouse button press event
    // ******************************************************************************
    bool Viewport::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        if (e.GetMouseButton() == MouseCode::ButtonLeft && !Input::IsKeyPressed(KeyCode::LeftAlt))
        {
            if (m_Data.MousePosX >= 0 && m_Data.MousePosY >= 0 && m_Data.MousePosX <= m_Data.Width && m_Data.MousePosY <= m_Data.Height )
                m_SceneHierarchyPannel.SetSelectedEntity(m_Data.HoveredEntity);
            else
                m_Data.GizmoType = -1;
        }
        return false;
    }
    
    // ******************************************************************************
    // Window Resize event
    // ******************************************************************************
    bool Viewport::OnMouseWindowResize(WindowResizeEvent& e)
    {
        SceneRenderer::SetViewportSize(e.GetWidth(), e.GetHeight());
        return false;
    }

    // ******************************************************************************
    // key press events
    // ******************************************************************************
    bool Viewport::OnKeyPressed(KeyPressedEvent& event)
    {
        // Shortcuts
        if (event.GetRepeatCount() > 0)
            return false;

        bool cmd    = Input::IsKeyPressed(KeyCode::LeftSuper) || Input::IsKeyPressed(KeyCode::RightSuper);
        bool ctrl   = Input::IsKeyPressed(KeyCode::LeftControl) || Input::IsKeyPressed(KeyCode::RightControl);
        switch (event.GetKeyCode())
        {
            case KeyCode::S:    if (cmd && m_ActiveScene)   m_SaveFile   = true;    break;
            case KeyCode::A:    if (cmd && m_ActiveScene)   m_SaveFileAs = true;    break;

            case KeyCode::N:    if (cmd)    NewScene();     break;
            case KeyCode::X:    if (cmd)    CloseScene();   break;
                
            case KeyCode::D:    if (cmd)    OnDuplicateEntity(); break;
                                
            // Gizmos
            case KeyCode::Q:    if (ctrl)    m_Data.GizmoType = -1;                               break;
            case KeyCode::W:    if (ctrl)    m_Data.GizmoType = ImGuizmo::OPERATION::TRANSLATE;   break;
            case KeyCode::E:    if (ctrl)    m_Data.GizmoType = ImGuizmo::OPERATION::ROTATE;      break;
            case KeyCode::R:    if (ctrl)    m_Data.GizmoType = ImGuizmo::OPERATION::SCALE;       break;
                
            default:    break;
        }
        return false;
    }

    // ******************************************************************************
    // Create new active scene to the Viewport
    // ******************************************************************************
    const Ref<Scene>& Viewport::NewScene()
    {
        CloseScene();
        
        m_EditorScene = CreateRef<Scene>();
        Scene::ResetNativeData();
        
        m_EditorScene->OnViewportResize((uint32_t)m_Data.Size.x, (uint32_t)m_Data.Size.y);

        IK_INFO("New scene is created (Reference count for Scene is {0} )", m_ActiveScene.use_count());
        
        m_ActiveScene = m_EditorScene;
        
        // Set the current Scene to scene hierarchy pannel
        m_SceneHierarchyPannel.SetContext(m_ActiveScene);
        return m_ActiveScene;
    }

    // ******************************************************************************
    // Open saved scene
    // ******************************************************************************
    const Ref<Scene>& Viewport::OpenScene(const std::string& path)
    {
        if (m_ActiveScene->GetSceneState() != Scene::NativeData::State::Edit)
            OnSceneEdit();
        
        IK_INFO("Opening saved scene from {0}", path.c_str());
        if (!path.empty())
        {
            m_SceneHierarchyPannel.SetSelectedEntity({});
            CloseScene();
            
            m_EditorScene = CreateRef<Scene>(path);

            SceneSerializer serializer(m_EditorScene);
            serializer.Deserialize(path);
            
            m_EditorScene->OnViewportResize((uint32_t)m_Data.Size.x, (uint32_t)m_Data.Size.y);
            
            m_ActiveScene = m_EditorScene;
            m_SceneHierarchyPannel.SetContext(m_ActiveScene);
        }
        
        return m_ActiveScene;
    }

    // ******************************************************************************
    // Saving Scene to new file
    // ******************************************************************************
    const Ref<Scene>& Viewport::SaveSceneAs(const std::string& path)
    {
        if (path != "")
        {
            m_ActiveScene->SetFilePath(path);
            SceneSerializer serializer(m_ActiveScene);
            serializer.Serialize(path);
            return m_ActiveScene;
        }
        
        if (!m_SaveFileAs && !m_SaveFile)
            return m_ActiveScene;
        
        m_SaveFileAs = true;
        ImGui::Begin("Save File", &m_SaveFileAs);
        m_SaveFile = m_SaveFileAs;

        const auto& relativePath    = (std::filesystem::relative(m_ContentBrowserPannel.GetCurrentDir(), m_ContentBrowserPannel.GetRootDir())).string();
        PropertyGrid::String("Save Directory", relativePath, "File will be saved at the Current directory in the active scene", 150.0f);

        static std::string fileName = "";
        PropertyGrid::String("File Name", fileName, 150.0f);

        if (ImGui::Button("Save Scene") && fileName != "")
        {
            std::string filepath = m_ContentBrowserPannel.GetCurrentDir().string() + "/" + fileName + ".iKan";
            IK_INFO("Saving Scene at {0}", filepath.c_str());
            if (!filepath.empty())
            {
                m_ActiveScene->SetFilePath(filepath);
                SceneSerializer serializer(m_ActiveScene);
                serializer.Serialize(filepath);
            }
            m_SaveFileAs = false;
        }
        ImGui::End();
        
        return m_ActiveScene;
    }

    // ******************************************************************************
    // Saving Scene
    // ******************************************************************************
    const Ref<Scene>& Viewport::SaveScene()
    {
        if (m_ActiveScene->GetFileName() == "" || m_SaveFileAs)
        {
            SaveSceneAs();
        }
        else
        {
            const std::string& filepath = m_ActiveScene->GetFilePath();
            IK_INFO("Saving Scene at {0}", filepath.c_str());
            if (!filepath.empty())
            {
                SceneSerializer serializer(m_ActiveScene);
                serializer.Serialize(filepath);
                m_SaveFile = false;
            }
        }
        
        return m_ActiveScene;
    }

    // ******************************************************************************
    // Close the current Scene
    // ******************************************************************************
    void Viewport::CloseScene()
    {
        m_ActiveScene.reset();
        m_ActiveScene = nullptr;
        m_SceneHierarchyPannel.SetContext(nullptr);
    }

    // ******************************************************************************
    // Menu items of Viewport
    // ******************************************************************************
    void Viewport::ShowMenu()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::BeginMenu("Scene"))
                {
                    if (ImGui::MenuItem("New", "Cmd + N"))      NewScene();
                    if (ImGui::MenuItem("Close", "Cmd + X"))    CloseScene();
                    if (ImGui::MenuItem("Save", "Cmd + S", false,    m_ActiveScene != nullptr)) m_SaveFile   = true;
                    if (ImGui::MenuItem("Save As", "Cmd + A", false, m_ActiveScene != nullptr)) m_SaveFileAs = true;

                    ImGui::EndMenu(); // if (ImGui::BeginMenu("Scene"))
                }
                ImGui::Separator();
                
                if (ImGui::MenuItem("Exit", "Cmd + Q"))
                {
                    Application::Get().Close();
                }
                ImGui::EndMenu(); // ImGui::BeginMenu("File")
            } // if (ImGui::BeginMenuBar())

            if (ImGui::BeginMenu("View"))
            {
                ViewMenu();
                ImGui::EndMenu(); // if (ImGui::BeginMenu("View"))
            }

            if (ImGui::BeginMenu("Properties"))
            {
                PropertyMenu();
                ImGui::EndMenu(); // ImGui::BeginMenu("Properties")
            } // if (ImGui::BeginMenu("Properties"))

            ImGui::EndMenuBar(); // ImGui::BeginMenuBar()
        }
    }

    // ******************************************************************************
    // Property menu for Viewport
    // ******************************************************************************
    void Viewport::PropertyMenu()
    {
        if (ImGui::BeginMenu("Theme"))
        {
            if (ImGui::MenuItem("Light Theme", nullptr))
            {
                m_Data.BgColor = {0.9f, 0.9f, 0.9f, 1.0f};
                ImGuiAPI::SetLightThemeColors();
            }
            if (ImGui::MenuItem("Dark Theme", nullptr))
            {
                m_Data.BgColor = {0.1f, 0.1f, 0.1f, 1.0f};
                ImGuiAPI::SetGreyThemeColors();
            }
            ImGui::EndMenu(); // ImGui::BeginMenu("Theme")
        }
    }

    // ******************************************************************************
    // View menu for view port
    // NOTE: this should be called between Imgui::BeginMenue and ImGui::EndMenue();
    // ******************************************************************************
    void Viewport::ViewMenu()
    {
        if (ImGui::MenuItem("Scene Heirarchy Panel", nullptr, m_SceneHierarchyPannel.isSceneHeirarchypanel))
            m_SceneHierarchyPannel.isSceneHeirarchypanel = !m_SceneHierarchyPannel.isSceneHeirarchypanel;

        if (ImGui::MenuItem("Content Browser Panel", nullptr, m_ContentBrowserPannel.IsContentBrowserPannel))
            m_ContentBrowserPannel.IsContentBrowserPannel = !m_ContentBrowserPannel.IsContentBrowserPannel;

        ImGui::Separator();

        if (ImGui::MenuItem("Frame Rate", nullptr, m_Flags.IsFrameRate))
            m_Flags.IsFrameRate = !m_Flags.IsFrameRate;

        if (ImGui::MenuItem("Render Stats", nullptr, m_Flags.IsRendererStats))
            m_Flags.IsRendererStats = !m_Flags.IsRendererStats;

        if (ImGui::MenuItem("Vendor Types", nullptr, m_Flags.IsVendorType))
            m_Flags.IsVendorType = !m_Flags.IsVendorType;

        ImGui::Separator();

        if (ImGui::MenuItem("Renderer Viewport", nullptr, m_Flags.Present))
            m_Flags.Present = !m_Flags.Present;

        if (ImGui::MenuItem("Imgui", nullptr, m_Flags.IsImguiPannel))
            m_Flags.IsImguiPannel = !m_Flags.IsImguiPannel;
        
        ImGui::Separator();
    }
    
    // ******************************************************************************
    // Reender Viewport Imgui pannel, if flag is true then render
    // ******************************************************************************
    void Viewport::OnImguiRenderer(Timestep ts)
    {
        // Show Renderer Stats
        RendererStats(ts);

        if (!m_ActiveScene)
        {
            ShowMenu();
            m_ContentBrowserPannel.OnImguiender(&m_ContentBrowserPannel.IsContentBrowserPannel);

            ImGui::Begin("Warning");
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No Scene is created yet. Scene can be created from 'File->Scene->New' or 'Cmd+N'. Already open scene can be uploaded by dragging the scene file (.iKan) from content browser pannel to this area" );
            ImGui::SameLine(); ImGui::Button("Drop Scene file Here");
            PropertyGrid::DropConent([this](const std::string& path)
                                     {
                OpenScene(path);
            });

            ImGui::End();
            return;
        }
        else
        {
            bool isSceneEdititng = m_ActiveScene->GetNativeDataRef().SceneState == Scene::NativeData::State::Edit;
            
            // Update the Viewport Data
            OnUpdateImGui();

            // Select the type of Scene
            {
                ImGui::Begin("Scene Controler");
                ImGui::Columns(3);
                ImGui::SetColumnWidth(0, 80);
                ImGui::Text("Scene Type");

                ImGui::NextColumn();
                ImGui::SetColumnWidth(1, 200);
                ImGui::PushItemWidth(-1);
                const std::vector <const char*> sceneTypeString = { "2D", "3D" };
                const char* currentSceneType = sceneTypeString[(int32_t)m_ActiveScene->GetSceneType()];
                if (ImGui::BeginCombo("##SceneType", currentSceneType))
                {
                    for (int32_t i = 0; i < sceneTypeString.size(); i++)
                    {
                        bool bIsSelected = currentSceneType == sceneTypeString[i];
                        if (ImGui::Selectable(sceneTypeString[i], bIsSelected))
                        {
                            currentSceneType = sceneTypeString[i];
                            m_ActiveScene->SetSceneType((Scene::NativeData::Type)i);
                        }

                        if (bIsSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::PopItemWidth();

                ImGui::Columns(1);
                ImGui::End();
            }
            
            // Play Pause Buttons
            {
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

                auto& colors = ImGui::GetStyle().Colors;
                const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];

                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
                const auto& buttonActive = colors[ImGuiCol_ButtonActive];
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

                ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
                
                uint32_t texIdx = isSceneEdititng ? m_PlayeTexture->GetRendererID() : m_PauseTexture->GetRendererID();
                float size = ImGui::GetWindowHeight() - 4.0f; // 4 just random number
                ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
                if (PropertyGrid::ImageButton("Pause", texIdx, ImVec2(size, size)))
                {
                    if (isSceneEdititng)
                        OnScenePlay();
                    else
                        OnSceneEdit();
                }
                
                ImGui::PopStyleVar(2);
                ImGui::PopStyleColor(3);
                ImGui::End();
            }
            
//            if (isSceneEdititng)
            {
                ShowMenu();
                m_ContentBrowserPannel.OnImguiender(&m_ContentBrowserPannel.IsContentBrowserPannel);

                // Renderer Viewport Properties
                RendereViewportProp();

                // Render Scene Hierarchy pannel in imgui
                m_SceneHierarchyPannel.OnImguiender();
            }
        }
    }
    
    // ******************************************************************************
    // Edit the Scene
    // ******************************************************************************
    void Viewport::OnSceneEdit()
    {
        m_ActiveScene->OnRuntimeStop();
        m_ActiveScene = m_EditorScene;
    }
    
    // ******************************************************************************
    // Play The Scene
    // ******************************************************************************
    void Viewport::OnScenePlay()
    {
        m_ActiveScene = Scene::Copy(m_EditorScene);
        m_ActiveScene->OnRuntimeStart();
    }
    
    // ******************************************************************************
    // Duplicate the entity
    // ******************************************************************************
    void Viewport::OnDuplicateEntity()
    {
        if (m_ActiveScene->GetSceneState() != Scene::NativeData::State::Edit)
            return;
        
        Entity entity = m_SceneHierarchyPannel.GetSelectedEntity();
        if (entity)
            m_EditorScene->DuplicateScene(entity);
    }

    // ******************************************************************************
    // Reender Viewport Imgui pannel, if flag is true then render
    // ******************************************************************************
    void Viewport::RendereViewportProp()
    {
        // No Imgui renderer if flag is false
        if (!m_Flags.IsImguiPannel)
            return;

        // Basic Properties
        ImGui::Begin("Viewport Properties", &m_Flags.IsImguiPannel);
        ImGui::PushID("Viewport Properties");

        ImGui::Columns(3);

        ImGui::SetColumnWidth(0, 80);
        ImGui::Text("Present : %d", m_Flags.Present);
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 80);
        ImGui::Text("Focused : %d", m_Data.Focused);
        ImGui::NextColumn();

        ImGui::SetColumnWidth(2, 80);
        ImGui::Text("Hovered : %d", m_Data.Hovered);
        ImGui::NextColumn();

        ImGui::Columns(1);
        ImGui::Separator();

        // Scene details should not be present if scene is not created yet
        if (m_ActiveScene)
        {
            // Option for Background color
            static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Selected;
            bool bgOpened = ImGui::TreeNodeEx((void*)1234567, flags, "Background Color");
            if (bgOpened)
            {
                ImGuiAPI::ColorEdit(m_Data.BgColor);
                ImGui::TreePop();
            }
            ImGui::Separator();

            // Other Properties
            ImGui::Columns(2);

            float columnWidth = 150;

            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::Text("Scene Size ");
            ImGui::NextColumn();

            ImGui::SetColumnWidth(1, columnWidth);
            ImGui::Text("%d x %d", (int32_t)m_Data.Width,  (int32_t)m_Data.Height);
            ImGui::NextColumn();

            ImGui::Columns(2);

            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::Text("Viewport Size ");
            ImGui::NextColumn();

            ImGui::SetColumnWidth(1, columnWidth);
            ImGui::Text("%d x %d", (int32_t)m_Data.Size.x, (int32_t)m_Data.Size.y);
            ImGui::NextColumn();

            ImGui::Columns(2);

            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::Text("Top Left Position ");
            ImGui::NextColumn();

            ImGui::SetColumnWidth(1, columnWidth);
            ImGui::Text("%d x %d", (int32_t)m_Data.Bounds[0].x, (int32_t)m_Data.Bounds[0].y);
            ImGui::NextColumn();

            ImGui::Columns(2);

            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::Text("Bottom Right Position ");
            ImGui::NextColumn();

            ImGui::SetColumnWidth(1, columnWidth);
            ImGui::Text("%d x %d", (int32_t)m_Data.Bounds[1].x, (int32_t)m_Data.Bounds[1].y);
            ImGui::NextColumn();

            ImGui::Columns(2);

            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::Text("Mouse Position ");
            ImGui::NextColumn();

            ImGui::SetColumnWidth(1, columnWidth);
            ImGui::Text("%d x %d", m_Data.MousePosX, m_Data.MousePosY);
            ImGui::NextColumn();

            ImGui::Columns(1);

            ImGui::Separator();

            // SHow the Hovered Eneity
            std::string entityName = "NULL";
            if ((entt::entity)m_Data.HoveredEntity != entt::null)
            {
                entityName = m_Data.HoveredEntity.GetComponent<TagComponent>().Tag;

                ImGui::Text("Hovered Entity");
                PropertyGrid::String("Entity ID", (uint32_t)m_Data.HoveredEntity, columnWidth);
                PropertyGrid::String("Unique ID", (uint32_t)m_Data.HoveredEntity.GetComponent<IDComponent>().ID, columnWidth);
                PropertyGrid::String("Entity Name", entityName, columnWidth, 300.0f, nullptr, false); // No need to add any Hint in non modifiable string
                ImGui::Separator();
            }
        }
        ImGui::PopID();
        ImGui::End();
    }

    // ******************************************************************************
    // Show the renderer stats
    // ******************************************************************************
    void Viewport::RendererStats(Timestep ts)
    {
        if (!m_ActiveScene)
            return;
        
        if (m_ActiveScene->GetSceneState() == Scene::NativeData::State::Edit)
        {
            if (m_Flags.IsRendererStats)
                ImGuiAPI::RendererStats(&m_Flags.IsRendererStats);
            
            if (m_Flags.IsVendorType)
                ImGuiAPI::RendererVersion(&m_Flags.IsVendorType);
        }

        if (m_Flags.IsFrameRate)
            ImGuiAPI::FrameRate(ts, &m_Flags.IsFrameRate);
    }

}
