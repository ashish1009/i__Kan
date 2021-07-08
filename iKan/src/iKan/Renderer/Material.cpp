// ******************************************************************************
// File         : Material.cpp
// Description  : Material class to hold property of Renderer Materials
//              : implementation
// Projecte     : iKan : Renderer
//
// Created by Ashish on 06/07/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "Material.h"

namespace iKan {

    // ******************************************************************************
    // Create the instance of Material
    // ******************************************************************************
    Ref<Material> Material::Create(const Ref<Shader>& shader)
    {
        return CreateRef<Material>(shader);
    }

    // ******************************************************************************
    // Mateiral Constructor
    // ******************************************************************************
    Material::Material(const Ref<Shader>& shader)
    : m_Shader(shader)
    {

    }

    // ******************************************************************************
    // Material destructor
    // ******************************************************************************
    Material::~Material()
    {

    }

    // ******************************************************************************
    // ******************************************************************************
    void Material::AllocateStorage()
    {
        if (m_Shader->HasVSMaterialUniformBuffer())
        {
            const auto& vsBuffer = m_Shader->GetVSMaterialUniformBuffer();
            m_VSUniformStorageBuffer.Allocate(vsBuffer.GetSize());
            m_VSUniformStorageBuffer.ZeroInitialize();
        }

        if (m_Shader->HasPSMaterialUniformBuffer())
        {
            const auto& psBuffer = m_Shader->GetPSMaterialUniformBuffer();
            m_PSUniformStorageBuffer.Allocate(psBuffer.GetSize());
            m_PSUniformStorageBuffer.ZeroInitialize();
        }
    }

    // ******************************************************************************
    // ******************************************************************************
    void Material::OnShaderReloaded()
    {
        AllocateStorage();

        for (auto mi : m_MaterialInstances)
            mi->OnShaderReloaded();
    }

    // ******************************************************************************
    // ******************************************************************************
    ShaderUniformDeclaration* Material::FindUniformDeclaration(const std::string& name)
    {
        if (m_VSUniformStorageBuffer)
        {
            auto& declarations = m_Shader->GetVSMaterialUniformBuffer().GetUniformDeclarations();
            for (ShaderUniformDeclaration* uniform : declarations)
            {
                if (uniform->GetName() == name)
                    return uniform;
            }
        }

        if (m_PSUniformStorageBuffer)
        {
            auto& declarations = m_Shader->GetPSMaterialUniformBuffer().GetUniformDeclarations();
            for (ShaderUniformDeclaration* uniform : declarations)
            {
                if (uniform->GetName() == name)
                    return uniform;
            }
        }
        return nullptr;
    }

    // ******************************************************************************
    // ******************************************************************************
    ShaderResourceDeclaration* Material::FindResourceDeclaration(const std::string& name)
    {
        auto& resources = m_Shader->GetResources();
        for (ShaderResourceDeclaration* resource : resources)
        {
            if (resource->GetName() == name)
                return resource;
        }
        return nullptr;
    }

    // ******************************************************************************
    // ******************************************************************************
    Buffer& Material::GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration)
    {
        switch (uniformDeclaration->GetDomain())
        {
            case ShaderDomain::Vertex:    return m_VSUniformStorageBuffer;
            case ShaderDomain::Pixel:     return m_PSUniformStorageBuffer;
        }

        IK_CORE_ASSERT(false, "Invalid uniform declaration domain! Material does not support this shader type.");
        return m_VSUniformStorageBuffer;
    }

    // ******************************************************************************
    // ******************************************************************************
    void Material::Bind()
    {
        m_Shader->Bind();

        if (m_VSUniformStorageBuffer)
            m_Shader->SetVSMaterialUniformBuffer(m_VSUniformStorageBuffer);

        if (m_PSUniformStorageBuffer)
            m_Shader->SetPSMaterialUniformBuffer(m_PSUniformStorageBuffer);

        BindTextures();
    }

    // ******************************************************************************
    // ******************************************************************************
    void Material::BindTextures()
    {
        for (size_t i = 0; i < m_Textures.size(); i++)
        {
            auto& texture = m_Textures[i];
            if (texture)
                texture->Bind((uint32_t)i);
        }
    }

    // ******************************************************************************
    // Create the instance of Material instance
    // ******************************************************************************
    Ref<MaterialInstance> MaterialInstance::Create(const Ref<Material>& material)
    {
        return CreateRef<MaterialInstance>(material);
    }

    // ******************************************************************************
    // Material instance constructor
    // ******************************************************************************
    MaterialInstance::MaterialInstance(const Ref<Material>& material, const std::string& name)
    : m_Material(material), m_Name(name)
    {
        IK_CORE_INFO("Constructed Material instnace.");
        m_Material->m_MaterialInstances.insert(this);
        AllocateStorage();
        m_OverriddenValues.clear();
    }

    // ******************************************************************************
    // Material Instance destructor
    // ******************************************************************************
    MaterialInstance::~MaterialInstance()
    {
        m_Material->m_MaterialInstances.erase(this);
    }

    // ******************************************************************************
    // ******************************************************************************
    void MaterialInstance::OnShaderReloaded()
    {
        AllocateStorage();
        m_OverriddenValues.clear();
    }

    // ******************************************************************************
    // Allocate the storage buffers
    // ******************************************************************************
    void MaterialInstance::AllocateStorage()
    {
        if (m_Material->m_Shader->HasVSMaterialUniformBuffer())
        {
            const auto& vsBuffer = m_Material->m_Shader->GetVSMaterialUniformBuffer();
            m_VSUniformStorageBuffer.Allocate(vsBuffer.GetSize());
            memcpy(m_VSUniformStorageBuffer.Data, m_Material->m_VSUniformStorageBuffer.Data, vsBuffer.GetSize());
        }

        if (m_Material->m_Shader->HasPSMaterialUniformBuffer())
        {
            const auto& psBuffer = m_Material->m_Shader->GetPSMaterialUniformBuffer();
            m_PSUniformStorageBuffer.Allocate(psBuffer.GetSize());
            memcpy(m_PSUniformStorageBuffer.Data, m_Material->m_PSUniformStorageBuffer.Data, psBuffer.GetSize());
        }
    }

    // ******************************************************************************
    // ******************************************************************************
    void MaterialInstance::SetFlag(MaterialFlag flag, bool value)
    {
        if (value)
        {
            m_Material->m_MaterialFlags |= (uint32_t)flag;
        }
        else
        {
            m_Material->m_MaterialFlags &= ~(uint32_t)flag;
        }
    }

    // ******************************************************************************
    // ******************************************************************************
    void MaterialInstance::OnMaterialValueUpdated(ShaderUniformDeclaration* decl)
    {
        if (m_OverriddenValues.find(decl->GetName()) == m_OverriddenValues.end())
        {
            auto& buffer = GetUniformBufferTarget(decl);
            auto& materialBuffer = m_Material->GetUniformBufferTarget(decl);
            buffer.Write(materialBuffer.Data + decl->GetOffset(), decl->GetSize(), decl->GetOffset());
        }
    }

    // ******************************************************************************
    // ******************************************************************************
    Buffer& MaterialInstance::GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration)
    {
        switch (uniformDeclaration->GetDomain())
        {
            case ShaderDomain::Vertex:    return m_VSUniformStorageBuffer;
            case ShaderDomain::Pixel:     return m_PSUniformStorageBuffer;
        }

        IK_CORE_ASSERT(false, "Invalid uniform declaration domain! Material does not support this shader type.");
        return m_VSUniformStorageBuffer;
    }

    // ******************************************************************************
    // ******************************************************************************
    void MaterialInstance::Bind()
    {
        m_Material->m_Shader->Bind();

        if (m_VSUniformStorageBuffer)
            m_Material->m_Shader->SetVSMaterialUniformBuffer(m_VSUniformStorageBuffer);

        if (m_PSUniformStorageBuffer)
            m_Material->m_Shader->SetPSMaterialUniformBuffer(m_PSUniformStorageBuffer);

        m_Material->BindTextures();
        for (size_t i = 0; i < m_Textures.size(); i++)
        {
            auto& texture = m_Textures[i];
            if (texture)
                texture->Bind((uint32_t)i);
        }
    }

}
