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
    class PlayerController : public ScriptableEntity
    {
    public:
        static constexpr uint32_t MAX_STATES         = 7;
        static constexpr uint32_t MAX_RUNNING_IMG    = 3;
        static constexpr uint32_t MAX_JUMPING_HEIGHT = 5;
        
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
        ~PlayerController();
        PlayerController(Ref<Scene>& scene);
        
        virtual void OnCreate() override;
        virtual void OnUpdate(Timestep ts) override;
        virtual void OnDestroy() override {}
        virtual void ImguiRenderer() override;
        virtual void OnEvent(Event& event) override;
        virtual void OnCollision(Entity& colloidedEntity) override;
        
    private:
        
        void ChangeSize(Size size);
        
        bool OnkeyPressed(KeyPressedEvent& event);
        bool OnKeyReleased(KeyReleasedEvent& event);
        
        void ClearState(State state)  { m_State = m_State & (~(1 << (Utils::GetFirstSetBit((uint32_t)state) - 1))); }
        void ToggleState(State state) { m_State ^= (1 << (Utils::GetFirstSetBit((uint32_t)state) - 1)); }
        void SetState(State state)    { m_State = m_State | (1 << (Utils::GetFirstSetBit((uint32_t)state) - 1)); }
        
        bool IsState(State state) { return ((m_State & (uint32_t)state) != 0); }
        
    private:
        static int32_t GetFirstSetBit(uint32_t value);
        
        static void Init(Ref<Scene> scene, bool isShort, Color color);
        static void SetPlayerTextureForAllStates(bool isShort, Color color);
        
        // Functions for player state machine
        static void Falling(PlayerController* player);
        static void Jumping(PlayerController* player);
        static void Standing(PlayerController* player);
        static void Firing(PlayerController* player);
        static void Dying(PlayerController* player);
        static void Sitting(PlayerController* player);
        static void Running(PlayerController* player);
        
    private:
        static Ref<Scene> s_ActiveScene;
        
        // Function pointer to store the functionality of states
        static std::function <void(PlayerController*)> s_StateFunc[MAX_STATES];
        
        // Texture to store tile sprite sheet
        static Ref<Texture>    s_Texture;
        static Ref<SubTexture> s_StandingSubtexComp;
        static Ref<SubTexture> s_RunningSubtexComp[MAX_RUNNING_IMG];
        
        static float s_RunningSpeed;
        static float s_FallingSpeed;
        static float s_JumpingSpeed;
        
        // Non Static members
        glm::vec3* m_EntityPosition;
        glm::vec3* m_EntitySize;
        
        Ref<SubTexture>* m_EntitySubtexture;
        
        Color     m_Color     = Color::Classic;
        Size      m_Size      = Size::Short;
        Direction m_Direction = Direction::Right;
        
        uint32_t m_State = (uint32_t)State::Falling;
        uint32_t m_RunningImgIdx = 0;
        
        float m_StartingJumpingPosing = 0;
        
        float* m_CameraRefPos;
        float* m_EditorCameraRefPos; // TODO: Temp for now Remove after finalize
        
        // Actual Player Info
        int32_t m_Life = 3;
        
        friend class MarioLayer;
    };
    
}
