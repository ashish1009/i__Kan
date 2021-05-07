// ******************************************************************************
// File         : Background.cpp
// Description  : Storing all entity for Background rendering
// Project      : Mario
//
//  Created by Ashish on 07/05/21.
//  Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "Background.h"

namespace Mario {

    glm::vec4 Background::s_BgColor = { 0.3f, 0.1f, 0.6f, 1.0f };

    // ******************************************************************************
    // Initialize the background tiles and creat entities
    // ******************************************************************************
    void Background::CreateEntities(Ref<Scene>& scene)
    {
    }

    // ******************************************************************************
    // Imgui update each frame for background
    // ******************************************************************************
    void Background::ImGuiRenderer()
    {
        if (ImGui::TreeNode("Background Color"))
        {
            ImGuiAPI::ColorEdit(s_BgColor);
            ImGui::TreePop();
        }
    }

}
