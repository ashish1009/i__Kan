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
    // Return the pointer of first occurance of token, token should be alone
    // (space or nothing on left and right side)
    // NOTE: it is assumed that struct and uniform in the shader code should be alone
    // eg. struct { } or uniform ...
    // str   : base string
    // token : to be searched
    // ******************************************************************************
    static const char* FindToken(const char* str, const std::string& token)
    {
        const char* t = str;
        while ((t = strstr(t, token.c_str())))
        {
            // Check if left character of token is empty
            // TODO: '/' might chnage if wont work for some scenario, using '/' as indicator
            // of first character of line
            IK_INFO("{0}, {1}, {2}, {3}", t[-1], str == t, isspace(t[-1]), t[-1] == '/');
            bool left = str == t || isspace(t[-1]) || t[-1] == '/';

            // Check if right character of token is empty
            bool right = !t[token.size()] || isspace(t[token.size()]);

            if (left && right)
                return t;

            t += token.size();
        }
        return nullptr;
    }

    // ******************************************************************************
    // Extract the struct code from the shader string "struct { ... }"
    // return the pointer to 's' in "struct { ... }"
    // src         : token
    // outPosition : string pointer of the shader code, that will move to some offset
    //               offset is the "}" of the strcut
    // ******************************************************************************
    static std::string GetBlock(const char* str, const char** outPosition)
    {
        const char* end = strstr(str, "}");
        if (!end)
            return str;

        // offset the shader string to the end of current strcut
        if (outPosition)
            *outPosition = end;

        uint32_t length = (uint32_t)(end - str + 1);
        return std::string(str, length);
    }

    // ******************************************************************************
    // Extract the Code statement before ";"
    // ******************************************************************************
    std::string GetStatement(const char* str, const char** outPosition)
    {
        const char* end = strstr(str, ";");
        if (!end)
            return str;

        if (outPosition)
            *outPosition = end;
        uint32_t length = (uint32_t)(end - str + 1);
        return std::string(str, length);
    }

    // ******************************************************************************
    // Break the string. Break points are stored in delimiter, any character in the
    // delimeter will be cosnider as break points
    // ******************************************************************************
    std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters)
    {
        size_t start = 0;
        size_t end = string.find_first_of(delimiters);

        std::vector<std::string> result;

        while (end <= std::string::npos)
        {
            std::string token = string.substr(start, end - start);
            if (!token.empty())
                result.push_back(token);

            if (end == std::string::npos)
                break;

            start = end + 1;
            end = string.find_first_of(delimiters, start);
        }

        return result;
    }

    // ******************************************************************************
    // ******************************************************************************
    std::vector<std::string> SplitString(const std::string& string, const char delimiter)
    {
        return SplitString(string, std::string(1, delimiter));
    }

    // ******************************************************************************
    // ******************************************************************************
    std::vector<std::string> Tokenize(const std::string& string)
    {
        return SplitString(string, " \t\n");
    }

    // ******************************************************************************
    // ******************************************************************************
    std::vector<std::string> GetLines(const std::string& string)
    {
        return SplitString(string, "\n");
    }

    // ******************************************************************************
    // get the type of uniform is premitive or not
    // ******************************************************************************
    static bool IsTypeStringResource(const std::string& type)
    {
        if (type == "sampler2D")          return true;
        if (type == "sampler2DMS")        return true;
        if (type == "samplerCube")        return true;
        if (type == "sampler2DShadow")    return true;
        return false;
    }

    // ******************************************************************************
    // ******************************************************************************
    bool StartsWith(const std::string& string, const std::string& start)
    {
        return string.find(start) == 0;
    }
    
    // ******************************************************************************
    // Get the Open GL Shader Type from string
    // ******************************************************************************
    GLenum OpenGLShader::ShaderTypeFromString(const std::string& type)
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
    : m_AssetPath(path)
    {
        IK_CORE_INFO("Open GL Shader Constructed from path : {0}", path.c_str());

        // Find the last '/' and extract the rest of the string
        size_t found = path.find_last_of("/\\");
        m_Name       = found != std::string::npos ? path.substr(found + 1) : path;

        // Find the last '.' and extract the rest of the string
        found  = m_Name.find_last_of(".");
        m_Name = found != std::string::npos ? m_Name.substr(0, found) : m_Name;

        std::string source  = ReadFromFile(path);
        m_Source            = PreprocessFile(source);
        
        // Extract the name
        m_Name = Utils::GetNameFromFilePath(path);

        if (!m_IsCompute)
            Parse();

        Compile();

        if (!m_IsCompute)
            ResolveUniforms();

        if (m_Loaded)
            for (auto& callback : m_ShaderReloadedCallbacks)
                callback();

        m_Loaded = true;
    }

    // ******************************************************************************
    // Find the structure from stored ones by name
    // ******************************************************************************
    ShaderStruct* OpenGLShader::FindStruct(const std::string& name)
    {
        for (ShaderStruct* s : m_Structs)
            if (s->GetName() == name)
                return s;

        return nullptr;
    }

    // ******************************************************************************
    // Psrse the compute shader
    // ******************************************************************************
    void OpenGLShader::Parse()
    {
        const char* token;
        const char* vstr;
        const char* fstr;

        m_Resources.clear();
        m_Structs.clear();

        m_VSMaterialUniformBuffer.reset();
        m_PSMaterialUniformBuffer.reset();

        auto& vertexSource   = m_Source[GL_VERTEX_SHADER];
        auto& fragmentSource = m_Source[GL_FRAGMENT_SHADER];

        // Vertex Shader
        vstr = vertexSource.c_str();
        while ((token = FindToken(vstr, "struct")))
            ParseUniformStruct(GetBlock(token, &vstr), ShaderDomain::Vertex);

        vstr = vertexSource.c_str();
        while ((token = FindToken(vstr, "uniform")))
            ParseUniform(GetStatement(token, &vstr), ShaderDomain::Vertex);

        // Fragment Shader
        fstr = fragmentSource.c_str();
        while ((token = FindToken(fstr, "struct")))
            ParseUniformStruct(GetBlock(token, &fstr), ShaderDomain::Pixel);

        fstr = fragmentSource.c_str();
        while ((token = FindToken(fstr, "uniform")))
            ParseUniform(GetStatement(token, &fstr), ShaderDomain::Pixel);
    }

    // ******************************************************************************
    // Parse the uniforms, like uniform texture, sampler2D ...
    // ******************************************************************************
    void OpenGLShader::ParseUniform(const std::string& statement, ShaderDomain domain)
    {
        std::vector<std::string> tokens = Tokenize(statement);
        uint32_t index = 1; // 0th is for keyword unifrom

        std::string typeString = tokens[index++];
        std::string name = tokens[index++];

        // Strip ; from name if present
        if (const char* s = strstr(name.c_str(), ";"))
            name = std::string(name.c_str(), s - name.c_str());

        std::string n(name);
        int32_t count = 1;
        const char* namestr = n.c_str();
        if (const char* s = strstr(namestr, "["))
        {
            name = std::string(namestr, s - namestr);

            const char* end = strstr(namestr, "]");
            std::string c(s + 1, end - s);
            count = atoi(c.c_str());
        }

        if (IsTypeStringResource(typeString))
        {
            ShaderResourceDeclaration* declaration = new OpenGLShaderResourceDeclaration(OpenGLShaderResourceDeclaration::StringToType(typeString), name, count);
            m_Resources.push_back(declaration);
        }
        else
        {
            OpenGLShaderUniformDeclaration::Type t = OpenGLShaderUniformDeclaration::StringToType(typeString);
            OpenGLShaderUniformDeclaration* declaration = nullptr;

            if (t == OpenGLShaderUniformDeclaration::Type::NONE)
            {
                // Find struct
                ShaderStruct* s = FindStruct(typeString);
                IK_CORE_ASSERT(s, "");
                declaration = new OpenGLShaderUniformDeclaration(domain, s, name, count);
            }
            else
            {
                declaration = new OpenGLShaderUniformDeclaration(domain, t, name, count);
            }

            if (StartsWith(name, "r_"))
            {
                if (domain == ShaderDomain::Vertex)
                    ((OpenGLShaderUniformBufferDeclaration*)m_VSRendererUniformBuffers.front())->PushUniform(declaration);
                else if (domain == ShaderDomain::Pixel)
                    ((OpenGLShaderUniformBufferDeclaration*)m_PSRendererUniformBuffers.front())->PushUniform(declaration);
            }
            else
            {
                if (domain == ShaderDomain::Vertex)
                {
                    if (!m_VSMaterialUniformBuffer)
                        m_VSMaterialUniformBuffer.reset(new OpenGLShaderUniformBufferDeclaration("", domain));

                    m_VSMaterialUniformBuffer->PushUniform(declaration);
                }
                else if (domain == ShaderDomain::Pixel)
                {
                    if (!m_PSMaterialUniformBuffer)
                        m_PSMaterialUniformBuffer.reset(new OpenGLShaderUniformBufferDeclaration("", domain));

                    m_PSMaterialUniformBuffer->PushUniform(declaration);
                }
            }
        }
    }

    // ******************************************************************************
    // block  : struct block string "struct { ... }"
    // domain : type of shader
    // ******************************************************************************
    void OpenGLShader::ParseUniformStruct(const std::string& block, ShaderDomain domain)
    {
        // get each word from the block and store them in vector
        std::vector<std::string> tokens = Tokenize(block);

        uint32_t index = 0;
        index++;  // "struct"

        std::string name = tokens[index++];
        ShaderStruct* uniformStruct = new ShaderStruct(name);
        index++;
        while (index < tokens.size())
        {
            if (tokens[index] == "}")
                break;

            std::string type = tokens[index++];
            std::string name = tokens[index++];

            // Strip ; from name if present
            if (const char* s = strstr(name.c_str(), ";"))
                name = std::string(name.c_str(), s - name.c_str());

            uint32_t count = 1;
            const char* namestr = name.c_str();
            if (const char* s = strstr(namestr, "["))
            {
                auto tempName = std::string(namestr, s - namestr);

                const char* end = strstr(namestr, "]");
                std::string c(s + 1, end - s);
                count = atoi(c.c_str());

                name = tempName;
            }
            ShaderUniformDeclaration* field = new OpenGLShaderUniformDeclaration(domain, OpenGLShaderUniformDeclaration::StringToType(type), name, count);
            uniformStruct->AddField(field);
        }
        m_Structs.push_back(uniformStruct);
    }

    // ******************************************************************************
    // ******************************************************************************
    void OpenGLShader::ResolveUniforms()
    {
        Renderer::Submit([this]()
                         {
            glUseProgram(m_RendererId);

            for (size_t i = 0; i < m_VSRendererUniformBuffers.size(); i++)
            {
                OpenGLShaderUniformBufferDeclaration* decl = (OpenGLShaderUniformBufferDeclaration*)m_VSRendererUniformBuffers[i];
                const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
                for (size_t j = 0; j < uniforms.size(); j++)
                {
                    OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[j];
                    if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
                    {
                        const ShaderStruct& s = uniform->GetShaderUniformStruct();
                        const auto& fields = s.GetFields();
                        for (size_t k = 0; k < fields.size(); k++)
                        {
                            OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
                            field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
                        }
                    }
                    else
                    {
                        uniform->m_Location = GetUniformLocation(uniform->m_Name);
                    }
                }
            }

            for (size_t i = 0; i < m_PSRendererUniformBuffers.size(); i++)
            {
                OpenGLShaderUniformBufferDeclaration* decl = (OpenGLShaderUniformBufferDeclaration*)m_PSRendererUniformBuffers[i];
                const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
                for (size_t j = 0; j < uniforms.size(); j++)
                {
                    OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[j];
                    if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
                    {
                        const ShaderStruct& s = uniform->GetShaderUniformStruct();
                        const auto& fields = s.GetFields();
                        for (size_t k = 0; k < fields.size(); k++)
                        {
                            OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
                            field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
                        }
                    }
                    else
                    {
                        uniform->m_Location = GetUniformLocation(uniform->m_Name);
                    }
                }
            }

            {
                const auto& decl = m_VSMaterialUniformBuffer;
                if (decl)
                {
                    const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
                    for (size_t j = 0; j < uniforms.size(); j++)
                    {
                        OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[j];
                        if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
                        {
                            const ShaderStruct& s = uniform->GetShaderUniformStruct();
                            const auto& fields = s.GetFields();
                            for (size_t k = 0; k < fields.size(); k++)
                            {
                                OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
                                field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
                            }
                        }
                        else
                        {
                            uniform->m_Location = GetUniformLocation(uniform->m_Name);
                        }
                    }
                }
            }

            {
                const auto& decl = m_PSMaterialUniformBuffer;
                if (decl)
                {
                    const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
                    for (size_t j = 0; j < uniforms.size(); j++)
                    {
                        OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[j];
                        if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
                        {
                            const ShaderStruct& s = uniform->GetShaderUniformStruct();
                            const auto& fields = s.GetFields();
                            for (size_t k = 0; k < fields.size(); k++)
                            {
                                OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
                                field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
                            }
                        }
                        else
                        {
                            uniform->m_Location = GetUniformLocation(uniform->m_Name);
                        }
                    }
                }
            }

            uint32_t sampler = 0;
            for (size_t i = 0; i < m_Resources.size(); i++)
            {
                OpenGLShaderResourceDeclaration* resource = (OpenGLShaderResourceDeclaration*)m_Resources[i];
                int32_t location = GetUniformLocation(resource->m_Name);

                if (resource->GetCount() == 1)
                {
                    resource->m_Register = sampler;
                    if (location != -1)
                        SetUniformInt1(resource->m_Name, sampler);

                    sampler++;
                }
                else if (resource->GetCount() > 1)
                {
                    resource->m_Register = 0;
                    uint32_t count = resource->GetCount();
                    int* samplers = new int[count];
                    for (uint32_t s = 0; s < count; s++)
                        samplers[s] = s;
                    SetIntArray(resource->GetName(), samplers, count);
                    delete[] samplers;
                }
            }
        });
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
