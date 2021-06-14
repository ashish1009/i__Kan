// ******************************************************************************
// File         : IKanEditor.h
// Descrription : Client Side Layer
// Project      : Editor
//
// Created by Ashish on 14/06/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan.h>

using namespace iKan;

class IKanEditor: public Layer
{
public:
    IKanEditor();
    virtual ~IKanEditor();

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnEvent(Event& event) override;
    virtual void OnUpdate(Timestep ts) override;
    virtual void OnImguiRender(Timestep ts) override;

private:
    bool OnKeyPressed(KeyPressedEvent& e);
    bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

    void NewScene();
    void OpenScene();
    void SaveScene();

    void ShowMenu();

private:
    Ref<Scene>           m_ActiveScene;
};

