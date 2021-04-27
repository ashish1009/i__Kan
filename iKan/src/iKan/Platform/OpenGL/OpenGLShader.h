// ******************************************************************************
// File         : OpenGLShader.h
// Project      : iKan : Platform
// Description  : Implementation of Open GL Shader
//
// Created on   : 27/04/21.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/Shader.h>
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
        
        virtual void SetUniformInt1(const std::string& name, int value) override;
        virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
        
        virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) override;
        virtual void SetUniformMat3(const std::string& name, const glm::mat3& value) override;
        
        virtual void SetUniformFloat1(const std::string& name, float value) override;
        virtual void SetUniformFloat2(const std::string& name, const glm::vec2& value) override;
        virtual void SetUniformFloat3(const std::string& name, const glm::vec3& value) override;
        virtual void SetUniformFloat4(const std::string& name, const glm::vec4& value) override;

    private:
        void Compile();
        int32_t GetUniformLocation(const std::string& name);
        std::unordered_map<GLenum, std::string> PreprocessFile(const std::string& path);
        std::string ReadFromFile(const std::string& path);

    private:
        uint32_t    m_RendererId;
        std::string m_Name;

        std::unordered_map<std::string, int32_t> m_LocationMap;
        std::unordered_map<GLenum, std::string>  m_Source;
        std::vector<ShaderReloadedCallback>      m_ShaderReloadedCallbacks;
    };
    
}
