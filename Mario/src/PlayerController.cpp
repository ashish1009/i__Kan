// ******************************************************************************
// File         : Player.cpp
// Descrition   : Player Class info
// Project      : Mario
//
// Created by Ashish on 10/06/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "PlayerController.h"

namespace Mario {
    
    float PlayerController::s_RunningSpeed = 0.10;
    float PlayerController::s_FallingSpeed = 0.50;
    float PlayerController::s_JumpingSpeed = 0.25;
    
    // ******************************************************************************
    // Player Constructor
    // ******************************************************************************
    PlayerController::PlayerController(const Ref<Scene>& scene)
    : ScriptableEntity(scene)
    {
        IK_INFO("Mario Player Constructor called");
    }
    
    // ******************************************************************************
    // Player Destructor
    // ******************************************************************************
    PlayerController::~PlayerController()
    {
        IK_WARN("Mario Player Destructor called");
    }
    
    // ******************************************************************************
    // Create the Player Script Instance
    // ******************************************************************************
    void PlayerController::OnCreate()
    {
        Init();
    }
    
    // ******************************************************************************
    // Initialize the Script Run time
    // ******************************************************************************
    void PlayerController::Init()
    {
        m_EntityPosition   = &m_Entity.GetComponent<TransformComponent>().Translation;
        m_EntitySize       = &m_Entity.GetComponent<TransformComponent>().Scale;
        
        m_CameraRefPos       = &m_ActiveScene->GetPrimaryCameraEntity().GetComponent<TransformComponent>().Translation.x;
        m_EditorCameraRefPos = &m_ActiveScene->GetEditorCameraEntity().GetComponent<TransformComponent>().Translation.x;
    }
    
    // ******************************************************************************
    // Update the player each frame
    // ******************************************************************************
    void PlayerController::OnUpdate(Timestep ts)
    {
        if (Input::IsKeyPressed(KeyCode::Right) && !m_ActiveScene->IsRightCollision(m_Entity, s_RunningSpeed))
        {
            m_EntitySize->x     = 1.0f;
            m_EntityPosition->x += s_RunningSpeed;
            
            *m_CameraRefPos       += s_RunningSpeed;
            *m_EditorCameraRefPos += s_RunningSpeed;
        }
        if (Input::IsKeyPressed(KeyCode::Left) && !m_ActiveScene->IsLeftCollision(m_Entity, s_RunningSpeed))
        {
            m_EntitySize->x     = -1.0f;
            m_EntityPosition->x -= s_RunningSpeed;
            
            *m_CameraRefPos       -= s_RunningSpeed;
            *m_EditorCameraRefPos -= s_RunningSpeed;
        }
    }
    
    // ******************************************************************************
    // Player Event handler
    // ******************************************************************************
    void PlayerController::OnEvent(iKan::Event &event)
    {
        EventDispatcher dispather(event);
        dispather.Dispatch<KeyPressedEvent>(IK_BIND_EVENT_FN(PlayerController::OnkeyPressed));
        dispather.Dispatch<KeyReleasedEvent>(IK_BIND_EVENT_FN(PlayerController::OnKeyReleased));
    }
    
    // ******************************************************************************
    // Player Key pressed event handler
    // ******************************************************************************
    bool PlayerController::OnkeyPressed(KeyPressedEvent& event)
    {
        return false;
    }
    
    // ******************************************************************************
    // Player Key released event handler
    // ******************************************************************************
    bool PlayerController::OnKeyReleased(KeyReleasedEvent& event)
    {
        return false;
    }
    
    // ******************************************************************************
    // Imgui renderer for Imgui
    // ******************************************************************************
    void PlayerController::ImguiRenderer()
    {
        
    }
    
    // ******************************************************************************
    // Collision Callback
    // ******************************************************************************
    void PlayerController::OnCollision(Entity& colloidedEntity)
    {
        
    }

}
