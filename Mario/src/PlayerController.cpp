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
    
    // ******************************************************************************
    // Player Constructor
    // ******************************************************************************
    PlayerController::PlayerController(Ref<Scene> scene)
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

    }
    
    // ******************************************************************************
    // Update the player each frame
    // ******************************************************************************
    void PlayerController::OnUpdate(Timestep ts)
    {

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
