// ******************************************************************************
// File         : EditorLayer.h
// Description  : Client side Application
// Project      : Editor
//
// Created on   : 13/04/21.
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
    virtual void OnEvent() override;
    virtual void OnUpdate() override;
    virtual void OnImguiRender() override;
};
