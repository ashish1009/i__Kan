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
    // If no scene is present the return the update funciton
    if (!m_ActiveScene)
        return;
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
// Mouse button press event
// ******************************************************************************
bool IKanEditor::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
    return false;
}

// ******************************************************************************
// key press events
// ******************************************************************************
bool IKanEditor::OnKeyPressed(KeyPressedEvent& event)
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
        default:                                        break;
    }
    return false;
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
            if (ImGui::BeginMenu("Scene"))
            {
                if (ImGui::MenuItem("New", "Cmd + N"))      NewScene();
                if (ImGui::MenuItem("Open", "Cmd + O"))     OpenScene();
                if (ImGui::MenuItem("Save", "Cmd + S"))     SaveScene();

                ImGui::EndMenu(); // if (ImGui::BeginMenu("Scene"))
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Cmd + Q"))         Application::Get().Close();

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

// ******************************************************************************
// New scene
// ******************************************************************************
void IKanEditor::NewScene()
{
    m_ActiveScene = CreateRef<Scene>();
}

// ******************************************************************************
// Open saved scene
// ******************************************************************************
void IKanEditor::OpenScene()
{

}

// ******************************************************************************
// Saving Scene
// ******************************************************************************
void IKanEditor::SaveScene()
{

}


