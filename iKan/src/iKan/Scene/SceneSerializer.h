// ******************************************************************************
// File         : SceneSerializer.h
// Description  : Saves and stores the scene data
// Project      : iKan : Scene
//
// Created by Ashish on 14/06/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Scene/Scene.h>

namespace iKan {

    // ******************************************************************************
    // cl;ass to save the scene
    // ******************************************************************************
    class SceneSerializer
    {
    public:
        SceneSerializer(const Ref<Scene>& scene);
        ~SceneSerializer();

        void Serialize(const std::string& filepath);
        void SerializeRuntime(const std::string& filepath);

        bool Deserialize(const std::string& filepath);
        bool DeserializeRuntime(const std::string& filepath);
        
    private:
        Ref<Scene> m_Scene;
    };

}
