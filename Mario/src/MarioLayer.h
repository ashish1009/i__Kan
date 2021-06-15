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
#include "Player.h"

using namespace iKan;

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
    Entity               m_CameraEntity;

//    Mario::Player&       m_Player = Mario::Player::Create();
    
    Viewport& m_Viewport = Viewport::Get();

};

