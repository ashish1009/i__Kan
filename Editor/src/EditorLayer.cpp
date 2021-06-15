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
: Layer("Editor")
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
    m_Viewport.Data.FrameBuffer = Framebuffer::Create(specs);
    
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
    m_Viewport.OnUpdate(m_ActiveScene, ts, {0.1, 0.1, 0.1, 1.0});
}

// ******************************************************************************
// Imgui Render for Scene Editor
// ******************************************************************************
void EditorLayer::OnImguiRender(Timestep ts)
{
    ImGuiAPI::StartDcocking();
    
    ShowMenu();
    RendererStats(ts);

    // Render Scene Hierarchy pannel in imgui
    m_SceneHierarchyPannel.OnImguiender(&s_PropFlag.IsSceneHeirarchypanel);
    
    // Viewport Imgui Renderer
    m_Viewport.OnImguiRenderer(ts);
    
    ImGuiAPI::EndDcocking();
}

// ******************************************************************************
// Scene Editor Events
// ******************************************************************************
void EditorLayer::OnEvent(Event& event)
{
    m_Viewport.OnEvent(event);
    m_ActiveScene->OnEvent(event);
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

            m_Viewport.ViewMenu();

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
void EditorLayer::RendererStats(Timestep ts)
{
    if (EditorLayer::s_PropFlag.IsFrameRate)
    {
        ImGuiAPI::FrameRate(ts, &EditorLayer::s_PropFlag.IsFrameRate);
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
