// ******************************************************************************
// File         : Background.cpp
// Description  : Storing all entity for Background rendering
// Project      : Mario
//
//  Created by Ashish on 07/05/21.
//  Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan.h>

using namespace iKan;

namespace Mario {

    // ******************************************************************************
    // Background Wrapper to store Entity to render background
    // ******************************************************************************
    class Background
    {
    public:
        static void CreateEntities(Ref<Scene>& scene);
        static void ImGuiRenderer();

    private:
        static void Init();

    public:
        static glm::vec4 s_BgColor;
    };

}
