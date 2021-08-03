// ******************************************************************************
// File         : Shader.h
// Description  : Interface for Shader class
// Project      : i-Kan : Renderer
//
// Created by Ashish on 27/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

namespace iKan {
    
    // ******************************************************************************
    // Interface for Graphics Shader
    // ******************************************************************************
    class Shader
    {
    public:
        using ShaderReloadedCallback = std::function<void()>;

        virtual ~Shader() = default;
        
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
        
        virtual const std::string& GetName() = 0;
        
        virtual void SetUniformInt1(const std::string& name, int32_t value) = 0;
        virtual void SetIntArray(const std::string& name, int32_t* values, uint32_t count) = 0;
        
        virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) = 0;
        virtual void SetUniformMat3(const std::string& name, const glm::mat3& value) = 0;
        
        virtual void SetUniformFloat1(const std::string& name, float value) = 0;
        virtual void SetUniformFloat2(const std::string& name, const glm::vec2& value) = 0;
        virtual void SetUniformFloat3(const std::string& name, const glm::vec3& value) = 0;
        virtual void SetUniformFloat4(const std::string& name, const glm::vec4& value) = 0;
        
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
