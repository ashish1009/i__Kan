// ******************************************************************************
// File         : ShaderUniform.h
// Description  : Interface for Sotring Shader uniform in buffer
// Project      : i-Kan : Renderer
//
// Created by Ashish on 06/07/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

namespace iKan {

    // ******************************************************************************
    // ******************************************************************************
    enum class ShaderDomain
    {
        None = 0, Vertex = 0, Pixel = 1
    };

    // ******************************************************************************
    // ******************************************************************************
    class ShaderUniformDeclaration
    {
    private:
        friend class Shader;
        friend class OpenGLShader;
        friend class ShaderStruct;

    public:
        virtual const std::string& GetName() const = 0;
        virtual uint32_t GetSize() const = 0;
        virtual uint32_t GetCount() const = 0;
        virtual uint32_t GetOffset() const = 0;
        virtual ShaderDomain GetDomain() const = 0;

    protected:
        virtual void SetOffset(uint32_t offset) = 0;
    };
    typedef std::vector<ShaderUniformDeclaration*> ShaderUniformList;

    // ******************************************************************************
    // ******************************************************************************
    class ShaderUniformBufferDeclaration
    {
    public:
        virtual const std::string& GetName() const = 0;
        virtual uint32_t GetRegister() const = 0;
        virtual uint32_t GetSize() const = 0;
        virtual const ShaderUniformList& GetUniformDeclarations() const = 0;

        virtual ShaderUniformDeclaration* FindUniform(const std::string& name) = 0;
    };
    typedef std::vector<ShaderUniformBufferDeclaration*> ShaderUniformBufferList;

    // ******************************************************************************
    // ******************************************************************************
    class ShaderStruct
    {
    private:
        friend class Shader;

    public:
        ShaderStruct(const std::string& name)
        : m_Name(name), m_Size(0), m_Offset(0)
        {
        }

        void AddField(ShaderUniformDeclaration* field)
        {
            m_Size += field->GetSize();
            uint32_t offset = 0;
            if (m_Fields.size())
            {
                ShaderUniformDeclaration* previous = m_Fields.back();
                offset = previous->GetOffset() + previous->GetSize();
            }
            field->SetOffset(offset);
            m_Fields.push_back(field);
        }

        void SetOffset(uint32_t offset) { m_Offset = offset; }

        uint32_t GetSize() const { return m_Size; }
        uint32_t GetOffset() const { return m_Offset; }

        const std::string& GetName() const { return m_Name; }
        const std::vector<ShaderUniformDeclaration*>& GetFields() const { return m_Fields; }\

    private:
        std::string m_Name;

        uint32_t m_Size;
        uint32_t m_Offset;

        std::vector<ShaderUniformDeclaration*> m_Fields;
    };
    typedef std::vector<ShaderStruct*> ShaderStructList;

    // ******************************************************************************
    // Stores the type of shader
    // ******************************************************************************
    class ShaderResourceDeclaration
    {
    public:
        virtual const std::string& GetName() const = 0;
        virtual uint32_t GetRegister() const = 0;
        virtual uint32_t GetCount() const = 0;
    };
    typedef std::vector<ShaderResourceDeclaration*> ShaderResourceList;

}
