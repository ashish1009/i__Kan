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
    virtual void OnImguiRender() override;
    
private:
    void ShowMenu();
    void RendererStats();
    
private:
    Ref<Scene>           m_ActiveScene;
    SceneHeirarchyPannel m_SceneHierarchyPannel;
    Entity               m_CameraEntity;
    
    Viewport& m_Viewport = Viewport::Get();
    
    struct PropertyFlag
    {
        bool IsFrameRate           = true;
        bool IsRendererStats       = true;
        bool IsVendorType          = true;
        bool IsSceneHeirarchypanel = true;
        bool IsSettings            = true;
    };
    static PropertyFlag s_PropFlag;

    Entity ent1, ent2;

};

