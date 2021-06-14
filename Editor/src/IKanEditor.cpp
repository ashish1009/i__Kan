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

}

// ******************************************************************************
// Imgui Render for Scene Editor
// ******************************************************************************
void IKanEditor::OnImguiRender(Timestep ts)
{
    ImGuiAPI::StartDcocking();
    ShowMenu();
    ImGuiAPI::EndDcocking();
}

// ******************************************************************************
// Scene Editor Events
// ******************************************************************************
void IKanEditor::OnEvent(Event& event)
{

}

// ******************************************************************************
// Menu for Imgui View port
// ******************************************************************************
void IKanEditor::ShowMenu()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Cmd + Q"))
            {
                Application::Get().Close();
            }
            ImGui::EndMenu(); // ImGui::BeginMenu("File")
        } // ImGui::BeginMenu("File")

        if (ImGui::BeginMenu("View"))
        {
            ImGui::EndMenu(); // if (ImGui::BeginMenu("View"))
        } // if (ImGui::BeginMenu("View"))

        if (ImGui::BeginMenu("Properties"))
        {
            ImGui::EndMenu(); // ImGui::BeginMenu("Properties")
        } // if (ImGui::BeginMenu("Properties"))

        ImGui::EndMenuBar(); // ImGui::BeginMenuBar()
    }
}

