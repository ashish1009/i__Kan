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

    Player* Player::s_Instance = nullptr;

    // ******************************************************************************
    // Player Constructor
    // ******************************************************************************
    Player::Player()
    {
        IK_ASSERT(!s_Instance, "Duplicate player instance ceation attempt");
        
        IK_INFO("Mario Player Constructor called");
    }

    // ******************************************************************************
    // Player Destructor
    // ******************************************************************************
    Player::~Player()
    {
        IK_WARN("Mario Player Destructor called");
    }

}
