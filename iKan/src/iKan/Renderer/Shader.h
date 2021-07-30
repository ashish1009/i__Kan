// ******************************************************************************
// File         : Shader.h
// Description  : Interface for Shader class
// Project      : i-Kan : Renderer
//
// Created by Ashish on 27/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/ShaderUniform.h>
#include <iKan/Core/Buffer.h>

namespace iKan {

    // ******************************************************************************
    // ******************************************************************************
    enum class UniformType
    {
        None = 0,
        Float, Float2, Float3, Float4,
        Matrix3x3, Matrix4x4,
        Int32, Uint32
    };

    // ******************************************************************************
    // ******************************************************************************
    struct UniformDecl
    {
        UniformType Type;
        std::ptrdiff_t Offset;
        std::string Name;
    };

    // ******************************************************************************
    // ******************************************************************************
    struct UniformBuffer
    {
        // TODO: This currently represents a byte buffer that has been
        // packed with uniforms. This was primarily created for OpenGL,
        // and needs to be revisted for other rendering APIs. Furthermore,
        // this currently does not assume any alignment. This also has
        // nothing to do with GL uniform buffers, this is simply a CPU-side
        // buffer abstraction.
        std::byte* Buffer;
        std::vector<UniformDecl> Uniforms;
    };

    // ******************************************************************************
    // ******************************************************************************
    struct UniformBufferBase
    {
        virtual const std::byte* GetBuffer() const = 0;
        virtual const UniformDecl* GetUniforms() const = 0;
        virtual unsigned int GetUniformCount() const = 0;
    };

    // ******************************************************************************
    // ******************************************************************************
    template<uint32_t N, uint32_t U>
    struct UniformBufferDeclaration : public UniformBufferBase
    {
        std::byte Buffer[N];
        UniformDecl Uniforms[U];
        std::ptrdiff_t Cursor = 0;
        int Index = 0;

        virtual const std::byte* GetBuffer() const override { return Buffer; }
        virtual const UniformDecl* GetUniforms() const override { return Uniforms; }
        virtual unsigned int GetUniformCount() const { return U; }

        template<typename T>
        void Push(const std::string& name, const T& data) {}

        template<>
        void Push(const std::string& name, const float& data)
        {
            Uniforms[Index++] = { UniformType::Float, Cursor, name };
            memcpy(Buffer + Cursor, &data, sizeof(float));
            Cursor += sizeof(float);
        }

        template<>
        void Push(const std::string& name, const glm::vec3& data)
        {
            Uniforms[Index++] = { UniformType::Float3, Cursor, name };
            memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(glm::vec3));
            Cursor += sizeof(glm::vec3);
        }

        template<>
        void Push(const std::string& name, const glm::vec4& data)
        {
            Uniforms[Index++] = { UniformType::Float4, Cursor, name };
            memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(glm::vec4));
            Cursor += sizeof(glm::vec4);
        }

        template<>
        void Push(const std::string& name, const glm::mat4& data)
        {
            Uniforms[Index++] = { UniformType::Matrix4x4, Cursor, name };
            memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(glm::mat4));
            Cursor += sizeof(glm::mat4);
        }

    };
    
    // ******************************************************************************
    // Interface for Graphics Shader
    // ******************************************************************************
    class Shader
    {
    public:
        using ShaderReloadedCallback = std::function<void()>;

        virtual ~Shader() = default;
        
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        
        virtual const std::string& GetName() = 0;

        virtual void UploadUniformBuffer(const UniformBufferBase& uniformBuffer) = 0;

        virtual void SetVSMaterialUniformBuffer(Buffer buffer) = 0;
        virtual void SetPSMaterialUniformBuffer(Buffer buffer) = 0;

        virtual const ShaderUniformBufferList& GetVSRendererUniforms() const = 0;
        virtual const ShaderUniformBufferList& GetPSRendererUniforms() const = 0;
        virtual bool HasVSMaterialUniformBuffer() const = 0;
        virtual bool HasPSMaterialUniformBuffer() const = 0;
        virtual const ShaderUniformBufferDeclaration& GetVSMaterialUniformBuffer() const = 0;
        virtual const ShaderUniformBufferDeclaration& GetPSMaterialUniformBuffer() const = 0;

        virtual const ShaderResourceList& GetResources() const = 0;

        // Temporary, before we have an asset manager
//        static std::vector<Ref<Shader>> s_AllShaders;
        
        virtual void SetUniformInt1(const std::string& name, int value) = 0;
        virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
        
        virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) = 0;
        virtual void SetUniformMat3(const std::string& name, const glm::mat3& value) = 0;
        
        virtual void SetUniformFloat1(const std::string& name, float value) = 0;
        virtual void SetUniformFloat2(const std::string& name, const glm::vec2& value) = 0;
        virtual void SetUniformFloat3(const std::string& name, const glm::vec3& value) = 0;
        virtual void SetUniformFloat4(const std::string& name, const glm::vec4& value) = 0;

        virtual void SetUniformInt1(uint32_t location, int value) = 0;
        virtual void SetIntArray(uint32_t location, int* values, uint32_t count) = 0;

        virtual void SetUniformMat4(uint32_t location, const glm::mat4& value) = 0;
        virtual void SetUniformMat3(uint32_t location, const glm::mat3& value) = 0;
        virtual void SetUniformMat4Array(uint32_t location, const glm::mat4& values, uint32_t count) = 0;

        virtual void SetUniformFloat1(uint32_t location, float value) = 0;
        virtual void SetUniformFloat2(uint32_t location, const glm::vec2& value) = 0;
        virtual void SetUniformFloat3(uint32_t location, const glm::vec3& value) = 0;
        virtual void SetUniformFloat4(uint32_t location, const glm::vec4& value) = 0;

        
        virtual void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) = 0;

        virtual uint32_t GetRendererID() const = 0;
        
        static Ref<Shader> Create(const std::string& vertexSrc, const std::string& fragmentSrc);
        static Ref<Shader> Create(const std::string& path);
    };
    
    // ******************************************************************************
    // Implementation to store shaders in a library
    // ******************************************************************************
    class ShaderLibrary
    {
    public:
        ShaderLibrary() = default;
        ~ShaderLibrary() = default;
        
        std::unordered_map<std::string, Ref<Shader>>& GetShaders() { return m_Shaders; }
        
        Ref<Shader> Get(const std::string& name);
        
        Ref<Shader> Load(const std::string& path);
        Ref<Shader> Load(const std::string& name, const std::string& filepath);
        
    private:
        bool Exists(const std::string& name) const;
        
    private:
        std::unordered_map<std::string, Ref<Shader>> m_Shaders;
    };
    
}
