// ******************************************************************************
// File         : MarioLayer.cpp
// Description  : Client (Mario) Main Layer to be added in iKan Application
// Project      : Mario
//
// Created by Ashish on 05/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan.h>

using namespace iKan;

namespace Mario {
    
    // ******************************************************************************
    // Client layer for Mario (Main layer) derived from iKan Layer
    // ******************************************************************************
    class MarioLayer : public Layer
    {
    public:
        MarioLayer();
        virtual ~MarioLayer();
        
        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnEvent(Event& event) override;
        virtual void OnUpdate(Timestep ts) override;
        virtual void OnImguiRender(Timestep ts) override;
        
    private:
        void ShowMenu();
        
    private:
        bool m_IsSetting = true;

        Viewport& m_Viewport = Viewport::Create("../../../../../../../iKan.");
        Ref<Scene> m_ActiveScene;
    };

}
