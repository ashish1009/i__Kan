// ******************************************************************************
// File         : ImguiLayer.h
// Description  : Implementation of Imgui Layer
// Project      : iKan : Core
//
// Created by Ashish on 24/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Core/Layer.h>

namespace iKan {
    
    // ******************************************************************************
    // Imgui Layer that will handle all the imgui functionality. This layer will be
    // embedded in Application
    // ******************************************************************************
    class ImguiLayer : public Layer
    {
    public:
        ImguiLayer()
        : Layer("Imgui Layer") {}
        
        virtual ~ImguiLayer() = default;
        
        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnEvent(Event& event) override;

        void Begin();
        void End();
        
        void BlockEvents(bool blockEvents) { m_BlockEvents = blockEvents; }
        
    private:
        bool m_BlockEvents = true;
    };

}
