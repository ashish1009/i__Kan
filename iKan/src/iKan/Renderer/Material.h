// ******************************************************************************
// File         : Material.h
// Description  : Material class to hold property of Renderer Materials
// Projecte     : iKan : Renderer
//
// Created by Ashish on 06/07/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Core/Buffer.h>

#include <iKan/Renderer/Shader.h>
#include <iKan/Renderer/Texture.h>

namespace iKan {

    class MaterialInstance;

    // ******************************************************************************
    // Implementation of Material class that stores the material property of Object
    // ******************************************************************************
    class Material
    {
        friend class MaterialInstance;

    public:
        Material(const Ref<Shader>& shader);
        virtual ~Material();

    public:
        static Ref<Material> Create(const Ref<Shader>& shader);

    private:
        Ref<Shader> m_Shader;
        std::unordered_set<MaterialInstance*> m_MaterialInstances;
    };

    // ******************************************************************************
    // Instancisation of Material
    // ******************************************************************************
    class MaterialInstance
    {
        friend class Material;
    public:
        MaterialInstance(const Ref<Material>& material, const std::string& name = "");
        virtual ~MaterialInstance();

    public:
        static Ref<MaterialInstance> Create(const Ref<Material>& material);

    private:
        void AllocateStorage();

    private:
        Ref<Material> m_Material;
        std::string   m_Name;

        Buffer m_VSUniformStorageBuffer;
        Buffer m_PSUniformStorageBuffer;

        std::vector<Ref<Texture>> m_Textures;
    };

}

