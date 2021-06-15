// ******************************************************************************
// File         : StartScreen.h
// Description  : Storing all entity for Start Sceen rendering
// Project      : Mario
//
//  Created by Ashish on 15/06/21.
//  Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan.h>

using namespace iKan;

namespace Mario {

    // ******************************************************************************
    // Background Wrapper to store Entity to render background
    // ******************************************************************************
    class StartScreen
    {
    public:
        static void CreateEntities(Ref<Scene>& scene);
    };

}
