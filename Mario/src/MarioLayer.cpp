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
#include "PlayerController.h"

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

        m_ActiveScene = m_Viewport.NewScene();
        
        // Creating Entities for background tiles
        Mario::Background::CreateEntities(m_ActiveScene);
        
        // Creating Entities for background tiles
//        Mario::StartScreen::CreateEntities(m_ActiveScene);
        
        {
            // Setup the Camera Entity
            m_PrimaryCameraEntity = m_ActiveScene->CreateEntity("Primary Camera");
            m_PrimaryCameraEntity.GetComponent<TransformComponent>().Translation.x = 18.0f;

            auto& primaryCameraComponent = m_PrimaryCameraEntity.AddComponent<CameraComponent>();
            primaryCameraComponent.Camera.SetProjectionType(SceneCamera::ProjectionType::Orthographic);
            primaryCameraComponent.Camera.SetOrthographicSize(18.0f);
        }
        
        {
            m_EditorCameraEntity = m_ActiveScene->CreateEntity("Editor Camera");
            m_EditorCameraEntity.GetComponent<TransformComponent>().Translation.x = 18.0f;
            
            auto& editorCameraComponent = m_EditorCameraEntity.AddComponent<CameraComponent>();
            editorCameraComponent.Camera.SetProjectionType(SceneCamera::ProjectionType::Orthographic);
            editorCameraComponent.Camera.SetOrthographicSize(18.0f);
            editorCameraComponent.MakeEditor();
        }
        
        {
            m_PlayerEntity = m_ActiveScene->CreateEntity("Player 1");
            
            m_PlayerEntity.GetComponent<BoxCollider2DComponent>().IsRigid = true;
            m_PlayerEntity.AddComponent<SpriteRendererComponent>();
            m_PlayerEntity.AddComponent<NativeScriptComponent>().Bind<PlayerController>(m_ActiveScene, "PlayerController");
            m_PlayerEntity.AddComponent<AliveComponent>().Init(true, AliveComponent::ComponentType::Player);
            
            auto& position = m_PlayerEntity.GetComponent<TransformComponent>().Translation;
            position.x = 16.0f;
        }
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
        if (m_Viewport.GetScene() && m_Viewport.GetScene()->IsEditing() && m_IsSetting)
        {
            ShowMenu();

            ImGui::Begin("Mario Setting", &m_IsSetting, ImGuiWindowFlags_HorizontalScrollbar);

            // Background Imgui Rendeer
            Background::ImGuiRenderer();

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
    }

    // ******************************************************************************
    // Show Mario Menu
    // ******************************************************************************
    void MarioLayer::ShowMenu()
    {
        if (!m_Viewport.GetScene())
            return;
        
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
