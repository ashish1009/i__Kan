// ******************************************************************************
// File         : Player.cpp
// Descrition   : Player Class info
// Project      : Mario
//
// Created by Ashish on 10/06/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "Player.h"

namespace Mario {

    // ******************************************************************************
    // Player Constructor
    // ******************************************************************************
    Player::Player(Ref<Scene> scene)
    {
        IK_INFO("Mario Player Constructor called");

        Init(scene);
    }

    // ******************************************************************************
    // Player Destructor
    // ******************************************************************************
    Player::~Player()
    {
        IK_WARN("Mario Player Destructor called");
    }

    // ******************************************************************************
    // Initialize the player
    // ******************************************************************************
    void Player::Init(Ref<Scene> scene)
    {
        m_Entity = scene->CreateEntity("Player 1");
        m_Entity.GetComponent<BoxCollider2DComponent>().IsRigid = true;
        m_Entity.AddComponent<SpriteRendererComponent>();
    }

}
