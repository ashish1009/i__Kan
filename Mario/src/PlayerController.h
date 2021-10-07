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
    class PlayerController : public ScriptableEntity
    {
    public:
        ~PlayerController();
        PlayerController(Ref<Scene> scene);
        
        virtual void OnCreate() override;
        virtual void OnUpdate(Timestep ts) override;
        virtual void OnDestroy() override {}
        virtual void ImguiRenderer() override;
        virtual void OnEvent(Event& event) override;
        virtual void OnCollision(Entity& colloidedEntity) override;
        
    private:
        bool OnkeyPressed(KeyPressedEvent& event);
        bool OnKeyReleased(KeyReleasedEvent& event);
        friend class MarioLayer;
    };

}
