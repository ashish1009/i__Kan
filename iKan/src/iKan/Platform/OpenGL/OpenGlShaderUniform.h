// ******************************************************************************
// File         : OpenGLShaderUniform.h
// Description  : Implementation of Open GL Shader
// Project      : iKan : Platform
//
// Created by Ashish on 06/07/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/ShaderUniform.h>

namespace iKan {

    // ******************************************************************************
    // Stores the type of Shader datat type resources, like "sampler2d ... etc"
    // ******************************************************************************
    class OpenGLShaderResourceDeclaration : public ShaderResourceDeclaration
    {
    public:
        enum class Type
        {
            NONE, TEXTURE2D, TEXTURECUBE
        };

    private:
        friend class OpenGLShader;

    public:
        OpenGLShaderResourceDeclaration(Type type, const std::string& name, uint32_t count);

        const std::string& GetName() const override { return m_Name; }
        uint32_t GetRegister() const override { return m_Register; }
        uint32_t GetCount() const override { return m_Count; }

        Type GetType() const { return m_Type; }

    public:
        static Type StringToType(const std::string& type);
        static std::string TypeToString(Type type);

    private:
        std::string m_Name;
        uint32_t m_Register = 0;
        uint32_t m_Count;
        Type m_Type;
    };

    // ******************************************************************************
    // Stores the field, it can be structur or pre defined data type.
    // ******************************************************************************
    class OpenGLShaderUniformDeclaration : public ShaderUniformDeclaration
    {
    private:
        friend class OpenGLShader;
        friend class OpenGLShaderUniformBufferDeclaration;

    public:
        enum class Type
        {
            NONE, FLOAT32, VEC2, VEC3, VEC4, MAT3, MAT4, INT32, STRUCT
        };

    public:
        OpenGLShaderUniformDeclaration(ShaderDomain domain, Type type, const std::string& name, uint32_t count = 1);
        OpenGLShaderUniformDeclaration(ShaderDomain domain, ShaderStruct* uniformStruct, const std::string& name, uint32_t count = 1);

        const std::string& GetName() const override { return m_Name; }

        uint32_t GetSize() const override { return m_Size; }
        uint32_t GetCount() const override { return m_Count; }
        uint32_t GetOffset() const override { return m_Offset; }

        ShaderDomain GetDomain() const override { return m_Domain; }

        uint32_t GetAbsoluteOffset() const { return m_Struct ? m_Struct->GetOffset() + m_Offset : m_Offset; }
        int32_t GetLocation() const { return m_Location; }

        Type GetType() const { return m_Type; }
        bool IsArray() const { return m_Count > 1; }

        const ShaderStruct& GetShaderUniformStruct() const { IK_CORE_ASSERT(m_Struct, ""); return *m_Struct; }

    protected:
        void SetOffset(uint32_t offset) override;

    public:
        static uint32_t SizeOfUniformType(Type type);
        static Type StringToType(const std::string& type);
        static std::string TypeToString(Type type);

    private:
        std::string m_Name;

        uint32_t m_Size   = 0;
        uint32_t m_Count  = 0;
        uint32_t m_Offset = 0;

        Type m_Type;
        ShaderDomain m_Domain;
        ShaderStruct* m_Struct;

        mutable int32_t m_Location;
    };

    // ******************************************************************************
    // Shader Buffer that stores the Uniform declaration for Shader
    // ******************************************************************************
    class OpenGLShaderUniformBufferDeclaration : public ShaderUniformBufferDeclaration
    {
    private:
        friend class Shader;

    private:
        std::string m_Name;

        uint32_t m_Register;
        uint32_t m_Size;

        ShaderUniformList m_Uniforms;
        ShaderDomain      m_Domain;

    public:
        OpenGLShaderUniformBufferDeclaration(const std::string& name, ShaderDomain domain);

        void PushUniform(OpenGLShaderUniformDeclaration* uniform);

        const std::string& GetName() const override { return m_Name; }

        uint32_t GetRegister() const override { return m_Register; }
        uint32_t GetSize() const override { return m_Size; }

        const ShaderUniformList& GetUniformDeclarations() const override { return m_Uniforms; }
        ShaderUniformDeclaration* FindUniform(const std::string& name) override;
        
        ShaderDomain GetDomain() const { return m_Domain; }
    };



}
