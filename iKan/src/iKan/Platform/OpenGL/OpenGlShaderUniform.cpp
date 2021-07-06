// ******************************************************************************
// File         : OpenGLShaderUniform.h
// Description  : Implementation of Open GL Shader
// Project      : iKan : Platform
//
// Created by Ashish on 06/07/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "OpenGlShaderUniform.h"

namespace iKan {

    // ******************************************************************************
    // OpenGLShaderResourceDeclaration constructor
    // ******************************************************************************
    OpenGLShaderResourceDeclaration::OpenGLShaderResourceDeclaration(Type type, const std::string& name, uint32_t count)
    : m_Type(type), m_Name(name), m_Count(count)
    {
        IK_CORE_INFO("OpenGLShaderResourceDeclaration, constructor called Type: {0}, Name: {1}, Count: {2}", m_Type, m_Name, m_Count);
    }

    // ******************************************************************************
    // Convert string to OpenGLShaderResourceDeclaration type
    // ******************************************************************************
    OpenGLShaderResourceDeclaration::Type OpenGLShaderResourceDeclaration::StringToType(const std::string& type)
    {
        if (type == "sampler2D")    return Type::TEXTURE2D;
        if (type == "sampler2DMS")  return Type::TEXTURE2D;
        if (type == "samplerCube")  return Type::TEXTURECUBE;

        return Type::NONE;
    }

    // ******************************************************************************
    // convert OpenGLShaderResourceDeclaration type to string
    // ******************************************************************************
    std::string OpenGLShaderResourceDeclaration::TypeToString(Type type)
    {
        switch (type)
        {
            case Type::NONE:        return "None";
            case Type::TEXTURE2D:   return "sampler2D";
            case Type::TEXTURECUBE: return "samplerCube";
        }
        return "Invalid Type";
    }

    // ******************************************************************************
    // OpenGLShaderUniformDeclaration Constructor
    // ******************************************************************************
    OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(ShaderDomain domain, Type type, const std::string& name, uint32_t count)
    : m_Type(type), m_Struct(nullptr), m_Domain(domain), m_Name(name), m_Count(count)
    {
        m_Size = SizeOfUniformType(m_Type) * m_Count;
        IK_CORE_INFO("OpenGLShaderUniformDeclaration, constructor called Type: {0}, Domain: {1}, Name: {2}, Count: {3}, Size: {4}", m_Type, m_Domain, m_Name, m_Count, m_Size);
    }

    // ******************************************************************************
    // OpenGLShaderUniformDeclaration Constructor
    // ******************************************************************************
    OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(ShaderDomain domain, ShaderStruct* uniformStruct, const std::string& name, uint32_t count)
    : m_Struct(uniformStruct), m_Type(OpenGLShaderUniformDeclaration::Type::STRUCT), m_Domain(domain), m_Name(name), m_Count(count)
    {
        m_Size = m_Struct->GetSize() * count;
    }

    // ******************************************************************************
    // Set the Ofset of the buffer
    // ******************************************************************************
    void OpenGLShaderUniformDeclaration::SetOffset(uint32_t offset)
    {
        if (m_Type == OpenGLShaderUniformDeclaration::Type::STRUCT)
            m_Struct->SetOffset(offset);

        m_Offset = offset;
    }

    // ******************************************************************************
    // Get size of type
    // ******************************************************************************
    uint32_t OpenGLShaderUniformDeclaration::SizeOfUniformType(Type type)
    {
        switch (type)
        {
            case OpenGLShaderUniformDeclaration::Type::NONE:       return 0;
            case OpenGLShaderUniformDeclaration::Type::INT32:      return 4;
            case OpenGLShaderUniformDeclaration::Type::FLOAT32:    return 4;
            case OpenGLShaderUniformDeclaration::Type::VEC2:       return 4 * 2;
            case OpenGLShaderUniformDeclaration::Type::VEC3:       return 4 * 3;
            case OpenGLShaderUniformDeclaration::Type::VEC4:       return 4 * 4;
            case OpenGLShaderUniformDeclaration::Type::MAT3:       return 4 * 3 * 3;
            case OpenGLShaderUniformDeclaration::Type::MAT4:       return 4 * 4 * 4;
            case OpenGLShaderUniformDeclaration::Type::STRUCT:     IK_CORE_ASSERT(false, "No need to return the size in case of struct")
        }
        return 0;
    }

    // ******************************************************************************
    // Convert String to type
    // ******************************************************************************
    OpenGLShaderUniformDeclaration::Type OpenGLShaderUniformDeclaration::StringToType(const std::string& type)
    {
        if (type == "int")      return Type::INT32;
        if (type == "float")    return Type::FLOAT32;
        if (type == "vec2")     return Type::VEC2;
        if (type == "vec3")     return Type::VEC3;
        if (type == "vec4")     return Type::VEC4;
        if (type == "mat3")     return Type::MAT3;
        if (type == "mat4")     return Type::MAT4;

        return Type::NONE;
    }

    // ******************************************************************************
    // COnvert Type to string
    // ******************************************************************************
    std::string OpenGLShaderUniformDeclaration::TypeToString(Type type)
    {
        switch (type)
        {
            case OpenGLShaderUniformDeclaration::Type::NONE:       return "None";
            case OpenGLShaderUniformDeclaration::Type::INT32:      return "int32";
            case OpenGLShaderUniformDeclaration::Type::FLOAT32:    return "float";
            case OpenGLShaderUniformDeclaration::Type::VEC2:       return "vec2";
            case OpenGLShaderUniformDeclaration::Type::VEC3:       return "vec3";
            case OpenGLShaderUniformDeclaration::Type::VEC4:       return "vec4";
            case OpenGLShaderUniformDeclaration::Type::MAT3:       return "mat3";
            case OpenGLShaderUniformDeclaration::Type::MAT4:       return "mat4";
            case OpenGLShaderUniformDeclaration::Type::STRUCT:     return "struct";
        }
        return "Invalid Type";
    }

    // ******************************************************************************
    // Push the uniform in the buffer
    // ******************************************************************************
    void OpenGLShaderUniformBufferDeclaration::PushUniform(OpenGLShaderUniformDeclaration* uniform)
    {
        uint32_t offset = 0;
        if (m_Uniforms.size())
        {
            OpenGLShaderUniformDeclaration* previous = (OpenGLShaderUniformDeclaration*)m_Uniforms.back();
            offset = previous->m_Offset + previous->m_Size;
        }
        uniform->SetOffset(offset);
        m_Size += uniform->GetSize();
        m_Uniforms.push_back(uniform);
    }

    // ******************************************************************************
    // Find the specific uniform from buffer
    // ******************************************************************************
    ShaderUniformDeclaration* OpenGLShaderUniformBufferDeclaration::FindUniform(const std::string& name)
    {
        for (ShaderUniformDeclaration* uniform : m_Uniforms)
        {
            if (uniform->GetName() == name)
                return uniform;
        }
        return nullptr;
    }

}
