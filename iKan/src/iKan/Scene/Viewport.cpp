// ******************************************************************************
// File         : Viewport.cpp
// Description  : stores all the information of Viewport
// Project      : iKan
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

namespace iKan {

    // ******************************************************************************
    // Viewport Default Constructor
    // ******************************************************************************
    Viewport::Viewport()
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
            m_ActiveScene->OnUpdateRuntime(ts);

            // Update selected entity
            if (m_Data.SelectedEntity != Entity(entt::null, nullptr))
            {
                m_SceneHierarchyPannel.SetSelectedEntity(m_Data.SelectedEntity);
            }

            UpdateMousePos();
            UpdateHoveredEntity();
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
            int32_t ID = m_ActiveScene->GetEntityIdFromPixels(m_Data.MousePosX, m_Data.MousePosY);
            m_Data.HoveredEntity = (ID >= m_ActiveScene->GetNumEntities()) ? Entity() : Entity((entt::entity)ID, m_ActiveScene.get());
        }
    }

    // ******************************************************************************
    // Scene Editor Events
    // ******************************************************************************
    void Viewport::OnEvent(Event& event)
    {
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
            {
                m_Data.SelectedEntity = m_Data.HoveredEntity;
            }
            else
            {
                m_Data.SelectedEntity = {};
            }
        }
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
            case KeyCode::N:    if (cmd)    NewScene();     break;
            case KeyCode::O:    if (cmd)    OpenScene();    break;
            case KeyCode::S:    if (cmd)    SaveScene();    break;
            case KeyCode::X:    if (cmd)    CloseScene();    break;
            default:                                        break;
        }
        return false;
    }

    // ******************************************************************************
    // Create new active scene to the Viewport
    // ******************************************************************************
    void Viewport::NewScene()
    {
        m_ActiveScene = CreateRef<Scene>();
        m_ActiveScene->OnViewportResize((uint32_t)m_Data.Size.x, (uint32_t)m_Data.Size.y);
        
        // Set the current Scene to scene hierarchy pannel
        m_SceneHierarchyPannel.SetContext(m_ActiveScene);

        IK_INFO("New scene is created");
    }

    // ******************************************************************************
    // Open saved scene
    // ******************************************************************************
    void Viewport::OpenScene()
    {

    }

    // ******************************************************************************
    // Saving Scene
    // ******************************************************************************
    void Viewport::SaveScene()
    {

    }

    // ******************************************************************************
    // Close the current Scene
    // ******************************************************************************
    void Viewport::CloseScene()
    {
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
                    if (ImGui::MenuItem("Open", "Cmd + O"))     OpenScene();
                    if (ImGui::MenuItem("Save", "Cmd + S"))     SaveScene();
                    if (ImGui::MenuItem("Close", "Cmd + X"))    CloseScene();

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
        ShowMenu();

        // Renderer Viewport Properties
        RendereViewportProp();

        // Show Renderer Stats
        RendererStats(ts);

        // Render Content browser pannel
        m_ContentBrowserPannel.OnImguiender();

        if (!m_ActiveScene)
        {
            ImGui::Begin("Warning");
            ImGui::Text("No Scene is created yet. Scene can be created from File->Scene->New or Cmd+N");
            ImGui::End();
            return;
        }

        // Render Scene Hierarchy pannel in imgui
        m_SceneHierarchyPannel.OnImguiender();

        // Update the Viewport Data
        OnUpdateImGui();
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

        // Option for Background color
        static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_SpanAvailWidth;
        bool bgOpened = ImGui::TreeNodeEx((void*)1234567, flags, "Background Color");
        if (bgOpened)
        {
            ImGuiAPI::ColorEdit(m_Data.BgColor);
            ImGui::TreePop();
        }
        ImGui::Separator();

        // Other Properties
        ImGui::Columns(2);

        float columnWidth = 120;

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
            PropertyGrid::String("Entity Name", entityName, columnWidth, 300.0f, "", false); // No need to add any Hint in non modifiable string
            ImGui::Separator();
        }

        ImGui::PopID();
        ImGui::End();

    }

    // ******************************************************************************
    // Show the renderer stats
    // ******************************************************************************
    void Viewport::RendererStats(Timestep ts)
    {
        if (m_Flags.IsFrameRate)
        {
            ImGuiAPI::FrameRate(ts, &m_Flags.IsFrameRate);
        }

        if (m_Flags.IsRendererStats)
        {
            ImGuiAPI::RendererStats(&m_Flags.IsRendererStats);
        }

        if (m_Flags.IsVendorType)
        {
            ImGuiAPI::RendererVersion(&m_Flags.IsVendorType);
        }
    }

}
