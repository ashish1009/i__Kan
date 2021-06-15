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

    class Player
    {
    public:
        ~Player();
        Player(Ref<Scene> scene);

        void Init(Ref<Scene> scene);

    private:
        Entity m_Entity;
    };

}
