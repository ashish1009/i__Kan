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
    Viewport& m_Viewport = Viewport::Create("../../../../../../../iKan./iKan/Github/iKan");
};
