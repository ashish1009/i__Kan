// ******************************************************************************
// File         : OpenGLShader.cpp
// Description  : Implementation of Open GL Shader
// Project      : iKan : Platform
//
// Created by Ashish on 27/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "OpenGLShader.h"
#include <iKan/Renderer/Renderer.h>
#include <glad/glad.h>

namespace iKan {
    
    // ******************************************************************************
    // Get the Open GL Shader Type from string
    // ******************************************************************************
    static GLenum ShaderTypeFromString(const std::string& type)
    {
        if ("vertex" == type)   return GL_VERTEX_SHADER;
        if ("fragment" == type) return GL_FRAGMENT_SHADER;
        if ("geometry" == type) return GL_GEOMETRY_SHADER;

        IK_CORE_ASSERT(false, "Unknown shader type!");
        return 0;
    }
    
    // ******************************************************************************
    // Open GL Constructor
    // ******************************************************************************
    OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        IK_CORE_INFO("Open GL Constructed from strings ");

        m_Source[GL_VERTEX_SHADER]   = vertexSrc;
        m_Source[GL_FRAGMENT_SHADER] = fragmentSrc;
        
        Compile();
    }
    
    // ******************************************************************************
    // Open GL Constructor
    // ******************************************************************************
    OpenGLShader::OpenGLShader(const std::string& path)
    {
        IK_CORE_INFO("Open GL Shader Constructed from path : {0}", path.c_str());

        std::string source  = ReadFromFile(path);
        m_Source            = PreprocessFile(source);
        
        // Extract the name
        m_Name = Utils::GetNameFromFilePath(path);
        
        Compile();
    }
    
    // ******************************************************************************
    // Open GL Read from file
    // ******************************************************************************
    std::string OpenGLShader::ReadFromFile(const std::string& path)
    {
        IK_CORE_INFO("Reading Open GL Shdaer from file : {0}", path.c_str());

        std::string result;
        std::ifstream in(path, std::ios::in | std::ios::binary);
        if (in)
        {
            in.seekg(0, std::ios::end);
            size_t size = in.tellg();
            if (-1 != size)
            {
                result.resize(size);
                in.seekg(0, std::ios::beg);
                in.read(&result[0], size);
                in.close();
            }
            else
            {
                IK_CORE_ERROR("Could not read from file '{0}'", path);
            }
        }
        else
        {
            IK_CORE_ERROR("Could not open file '{0}'", path);
        }
        
        return result;
    }
    
    // ******************************************************************************
    // Process the Shader file
    // ******************************************************************************
    std::unordered_map<GLenum, std::string> OpenGLShader::PreprocessFile(const std::string& source)
    {
        IK_CORE_INFO("Process the Open GL Shader: {0}", m_Name.c_str());

        std::unordered_map<GLenum, std::string> shaderSource;
        
        const char* token = "#type";
        size_t pos        = source.find(token);
        while (pos != std::string::npos)
        {
            size_t eol = source.find_first_of("\r\n", pos);
            IK_CORE_ASSERT((eol != std::string::npos), "Syntax error");

            size_t begin     = pos + strlen(token) + 1;
            std::string type = source.substr(begin, eol - begin);
            IK_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");
            
            size_t nextLine = source.find_first_of("\r\n", eol);
            pos             = source.find(token, nextLine);
            
            shaderSource[ShaderTypeFromString(type)] = source.substr(nextLine, pos - (nextLine == std::string::npos ? source.size() - 1 : nextLine));
        }
        
        return shaderSource;
    }
    
    // ******************************************************************************
    // Compile the Shader file
    // ******************************************************************************
    void OpenGLShader::Compile()
    {
        Renderer::Submit([this]()
                         {
            IK_CORE_INFO("Compiling Open GL Shader: {0} ", m_Name.c_str());

            int32_t program = glCreateProgram();
            int32_t glShaderIDIndex = 0;
            std::array<uint32_t, 3> shaderId;

            for (auto& kv : m_Source)
            {
                GLenum type = kv.first;
                std::string src = kv.second;

                uint32_t shader = glCreateShader(type);

                // Attch the shader source and then compile
                const char* string = src.c_str();
                glShaderSource(shader, 1, &string, nullptr);
                glCompileShader(shader);

                // Error Handling
                GLint isCompiled = 0;
                glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
                if (isCompiled == GL_FALSE)
                {
                    GLint maxLength = 0;
                    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

                    std::vector<GLchar> infoLog(maxLength);
                    glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

                    glDeleteShader(shader);

                    IK_CORE_ERROR("{0}", infoLog.data());
                    IK_CORE_ASSERT(false, "Shader compilation failure!");
                }
                // Attach both shader and link them
                glAttachShader(program, shader);
                shaderId[glShaderIDIndex++] = shader;
            }

            // Create Shader program to activate and linke the shader
            m_RendererId = program;

            glLinkProgram(m_RendererId);

            // Error Handling
            // Note the different functions here: glGetProgram* instead of glGetShader
            GLint isLinked = 0;
            glGetProgramiv(m_RendererId, GL_LINK_STATUS, (int32_t*)&isLinked);
            if (isLinked == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetProgramiv(m_RendererId, GL_INFO_LOG_LENGTH, &maxLength);

                /* The maxLength includes the NULL character */
                std::vector<GLchar> infoLog(maxLength);
                glGetProgramInfoLog(m_RendererId, maxLength, &maxLength, &infoLog[0]);

                /* We don't need the program anymore. */
                glDeleteProgram(m_RendererId);

                for (auto id : shaderId)
                {
                    glDeleteShader(id);
                }

                IK_CORE_ERROR("{0}", infoLog.data());
                IK_CORE_ASSERT(false, "Shader link failure!");
            }
            for (auto id : shaderId)
                glDeleteShader(id);
        });
    }
    
    // ******************************************************************************
    // Destructor for Open GL Shader file
    // ******************************************************************************
    OpenGLShader::~OpenGLShader()
    {
        Renderer::Submit([this]()
                         {
            IK_CORE_WARN("Destroying Open GL Shader : {0}", m_Name.c_str());
            glDeleteProgram(m_RendererId);
        });
    }
    
    // ******************************************************************************
    // Bind Open GL Shader file
    // ******************************************************************************
    void OpenGLShader::Bind()
    {
        Renderer::Submit([this]() { glUseProgram(m_RendererId); });
    }
    
    // ******************************************************************************
    // Unbind Open GL Shader file
    // ******************************************************************************
    void OpenGLShader::Unbind()
    {
        Renderer::Submit([]() { glUseProgram(0); });
    }
    
    // ******************************************************************************
    // Destructor for Open GL Shader file
    // ******************************************************************************
    void OpenGLShader::AddShaderReloadedCallback(const ShaderReloadedCallback& callback)
    {
        m_ShaderReloadedCallbacks.push_back(callback);
    }
    
    //-------------------------------- Uniforms --------------------------------
    void OpenGLShader::SetUniformInt1(const std::string& name, int32_t value)
    {
        glUniform1i(GetUniformLocation(name), value);
    }
    
    void OpenGLShader::SetIntArray(const std::string& name, int32_t* values, uint32_t count)
    {
        m_TextureArraySlotData = new int32_t[count];
        memcpy(m_TextureArraySlotData, values, count * sizeof(int32_t));
        Renderer::Submit([name, values, count, this]() { glUniform1iv(GetUniformLocation(name), count, m_TextureArraySlotData); });
    }
    
    void OpenGLShader::SetUniformMat4(const std::string& name, const glm::mat4& value)
    {
        Renderer::Submit([name, value, this]() { glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value)); });
    }

    void OpenGLShader::SetUniformMat3(const std::string& name, const glm::mat3& value)
    {
        Renderer::Submit([name, value, this]() { glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value)); });
    }
    
    void OpenGLShader::SetUniformFloat1(const std::string& name, float value)
    {
        Renderer::Submit([name, value, this]() { glUniform1f(GetUniformLocation(name), value); });
    }
    
    void OpenGLShader::SetUniformFloat2(const std::string& name, const glm::vec2& value)
    {
        Renderer::Submit([name, value, this]() { glUniform2f(GetUniformLocation(name), value.x, value.y); });
    }
    
    void OpenGLShader::SetUniformFloat3(const std::string& name, const glm::vec3& value)
    {
        Renderer::Submit([name, value, this]() { glUniform3f(GetUniformLocation(name), value.x, value.y, value.z); });
    }
    
    void OpenGLShader::SetUniformFloat4(const std::string& name, const glm::vec4& value)
    {
        Renderer::Submit([name, value, this]() { glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w); });
    }
    
    // ******************************************************************************
    // get the uniform Location
    // ******************************************************************************
    int32_t OpenGLShader::GetUniformLocation(const std::string& name)
    {
        if (m_LocationMap.find(name) != m_LocationMap.end())
            return m_LocationMap[name];
        
        int32_t location = glGetUniformLocation(m_RendererId, name.c_str());
        if (-1 == location)
            IK_CORE_WARN("Warning: uniform '{0}' doesnt exist", name);

        m_LocationMap[name] = location;
        return location;
    }

    
}
