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

    class MarioLayer;

    // ******************************************************************************
    // Mario Player Class
    // ******************************************************************************
    class Player
    {
    public:
        // ******************************************************************************
        // Mario Player Image Color
        // ******************************************************************************
        enum class Color
        {
            Black_Grey  = 0,
            Skin_Grey   = 3,
            Grey_Grey   = 6,
            Blue_Blue   = 9,
            Brown_Blue  = 12,
            Orange_Red  = 15,
            green_Amber = 18,
            Black_Brown = 21,
            Green_White = 24,
            Fire        = 27,
            Classic     = 30
        };
        
        // ******************************************************************************
        // State of Mario Player
        // Note, last entry should be None as it is the range of Array of state machine
        // ******************************************************************************
        enum class State { Falling = 0, Jumping = 1, Standing = 2, Firing = 3, Dying = 4, Sitting = 5, Running = 6, None = 7 };

        // ******************************************************************************
        // Size of Mario Player
        // ******************************************************************************
        enum class Size { Short, Tall };

    public:
        ~Player();
        Player(Ref<Scene> scene);

        void OnUpdate(Timestep ts);
        void ImguiRenderer();
        void ChangeSize(Size size);
        
    private:
        void SetCurrentTexture(Color color);
        
        static void Init(Ref<Scene> scene, bool isShort, Color color);
        static void SetPlayerTextureForAllStates(bool isShort, Color color);
        
        // Functions for player state machine
        static void Falling(Player* player);
        static void Jumping(Player* player);
        static void Standing(Player* player);
        static void Firing(Player* player);
        static void Dying(Player* player);
        static void Sitting(Player* player);
        static void Running(Player* player);
        
    private:
        static Ref<Scene> s_ActiveScene;

        // Function pointer to store the functionality of states
        static std::function <void(Player*)> s_StateFunc[(int32_t)State::None];
        
        // Texture to store tile sprite sheet
        static Ref<Texture>    s_Texture;
        static Ref<SubTexture> s_StandingSubtexComp;
        
        static float s_RunningSpeed;
        static float s_FallingSpeed;
        
        // Non Static members
        Entity     m_Entity;
        glm::vec3* m_EntityPosition;
        glm::vec3* m_EntitySize;
        
        Color m_Color = Color::Classic;
        Size  m_Size  = Size::Short;
        State m_State = State::Falling;
                
        friend class MarioLayer;
    };

}
