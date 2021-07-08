// ******************************************************************************
// File         : OpenGLShader.h
// Description  : Implementation of Open GL Shader
// Project      : iKan : Platform
//
// Created by Ashish on 27/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/Shader.h>
#include <iKan/Platform/OpenGL/OpenGlShaderUniform.h>
#include <glad/glad.h>

namespace iKan {
    
    // ******************************************************************************
    // Implementation of Open GL Shader
    // ******************************************************************************
    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const std::string& vertexSrc, const std::string& fragmantSrc);
        OpenGLShader(const std::string& path);
        
        virtual ~OpenGLShader();
        
        virtual void Bind() const override;
        virtual void Unbind() const override;
        
        virtual void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) override;
        
        virtual const std::string& GetName() override { return m_Name; }

        virtual void UploadUniformBuffer(const UniformBufferBase& uniformBuffer) override;

        virtual void SetVSMaterialUniformBuffer(Buffer buffer) override;
        virtual void SetPSMaterialUniformBuffer(Buffer buffer) override;

        virtual void SetUniformInt1(const std::string& name, int value) override;
        virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
        
        virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) override;
        virtual void SetUniformMat3(const std::string& name, const glm::mat3& value) override;
        
        virtual void SetUniformFloat1(const std::string& name, float value) override;
        virtual void SetUniformFloat2(const std::string& name, const glm::vec2& value) override;
        virtual void SetUniformFloat3(const std::string& name, const glm::vec3& value) override;
        virtual void SetUniformFloat4(const std::string& name, const glm::vec4& value) override;

        virtual void SetUniformInt1(uint32_t location, int value) override;
        virtual void SetIntArray(uint32_t location, int* values, uint32_t count) override;

        virtual void SetUniformMat4(uint32_t location, const glm::mat4& value) override;
        virtual void SetUniformMat3(uint32_t location, const glm::mat3& value) override;
        virtual void SetUniformMat4Array(uint32_t location, const glm::mat4& values, uint32_t count) override;

        virtual void SetUniformFloat1(uint32_t location, float value) override;
        virtual void SetUniformFloat2(uint32_t location, const glm::vec2& value) override;
        virtual void SetUniformFloat3(uint32_t location, const glm::vec3& value) override;
        virtual void SetUniformFloat4(uint32_t location, const glm::vec4& value) override;

        virtual uint32_t GetRendererID() const override { return m_RendererId; }

    private:
        void Compile();
        void Parse();

        void ParseUniformStruct(const std::string& block, ShaderDomain domain);
        void ParseUniform(const std::string& statement, ShaderDomain domain);

        int32_t GetUniformLocation(const std::string& name);
        std::unordered_map<GLenum, std::string> PreprocessFile(const std::string& path);
        std::string ReadFromFile(const std::string& path);

        ShaderStruct* FindStruct(const std::string& name);
        void ResolveUniforms();

        static GLenum ShaderTypeFromString(const std::string& type);

        void ResolveAndSetUniforms(const Ref<OpenGLShaderUniformBufferDeclaration>& decl, Buffer buffer);
        void ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, Buffer buffer);
        void ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field, std::byte* data, int32_t offset);
        void ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform, Buffer buffer);

        void UploadUniformStruct(OpenGLShaderUniformDeclaration* uniform, std::byte* buffer, uint32_t offset);

        virtual const ShaderUniformBufferList& GetVSRendererUniforms() const override { return m_VSRendererUniformBuffers; }
        virtual const ShaderUniformBufferList& GetPSRendererUniforms() const override { return m_PSRendererUniformBuffers; }
        virtual bool HasVSMaterialUniformBuffer() const override { return (bool)m_VSMaterialUniformBuffer; }
        virtual bool HasPSMaterialUniformBuffer() const override { return (bool)m_PSMaterialUniformBuffer; }
        virtual const ShaderUniformBufferDeclaration& GetVSMaterialUniformBuffer() const override { return *m_VSMaterialUniformBuffer; }
        virtual const ShaderUniformBufferDeclaration& GetPSMaterialUniformBuffer() const override { return *m_PSMaterialUniformBuffer; }
        virtual const ShaderResourceList& GetResources() const override { return m_Resources; }

    private:
        bool m_Loaded    = false;
        bool m_IsCompute = false;

        uint32_t    m_RendererId;
        std::string m_Name, m_AssetPath;

        std::unordered_map<std::string, int32_t> m_LocationMap;
        std::unordered_map<GLenum, std::string>  m_Source;
        std::vector<ShaderReloadedCallback>      m_ShaderReloadedCallbacks;

        ShaderUniformBufferList m_VSRendererUniformBuffers;
        ShaderUniformBufferList m_PSRendererUniformBuffers;

        Ref<OpenGLShaderUniformBufferDeclaration> m_VSMaterialUniformBuffer;
        Ref<OpenGLShaderUniformBufferDeclaration> m_PSMaterialUniformBuffer;

        ShaderResourceList m_Resources;
        ShaderStructList m_Structs;
    };
    
}
