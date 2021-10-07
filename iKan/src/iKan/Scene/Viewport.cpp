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

            UpdateBounds();
        }

        ImGui::End(); // ImGui::Begin("Viewport");
        ImGui::PopStyleVar(); // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
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

            if (m_ActiveScene->GetDataRef().Editing)
                m_ActiveScene->OnUpdateEditor(ts);
            else
                m_ActiveScene->OnUpdateRuntime(ts);

            if (m_ActiveScene->IsEditing())
            {
                // Update selected entity
                if (m_Data.SelectedEntity != Entity(entt::null, nullptr))
                    m_SceneHierarchyPannel.SetSelectedEntity(m_Data.SelectedEntity);
                
                // Update Viewprt entities
                UpdateMousePos();
                UpdateHoveredEntity();
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
            m_Data.HoveredEntity = (m_Data.HoveredEntityID >= m_ActiveScene->GetNumEntities()) ? Entity() : Entity((entt::entity)m_Data.HoveredEntityID, m_ActiveScene.get());
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
                m_Data.SelectedEntity = m_Data.HoveredEntity;
            else
                m_Data.SelectedEntity = {};
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

        bool cmd   = Input::IsKeyPressed(KeyCode::LeftSuper) || Input::IsKeyPressed(KeyCode::RightSuper);
        switch (event.GetKeyCode())
        {
            case KeyCode::S:    if (cmd && m_ActiveScene)   m_SaveFile   = true;    break;
            case KeyCode::A:    if (cmd && m_ActiveScene)   m_SaveFileAs = true;    break;

            case KeyCode::N:    if (cmd)    NewScene();     break;
            case KeyCode::X:    if (cmd)    CloseScene();   break;
            default:    break;
        }
        return false;
    }

    // ******************************************************************************
    // Create new active scene to the Viewport
    // ******************************************************************************
    Ref<Scene> Viewport::NewScene()
    {
        CloseScene();
        
        m_ActiveScene = CreateRef<Scene>();
        m_ActiveScene->OnViewportResize((uint32_t)m_Data.Size.x, (uint32_t)m_Data.Size.y);
        
        // Set the current Scene to scene hierarchy pannel
        m_SceneHierarchyPannel.SetContext(m_ActiveScene);

        IK_INFO("New scene is created");
        
        return m_ActiveScene;
    }

    // ******************************************************************************
    // Open saved scene
    // ******************************************************************************
    Ref<Scene> Viewport::OpenScene(const std::string& path)
    {
        IK_INFO("Opening saved scene from {0}", path.c_str());
        if (!path.empty())
        {
            m_ActiveScene = CreateRef<Scene>(path);
            m_ActiveScene->OnViewportResize((uint32_t)m_Data.Size.x, (uint32_t)m_Data.Size.y);
            m_SceneHierarchyPannel.SetContext(m_ActiveScene);

            SceneSerializer serializer(m_ActiveScene);
            serializer.Deserialize(path);
        }
        
        return m_ActiveScene;
    }

    // ******************************************************************************
    // Saving Scene to new file
    // ******************************************************************************
    Ref<Scene> Viewport::SaveSceneAs(const std::string& path)
    {
        if (path != "")
        {
            m_ActiveScene->SetFilePath(path);
            SceneSerializer serializer(m_ActiveScene);
            serializer.Serialize(path);
            return m_ActiveScene;
        }
        
        if (!m_SaveFileAs && !m_SaveFile)
            return nullptr;
        
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
    Ref<Scene> Viewport::SaveScene()
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
                ContentBrowserPannel::UpdateTileBgColor();
            }
            if (ImGui::MenuItem("Dark Theme", nullptr))
            {
                m_Data.BgColor = {0.1f, 0.1f, 0.1f, 1.0f};
                ImGuiAPI::SetGreyThemeColors();
                ContentBrowserPannel::UpdateTileBgColor();
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
            bool &isSceneEdititng = m_ActiveScene->GetDataRef().Editing;

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
                            m_ActiveScene->SetSceneType((Scene::Data::Type)i);
                        }

                        if (bIsSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::PopItemWidth();

                // Play Pause Icon
                ImGui::NextColumn();
                ImGui::SetColumnWidth(2, 50);
                uint32_t pauseTexId = m_PauseTexture->GetRendererID(), playTexId = m_PlayeTexture->GetRendererID();

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

                ImGui::Columns(1);
                ImGui::End();
            }
            
            if (isSceneEdititng)
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
        
        if (m_ActiveScene->IsEditing())
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
