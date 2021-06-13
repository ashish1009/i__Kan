// ******************************************************************************
// File         : Player.h
// Descrition   : Player Class info
// Project      : Mario
//
// Created by Ashish on 10/06/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan.h>

using namespace iKan;

namespace Mario {

    class Player
    {
    public:
        ~Player();

        // Delete Copy and move Constructor and = operator for singleton
        Player(const Player&) = delete;
        Player(Player&&) = delete;

        Player& operator=(const Player&) = delete;
        Player& operator=(Player&&) = delete;

        // ******************************************************************************
        // Get the player instance
        // ******************************************************************************
        static Player& Get()
        {
            IK_ASSERT(s_Instance, "Instance is not created yet");
            return *s_Instance;
        }

        // ******************************************************************************
        // Create player instance
        // ******************************************************************************
        static Player& Create()
        {
            if (!s_Instance)
            {
                IK_INFO("Creating Mario player instance");
                s_Instance = new Player;
            }
            return *s_Instance;
        }

        // ******************************************************************************
        // Destroy player instacne
        // ******************************************************************************
        static void Destroy()
        {
            if (s_Instance)
            {
                delete s_Instance;
            }
        }

    private:
        Player();

    private:
        static Player* s_Instance;

        Entity m_Entity;
    };

}
