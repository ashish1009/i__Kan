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
#include <iKan/Platform/OpenGL/OpenGLShaderUniform.h>
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
        
        virtual void Bind() override;
        virtual void Unbind() override;
        
        virtual void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) override;
        
        virtual const std::string& GetName() override { return m_Name; }
        
        virtual void SetUniformInt1(const std::string& name, int32_t value) override;
        virtual void SetIntArray(const std::string& name, int32_t* values, uint32_t count) override;
        
        virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) override;
        virtual void SetUniformMat3(const std::string& name, const glm::mat3& value) override;
        
        virtual void SetUniformFloat1(const std::string& name, float value) override;
        virtual void SetUniformFloat2(const std::string& name, const glm::vec2& value) override;
        virtual void SetUniformFloat3(const std::string& name, const glm::vec3& value) override;
        virtual void SetUniformFloat4(const std::string& name, const glm::vec4& value) override;

        virtual uint32_t GetRendererID() const override { return m_RendererId; }

    private:
        void Parse();
        void Compile();
        void ResolveUniforms();
        void ParseUniformStruct(const std::string& block, ShaderDomain domain);
        void ParseUniform(const std::string& statement, ShaderDomain domain);

        int32_t GetUniformLocation(const std::string& name);
        std::unordered_map<GLenum, std::string> PreprocessFile(const std::string& path);
        ShaderStruct* FindStruct(const std::string& name);
        std::string ReadFromFile(const std::string& path);

        static GLenum ShaderTypeFromString(const std::string& type);

    private:
        bool m_Loaded    = false;
        bool m_IsCompute = false;

        uint32_t    m_RendererId;
        std::string m_Name, m_AssetPath;

        std::unordered_map<std::string, int32_t> m_LocationMap;
        std::unordered_map<GLenum, std::string>  m_Source;
        std::vector<ShaderReloadedCallback>      m_ShaderReloadedCallbacks;

        // to store the texture slot array from local argument so that it can be passed to Lambda
        int32_t* m_TextureArraySlotData;

        ShaderUniformBufferList m_VSRendererUniformBuffers;
        ShaderUniformBufferList m_PSRendererUniformBuffers;

        Ref<OpenGLShaderUniformBufferDeclaration> m_VSMaterialUniformBuffer;
        Ref<OpenGLShaderUniformBufferDeclaration> m_PSMaterialUniformBuffer;

        ShaderResourceList m_Resources;
        ShaderStructList   m_Structs;
    };
    
}
