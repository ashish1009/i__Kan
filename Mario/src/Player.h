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
    class Player
    {
    public:
        ~Player();
        Player(Ref<Scene> scene);

        void Init(Ref<Scene> scene);
        void OnUpdate(Timestep ts);
        void ImguiRenderer();
        
    private:
        Ref<Scene> m_ActiveScene;
        Entity m_Entity;
        
        bool m_IsSetting = true;
        float m_RunningSpeed = 0.11;
        
        glm::vec3 m_Position;
        glm::vec3 m_Size;
        
        friend class MarioLayer;
    };

}
