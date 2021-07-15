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
    // Flg used for materials
    // ******************************************************************************
    enum class MaterialFlag
    {
        None       = BIT(0),
        DepthTest  = BIT(1),
        Blend      = BIT(2)
    };

    // ******************************************************************************
    // Implementation of Material class that stores the material property of Object
    // ******************************************************************************
    class Material
    {
        friend class MaterialInstance;

    public:
        Material(const Ref<Shader>& shader);
        virtual ~Material();

        void Bind();

        uint32_t GetFlags() const { return m_MaterialFlags; }
        void SetFlag(MaterialFlag flag) { m_MaterialFlags |= (uint32_t)flag; }

        void Set(const std::string& name, const Ref<Texture>& texture);
        void Set(const std::string& name, const Ref<TextureCube>& texture);

        template <typename T>
        void Set(const std::string& name, const T& value);

        template<typename T>
        T& Get(const std::string& name)
        {
            auto decl = FindUniformDeclaration(name);
            IK_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
            auto& buffer = GetUniformBufferTarget(decl);
            return buffer.Read<T>(decl->GetOffset());
        }

        template<typename T>
        Ref<T> GetResource(const std::string& name)
        {
            auto decl = FindResourceDeclaration(name);
            uint32_t slot = decl->GetRegister();
            IK_CORE_ASSERT((slot < m_Textures.size()), "Texture slot is invalid!");
            return m_Textures[slot];
        }

    public:
        static Ref<Material> Create(const Ref<Shader>& shader);

    private:
        void AllocateStorage();
        void OnShaderReloaded();
        void BindTextures();

        ShaderUniformDeclaration* FindUniformDeclaration(const std::string& name);
        ShaderResourceDeclaration* FindResourceDeclaration(const std::string& name);
        Buffer& GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration);

    private:
        Ref<Shader> m_Shader;
        std::unordered_set<MaterialInstance*> m_MaterialInstances;

        Buffer m_VSUniformStorageBuffer;
        Buffer m_PSUniformStorageBuffer;
        std::vector<Ref<Texture>> m_Textures;

        uint32_t m_MaterialFlags;
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

        void Set(const std::string& name, const Ref<Texture>& texture);
        void Set(const std::string& name, const Ref<TextureCube>& texture);

        template <typename T>
        void Set(const std::string& name, const T& value)
        {
            auto decl = m_Material->FindUniformDeclaration(name);
            if (!decl)
                return;

            IK_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
            auto& buffer = GetUniformBufferTarget(decl);
            buffer.Write((std::byte*)& value, decl->GetSize(), decl->GetOffset());

            m_OverriddenValues.insert(name);
        }

        template<typename T>
        T& Get(const std::string& name)
        {
            auto decl = m_Material->FindUniformDeclaration(name);
            IK_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
            auto& buffer = GetUniformBufferTarget(decl);
            return buffer.Read<T>(decl->GetOffset());
        }

        template<typename T>
        Ref<T> GetResource(const std::string& name)
        {
            auto decl = m_Material->FindResourceDeclaration(name);
            IK_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
            uint32_t slot = decl->GetRegister();
            IK_CORE_ASSERT((slot < m_Textures.size()), "Texture slot is invalid!");
            return Ref<T>(m_Textures[slot]);
        }

        template<typename T>
        Ref<T> TryGetResource(const std::string& name)
        {
            auto decl = m_Material->FindResourceDeclaration(name);
            if (!decl)
                return nullptr;

            uint32_t slot = decl->GetRegister();
            if (slot >= m_Textures.size())
                return nullptr;

            return Ref<T>(m_Textures[slot]);
        }

        void Bind();
        void SetFlag(MaterialFlag flag, bool value = true);
        bool GetFlag(MaterialFlag flag) const { return (uint32_t)flag & m_Material->m_MaterialFlags; }

        uint32_t GetFlags() const { return m_Material->m_MaterialFlags; }
        Ref<Shader> GetShader() { return m_Material->m_Shader; }

        const std::string& GetName() const { return m_Name; }

    public:
        static Ref<MaterialInstance> Create(const Ref<Material>& material);

    private:
        void AllocateStorage();
        void OnShaderReloaded();
        Buffer& GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration);
        void OnMaterialValueUpdated(ShaderUniformDeclaration* decl);

    private:
        Ref<Material> m_Material;
        std::string   m_Name;

        Buffer m_VSUniformStorageBuffer;
        Buffer m_PSUniformStorageBuffer;

        std::vector<Ref<Texture>> m_Textures;

        // TODO: This is temporary; come up with a proper system to track overrides
        std::unordered_set<std::string> m_OverriddenValues;
    };

    // ******************************************************************************
    // in order to avoide forward declaration defining this function here......
    // ******************************************************************************
    template <typename T>
    void Material::Set(const std::string& name, const T& value)
    {
        auto decl = FindUniformDeclaration(name);
        IK_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
        auto& buffer = GetUniformBufferTarget(decl);
        buffer.Write((std::byte*)&value, decl->GetSize(), decl->GetOffset());

        for (auto mi : m_MaterialInstances)
            mi->OnMaterialValueUpdated(decl);
    }

}

