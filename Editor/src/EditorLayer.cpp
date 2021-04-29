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
    m_EditorCamera.OnUpdate(ts);
    
    Renderer::Clear({ 0.1f, 0.1f, 0.1f, 1.0f });
}

// ******************************************************************************
// Imgui Render for Scene Editor
// ******************************************************************************
void EditorLayer::OnImguiRender()
{
    
}

// ******************************************************************************
// Scene Editor Events
// ******************************************************************************
void EditorLayer::OnEvent(Event& event)
{
    m_EditorCamera.OnEvent(event);
}
