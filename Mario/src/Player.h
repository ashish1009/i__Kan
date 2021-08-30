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
        static constexpr uint32_t MAX_STATES = 7;
        static constexpr uint32_t MAX_RUNNING_IMG = 3;
        
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
        enum class State
        {
            Falling   = BIT(0),
            Jumping   = BIT(1),
            Standing  = BIT(2),
            Firing    = BIT(3),
            Dying     = BIT(4),
            Sitting   = BIT(5),
            Running   = BIT(6),
            None      = BIT(MAX_STATES)
        };

        // ******************************************************************************
        // Size of Mario Player
        // ******************************************************************************
        enum class Size { Short, Tall };

        // ******************************************************************************
        // Direction of Running
        // ******************************************************************************
        enum class Direction { Left, Right };

    public:
        ~Player();
        Player(Ref<Scene> scene);

        void OnUpdate(Timestep ts);
        void ImguiRenderer();
        void ChangeSize(Size size);
        
        void OnEvent(Event& event);

    private:
        bool OnkeyPressed(KeyPressedEvent& event);
        bool OnKeyReleased(KeyReleasedEvent& event);
        
        void ClearState(State state)  { m_State = m_State & (~(1 << (Utils::GetFirstSetBit((uint32_t)state) - 1))); }
        void ToggleState(State state) { m_State ^= (1 << (Utils::GetFirstSetBit((uint32_t)state) - 1)); }
        
        void SetState(State state)
        {
            uint32_t val = (1 << (Utils::GetFirstSetBit((uint32_t)state) - 1));
            m_State = m_State | val;
        }

    private:
        static int32_t GetFirstSetBit(uint32_t value);
        
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
        static std::function <void(Player*)> s_StateFunc[MAX_STATES];
        
        // Texture to store tile sprite sheet
        static Ref<Texture>    s_Texture;
        static Ref<SubTexture> s_StandingSubtexComp;
        static Ref<SubTexture> s_RunningSubtexComp[MAX_RUNNING_IMG];
        
        static float s_RunningSpeed;
        static float s_FallingSpeed;
        
        // Non Static members
        Entity     m_Entity;
        glm::vec3* m_EntityPosition;
        glm::vec3* m_EntitySize;
        
        Ref<SubTexture>* m_EntitySubtexture;

        Color     m_Color     = Color::Classic;
        Size      m_Size      = Size::Short;
        Direction m_Direction = Direction::Right;
        
        uint32_t m_State = (uint32_t)State::Falling;
        uint32_t m_RunningImgIdx = 0;
                
        friend class MarioLayer;
    };

}
