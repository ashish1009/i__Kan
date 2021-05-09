// ******************************************************************************
// File         : EditorLayer.cpp
// Description  : Client side Application
// Project      : Editor
//
// Created by Ashish on 13/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "EditorLayer.h"

EditorLayer::PropertyFlag EditorLayer::s_PropFlag;

// ******************************************************************************
// EditorLayer Constructor
// ******************************************************************************
EditorLayer::EditorLayer()
: Layer("Editor"), m_EditorCamera(glm::radians(45.0f), 1800.0f/800.0f, 0.01f, 10000.0f)
{
    IK_INFO("Editor layer created");
        
    m_ActiveScene = CreateRef<Scene>();
    
    // Frame buffer specifications
    Framebuffer::Specification specs;
    specs.Attachments = { Framebuffer::TextureSpecification::TextureFormat::RGBA8,
                          Framebuffer::TextureSpecification::TextureFormat::DEPTH24STENCIL8,
                          Framebuffer::TextureSpecification::TextureFormat::R32I};
    
    specs.Width       = Window::Property::DefaultWidth;
    specs.Height      = Window::Property::DefaultHeight;
    
    // Creating instance for Frame buffer in viewport
    m_Viewport.FrameBuffer = Framebuffer::Create(specs);
    
    // Set the current Scene to scene hierarchy pannel
    m_SceneHierarchyPannel.SetContext(m_ActiveScene);

    // Creating Temp Entity
    auto ent1 = m_ActiveScene->CreateEntity("Entity");
    ent1.GetComponent<TransformComponent>().Translation = glm::vec3(0.5, 0.0, 0.0);
    ent1.AddComponent<SpriteRendererComponent>(glm::vec4(1.0f));
}

// ******************************************************************************
// EditorLayer Destructor
// ******************************************************************************
EditorLayer::~EditorLayer()
{
    IK_WARN("Editor layer destroyed");
}

// ******************************************************************************
// Attach Scene Editor
// ******************************************************************************
void EditorLayer::OnAttach()
{
    IK_INFO("Attaching {0} Layer to Application", GetName().c_str());
}

// ******************************************************************************
// Dettach Scene Editor
// ******************************************************************************
void EditorLayer::OnDetach()
{
    IK_WARN("Detaching {0} Layer", GetName().c_str());
}

// ******************************************************************************
// Update Editor
// ******************************************************************************
void EditorLayer::OnUpdate(Timestep ts)
{
    // If resize the window call the update the Scene View port and Frame buffer should be resized
    if (Framebuffer::Specification spec = m_Viewport.FrameBuffer->GetSpecification();
        m_Viewport.Size.x > 0.0f && m_Viewport.Size.y > 0.0f && // zero sized framebuffer is invalid
        (spec.Width != m_Viewport.Size.x || spec.Height != m_Viewport.Size.y))
    {
        m_Viewport.FrameBuffer->Resize((uint32_t)m_Viewport.Size.x, (uint32_t)m_Viewport.Size.y);
        m_ActiveScene->OnViewportResize((uint32_t)m_Viewport.Size.x, (uint32_t)m_Viewport.Size.y);
        m_EditorCamera.SetViewportSize((uint32_t)m_Viewport.Size.x, (uint32_t)m_Viewport.Size.y);
    }

    RendererStatistics::Reset();
    m_EditorCamera.OnUpdate(ts);
    
    m_Viewport.FrameBuffer->Bind();
    {
        Renderer::Clear({ 0.1f, 0.1f, 0.1f, 1.0f });
        m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);

        m_Viewport.OnUpdate(m_ActiveScene);
    }
    m_Viewport.FrameBuffer->Unbind();
}

// ******************************************************************************
// Imgui Render for Scene Editor
// ******************************************************************************
void EditorLayer::OnImguiRender()
{
    ImGuiAPI::StartDcocking();
    
    ShowMenu();
    RendererStats();

    m_SceneHierarchyPannel.SetSelectedEntity(m_Viewport.SelectedEntity);

    // Render Scene Hierarchy pannel in imgui
    if (s_PropFlag.IsSceneHeirarchypanel)
    {
        m_SceneHierarchyPannel.OnImguiender(&s_PropFlag.IsSceneHeirarchypanel);
    }
    
    // Viewport Update
    if (m_Viewport.Present)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Viewport", &m_Viewport.Present);
        {
            m_Viewport.OnUpdateImGui();
        }

        ImGui::End(); // ImGui::Begin("Viewport");
        ImGui::PopStyleVar(); // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    }

    // Viewport Imgui Renderer
    if (m_Viewport.IsImguiPannel)
    {
        m_Viewport.OnImguiRenderer();
    }

    // Editor Camera Imgui Renderer
    if (m_EditorCamera.IsImguiPannel)
    {
        m_EditorCamera.OnImguiRenderer();
    }
    
    ImGuiAPI::EndDcocking();
}

// ******************************************************************************
// Scene Editor Events
// ******************************************************************************
void EditorLayer::OnEvent(Event& event)
{
    m_EditorCamera.OnEvent(event);
    m_Viewport.OnEvent(event);
}

// ******************************************************************************
// Menu for Imgui View port
// ******************************************************************************
void EditorLayer::ShowMenu()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit", "Cmd + Q"))
            {
                Application::Get().Close();
            }
            ImGui::EndMenu(); // ImGui::BeginMenu("File")
        } // if (ImGui::BeginMenuBar())
        
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Scene Heirarchy Panel", NULL, EditorLayer::s_PropFlag.IsSceneHeirarchypanel))
            {
                EditorLayer::s_PropFlag.IsSceneHeirarchypanel = !EditorLayer::s_PropFlag.IsSceneHeirarchypanel;
            }

            if (ImGui::MenuItem("Frame Rate", NULL, EditorLayer::s_PropFlag.IsFrameRate))
            {
                EditorLayer::s_PropFlag.IsFrameRate = !EditorLayer::s_PropFlag.IsFrameRate;
            }

            if (ImGui::MenuItem("Render Stats", NULL, EditorLayer::s_PropFlag.IsRendererStats))
            {
                EditorLayer::s_PropFlag.IsRendererStats = !EditorLayer::s_PropFlag.IsRendererStats;
            }

            if (ImGui::MenuItem("Vendor Types", NULL, EditorLayer::s_PropFlag.IsVendorType))
            {
                EditorLayer::s_PropFlag.IsVendorType = !EditorLayer::s_PropFlag.IsVendorType;
            }
            
            if (ImGui::MenuItem("Editor Camera Imgui Pannel", nullptr, m_EditorCamera.IsImguiPannel))
            {
                m_EditorCamera.IsImguiPannel = !m_EditorCamera.IsImguiPannel;
            }

            if (ImGui::BeginMenu("Viewport"))
            {
                if (ImGui::MenuItem("Present", nullptr, m_Viewport.Present))
                {
                    m_Viewport.Present = !m_Viewport.Present;
                }

                if (ImGui::MenuItem("Imgui", nullptr, m_Viewport.IsImguiPannel))
                {
                    m_Viewport.IsImguiPannel = !m_Viewport.IsImguiPannel;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu(); // if (ImGui::BeginMenu("View"))
        }
        
        if (ImGui::BeginMenu("Properties"))
        {
            ImGui::EndMenu(); // ImGui::BeginMenu("Properties")
        } // if (ImGui::BeginMenu("Properties"))
        
        ImGui::EndMenuBar(); // ImGui::BeginMenuBar()
    }
}

// ******************************************************************************
// Show the renderer stats
// ******************************************************************************
void EditorLayer::RendererStats()
{
    ImGui::ShowDemoWindow();
    if (EditorLayer::s_PropFlag.IsFrameRate)
    {
        ImGuiAPI::FrameRate(&EditorLayer::s_PropFlag.IsFrameRate);
    }

    if (EditorLayer::s_PropFlag.IsRendererStats)
    {
        ImGuiAPI::RendererStats(&EditorLayer::s_PropFlag.IsRendererStats);
    }
    
    if (EditorLayer::s_PropFlag.IsVendorType)
    {
        ImGuiAPI::RendererVersion(&EditorLayer::s_PropFlag.IsVendorType);
    }
}
