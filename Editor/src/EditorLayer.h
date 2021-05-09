// ******************************************************************************
// File         : EditorLayer.h
// Description  : Client side Application
// Project      : Editor
//
// Created by Ashish on 13/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan.h>

using namespace iKan;

class EditorLayer : public Layer
{
public:
    EditorLayer();
    virtual ~EditorLayer();
    
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnEvent(Event& event) override;
    virtual void OnUpdate(Timestep ts) override;
    virtual void OnImguiRender() override;
    
private:
    void ShowMenu();
    void RendererStats();

private:
    EditorCamera         m_EditorCamera;
    Ref<Scene>           m_ActiveScene;
    SceneHeirarchyPannel m_SceneHierarchyPannel;
    
    Viewport& m_Viewport = Viewport::Get();
    
    struct PropertyFlag
    {
        bool IsFrameRate           = true;
        bool IsRendererStats       = true;
        bool IsVendorType          = true;
        bool IsSceneHeirarchypanel = true;
    };
    static PropertyFlag s_PropFlag;

};
