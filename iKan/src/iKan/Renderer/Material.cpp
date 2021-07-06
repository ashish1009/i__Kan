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
    }

    // ******************************************************************************
    // Material Instance destructor
    // ******************************************************************************
    MaterialInstance::~MaterialInstance()
    {
        m_Material->m_MaterialInstances.erase(this);
    }


}
