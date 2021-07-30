// ******************************************************************************
// File         : IKanEditor.cpp
// Descrription : Client Side Layer
// Project      : Editor
//
// Created by Ashish on 14/06/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "IKanEditor.h"

// ******************************************************************************
// EditorLayer Constructor
// ******************************************************************************
IKanEditor::IKanEditor()
: Layer("IKanEditor")
{
    IK_INFO("IKanEditor layer created");
}

// ******************************************************************************
// IKanEditorLayer Destructor
// ******************************************************************************
IKanEditor::~IKanEditor()
{
    IK_WARN("IKanEditor destroyed");
}

// ******************************************************************************
// Attach Scene Editor
// ******************************************************************************
void IKanEditor::OnAttach()
{
    IK_INFO("Attaching {0} Layer to Application", GetName().c_str());
}

// ******************************************************************************
// Dettach Scene Editor
// ******************************************************************************
void IKanEditor::OnDetach()
{
    IK_WARN("Detaching {0} Layer", GetName().c_str());
}

// ******************************************************************************
// Update Editor
// ******************************************************************************
void IKanEditor::OnUpdate(Timestep ts)
{
    m_Viewport.OnUpdate(ts);
}

// ******************************************************************************
// Imgui Render for Scene Editor
// ******************************************************************************
void IKanEditor::OnImguiRender(Timestep ts)
{
    ImGuiAPI::StartDcocking();

    // Viewport Imgui Renderer
    m_Viewport.OnImguiRenderer(ts);

    ImGuiAPI::EndDcocking();
}

// ******************************************************************************
// Scene Editor Events
// ******************************************************************************
void IKanEditor::OnEvent(Event& event)
{
    m_Viewport.OnEvent(event);
}
