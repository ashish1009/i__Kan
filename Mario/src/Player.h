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
        
//        // ******************************************************************************
//        // State of Mario Player
//        // ******************************************************************************
//        enum class State { Jumping = 4, Standing = 6, Firing, Dying, Sitting, Running };

        // ******************************************************************************
        // Size of Mario Player
        // ******************************************************************************
        enum class Size { Short, Tall };

    public:
        ~Player();
        Player(Ref<Scene> scene);

        void Init(Ref<Scene> scene);
        void OnUpdate(Timestep ts);
        void ImguiRenderer();
        void ChangeSize(Size size);
        
    private:
        void SetCurrentTexture(Color color);
        
    private:
        Ref<Scene> m_ActiveScene;

        Entity     m_Entity;
        glm::vec3* m_EntityPosition;
        glm::vec3* m_EntitySize;
        
        bool  m_IsSetting = true;
        float m_RunningSpeed = 0.11;
        
        Color m_Color = Color::Classic;
        Size  m_Size  = Size::Short;
        
        // Texture to store tile sprite sheet
        Ref<Texture>    m_Texture;
        Ref<SubTexture> m_StandingSubtexComp;
        
        friend class MarioLayer;
    };

}
