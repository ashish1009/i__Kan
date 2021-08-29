// ******************************************************************************
// File         : MarioLayer.cpp
// Description  : Client (Mario) Main Layer to be added in iKan Application
// Project      : Mario
//
// Created by Ashish on 05/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "MarioLayer.h"
#include "Background.h"
#include "StartScreen.h"

namespace Mario {
    // ******************************************************************************
    // MarioLayer Constructor
    // ******************************************************************************
    MarioLayer::MarioLayer()
    : Layer("Mario")
    {
        IK_INFO("{0} Layer created", GetName().c_str());
    }

    // ******************************************************************************
    // MarioLayer Destructor
    // ******************************************************************************
    MarioLayer::~MarioLayer()
    {
        IK_WARN("{0} Layer destroyed", GetName().c_str());
    }

    // ******************************************************************************
    // Attach Scene Editor
    // ******************************************************************************
    void MarioLayer::OnAttach()
    {
        IK_INFO("Attaching {0} Layer to Application", GetName().c_str());

        m_Viewport.NewScene();

        auto scene  = m_Viewport.GetScene();
        // Setup the Camera Entity
        m_CameraEntity        = scene->CreateEntity("Primary Camera");
        auto& cameraComponent = m_CameraEntity.AddComponent<CameraComponent>();
        cameraComponent.Camera.SetProjectionType(SceneCamera::ProjectionType::Orthographic);
        cameraComponent.Camera.SetOrthographicSize(18.0f);

        auto& cameraPositionX = m_CameraEntity.GetComponent<TransformComponent>().Translation.x;
        cameraPositionX = 18.0f;

        // Creating Entities for background tiles
        Mario::Background::CreateEntities(scene);

        // Creating Entities for background tiles
        Mario::StartScreen::CreateEntities(scene);

        // Create Player Instance
        // TODO: Add multi player Concept here
        m_Player = CreateRef<Mario::Player>(scene);
    }

    // ******************************************************************************
    // Dettach Scene Editor
    // ******************************************************************************
    void MarioLayer::OnDetach()
    {
        IK_WARN("Detaching {0} Layer", GetName().c_str());
    }

    // ******************************************************************************
    // Update Editor
    // ******************************************************************************
    void MarioLayer::OnUpdate(Timestep ts)
    {
        m_Viewport.OnUpdate(ts);
        m_Player->OnUpdate(ts);
    }

    // ******************************************************************************
    // Imgui Render for Scene Editor
    // ******************************************************************************
    void MarioLayer::OnImguiRender(Timestep ts)
    {
        ImGuiAPI::StartDcocking();

        // Viewport Imgui Renderer
        m_Viewport.OnImguiRenderer(ts);

        // Mario Setting pannel
//        if (m_Viewport.GetScene()->IsEditing() && m_IsSetting)
        {
            ShowMenu();

            ImGui::Begin("Mario Setting", &m_IsSetting, ImGuiWindowFlags_HorizontalScrollbar);

            // Background Imgui Rendeer
            Background::ImGuiRenderer();

            // Imgui rendering for Player
            m_Player->ImguiRenderer();

            ImGui::End();
        }

        ImGuiAPI::EndDcocking();
    }

    // ******************************************************************************
    // Scene Editor Events
    // ******************************************************************************
    void MarioLayer::OnEvent(Event& event)
    {
        m_Viewport.OnEvent(event);
        m_Player->OnEvent(event);
    }

    // ******************************************************************************
    // Show Mario Menu
    // ******************************************************************************
    void MarioLayer::ShowMenu()
    {
        if (!m_Viewport.GetScene()->IsEditing())
            return;
            
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Mario"))
            {
                if (ImGui::BeginMenu("View"))
                {
                    if (ImGui::MenuItem("backgroud Setting", nullptr, m_IsSetting))
                        m_IsSetting = !m_IsSetting;
                                        
                    ImGui::EndMenu(); // if (ImGui::BeginMenu("Scene"))
                }
                ImGui::Separator();

                ImGui::EndMenu(); // ImGui::BeginMenu("File")
            } // if (ImGui::BeginMenuBar())
            ImGui::EndMenuBar(); // ImGui::BeginMenuBar()
        }
    }

}
