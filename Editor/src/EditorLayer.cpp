// ******************************************************************************
// File         : EditorLayer.cpp
// Description  : Client side Application
// Project      : Editor
//
// Created on   : 13/04/21.
// ******************************************************************************

#include "EditorLayer.h"

// ******************************************************************************
// EditorLayer Constructor
// ******************************************************************************
EditorLayer::EditorLayer()
: Layer("Editor"), m_EditorCamera(glm::radians(45.0f), 1800.0f/800.0f, 0.01f, 10000.0f)
{
    IK_INFO("Editor layer created");
    
    m_ActiveScene = CreateRef<Scene>();
    
    auto ent1 = m_ActiveScene->CreateEntity();
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
    RendererStatistics::Reset();

    m_EditorCamera.OnUpdate(ts);
    
    Renderer::Clear({ 0.1f, 0.1f, 0.1f, 1.0f });
    
    m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
}

// ******************************************************************************
// Imgui Render for Scene Editor
// ******************************************************************************
void EditorLayer::OnImguiRender()
{
    ImGuiAPI::StartDcocking();
    
    ShowMenu();
    
    ImGuiAPI::FrameRate();
    ImGuiAPI::RendererStats();
    ImGuiAPI::RendererVersion();
    
    ImGuiAPI::EndDcocking();
}

// ******************************************************************************
// Scene Editor Events
// ******************************************************************************
void EditorLayer::OnEvent(Event& event)
{
    m_EditorCamera.OnEvent(event);
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
            if (ImGui::MenuItem("Exit"))
            {
                Application::Get().Close();
            }
            ImGui::EndMenu(); // ImGui::BeginMenu("File")
        } // if (ImGui::BeginMenuBar())
        
        if (ImGui::BeginMenu("Properties"))
        {
            ImGui::EndMenu(); // ImGui::BeginMenu("Properties")
        } // if (ImGui::BeginMenu("Properties"))
        
        ImGui::EndMenuBar(); // ImGui::BeginMenuBar()
    }
}
