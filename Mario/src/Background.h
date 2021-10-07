// ******************************************************************************
// File         : Background.h
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
    // Script for Each item
    // ******************************************************************************
    class ItemScript : public ScriptableEntity
    {
    public:
        ~ItemScript() = default;
        ItemScript(Ref<Scene> scene)
        : ScriptableEntity(scene) {}
        
        virtual void OnCreate() override { m_Created = true; }
        
        virtual void OnUpdate(Timestep ts) override {}
        virtual void OnDestroy() override {}
        virtual void ImguiRenderer() override {}
        virtual void OnEvent(Event& event) override {}
        
        virtual void OnCollision(Entity& colloidedEntity) override;
    };

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
        static void ImgButtons(const char name);

    public:
        static glm::vec4 s_BgColor;
    };

}
