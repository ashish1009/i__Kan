// ******************************************************************************
// File         : OpenGLShader.cpp
// Description  : Implementation of Open GL Shader
// Project      : iKan : Platform
//
// Created by Ashish on 27/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include <iKan/Platform/OpenGL/OpenGLShader.h>
#include <glad/glad.h>

namespace iKan {

    // ******************************************************************************
    // Return the pointer of first occurance of token, token should be alone
    // (space or nothing on left and right side)
    // NOTE: it is assumed that struct in the shader code should be alone
    // eg. struct
    //     { }
    // str   : base string
    // token : to be searched
    // ******************************************************************************
    static const char* FindToken(const char* str, const std::string& token)
    {
        const char* t = str;
        while ((t = strstr(t, token.c_str())))
        {
            // Check if left character of token is empty
            bool left = str == t || isspace(t[-1]);

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
    // Is it Premitive data type or not
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

        if (!m_IsCompute)
            Parse();
        
        Compile();

        if (!m_IsCompute)
        {
            ResolveUniforms();
        }

        if (m_Loaded)
        {
            for (auto& callback : m_ShaderReloadedCallbacks)
                callback();
        }

        m_Loaded = true;
    }

    // ******************************************************************************
    // ******************************************************************************
    ShaderStruct* OpenGLShader::FindStruct(const std::string& name)
    {
        for (ShaderStruct* s : m_Structs)
        {
            if (s->GetName() == name)
                return s;
        }
        return nullptr;
    }

    // ******************************************************************************
    // Psrse the the shader and extract the structure and uniforms
    // ******************************************************************************
    void OpenGLShader::Parse()
    {
        IK_CORE_INFO("Parse the shader {0}", m_Name.c_str());

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
    // Parse the Uniform and store them in Shader class eg.
    // uniform int a;
    // uniform sampler2D b;
    // uniform ABC abc;
    // block  : struct block string "struct { ... }"
    // domain : type of shader
    // ******************************************************************************
    void OpenGLShader::ParseUniform(const std::string& statement, ShaderDomain domain)
    {
        IK_CORE_INFO("Parse the Uniform from Shader {0}", m_Name.c_str());

        std::vector<std::string> tokens = Tokenize(statement);
        uint32_t index = 0;

        index++; // "uniform"
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
                IK_CORE_ASSERT(s, "No struct is created in the shader");
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
    // Parse the structure and store them in Shader class. Vector of structure
    // block  : struct block string "struct { ... }"
    // domain : type of shader
    // ******************************************************************************
    void OpenGLShader::ParseUniformStruct(const std::string& block, ShaderDomain domain)
    {
        IK_CORE_INFO("Parse the Uniform struct Shader {0}", m_Name.c_str());

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
    // Update the location of the Stored Uniforms
    // TODO: Chanege loop to iterator after testing
    // ******************************************************************************
    void OpenGLShader::ResolveUniforms()
    {
        IK_CORE_INFO("Resolve the uniforms from shader {0}", m_Name.c_str());

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

        std::unordered_map<GLenum, std::string> shaderSources;
        
        const char* token      = "#type";
        size_t pos             = source.find(token);
        while (pos != std::string::npos)
        {
            size_t eol = source.find_first_of("\r\n", pos);
            IK_CORE_ASSERT((eol != std::string::npos), "Syntax error");

            size_t begin     = pos + strlen(token) + 1;
            std::string type = source.substr(begin, eol - begin);
            IK_CORE_ASSERT((type == "vertex" || type == "fragment" || type == "pixel" || type == "compute"), "Invalid shader type specified");
            
            size_t nextLine = source.find_first_of("\r\n", eol);
            pos             = source.find(token, nextLine);
            auto shaderType = ShaderTypeFromString(type);

            shaderSources[shaderType] = source.substr(nextLine, pos - (nextLine == std::string::npos ? source.size() - 1 : nextLine));

            // Compute shaders cannot contain other types
            if (shaderType == GL_COMPUTE_SHADER)
            {
                m_IsCompute = true;
                break;
            }
        }
        
        return shaderSources;
    }
    
    // ******************************************************************************
    // Compile the Shader file
    // ******************************************************************************
    void OpenGLShader::Compile()
    {
        IK_CORE_INFO("Compiling Open GL Shader: {0} ", m_Name.c_str());

        GLuint program = glCreateProgram();
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
    }
    
    // ******************************************************************************
    // Destructor for Open GL Shader file
    // ******************************************************************************
    OpenGLShader::~OpenGLShader()
    {
        IK_CORE_WARN("Destroying Open GL Shader : {0}", m_Name.c_str());

        glDeleteProgram(m_RendererId);
    }
    
    // ******************************************************************************
    // Bind Open GL Shader file
    // ******************************************************************************
    void OpenGLShader::Bind() const
    {
        glUseProgram(m_RendererId);
    }
    
    // ******************************************************************************
    // Unbind Open GL Shader file
    // ******************************************************************************
    void OpenGLShader::Unbind() const
    {
        glUseProgram(0);
    }
    
    // ******************************************************************************
    // Destructor for Open GL Shader file
    // ******************************************************************************
    void OpenGLShader::AddShaderReloadedCallback(const ShaderReloadedCallback& callback)
    {
        m_ShaderReloadedCallbacks.push_back(callback);
    }

    // ******************************************************************************
    // ******************************************************************************
    void OpenGLShader::ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, Buffer buffer)
    {
        if (uniform->GetLocation() == -1)
            return;

        IK_CORE_ASSERT((uniform->GetLocation() != -1), "Uniform has invalid location!");

        uint32_t offset = uniform->GetOffset();
        switch (uniform->GetType())
        {
            case OpenGLShaderUniformDeclaration::Type::FLOAT32:
                SetUniformFloat1(uniform->GetLocation(), *(float*)&buffer.Data[offset]);
                break;
            case OpenGLShaderUniformDeclaration::Type::INT32:
                SetUniformInt1(uniform->GetLocation(), *(int32_t*)&buffer.Data[offset]);
                break;
            case OpenGLShaderUniformDeclaration::Type::VEC2:
                SetUniformFloat2(uniform->GetLocation(), *(glm::vec2*)&buffer.Data[offset]);
                break;
            case OpenGLShaderUniformDeclaration::Type::VEC3:
                SetUniformFloat3(uniform->GetLocation(), *(glm::vec3*)&buffer.Data[offset]);
                break;
            case OpenGLShaderUniformDeclaration::Type::VEC4:
                SetUniformFloat4(uniform->GetLocation(), *(glm::vec4*)&buffer.Data[offset]);
                break;
            case OpenGLShaderUniformDeclaration::Type::MAT3:
                SetUniformMat3(uniform->GetLocation(), *(glm::mat3*)&buffer.Data[offset]);
                break;
            case OpenGLShaderUniformDeclaration::Type::MAT4:
                SetUniformMat4(uniform->GetLocation(), *(glm::mat4*)&buffer.Data[offset]);
                break;
            case OpenGLShaderUniformDeclaration::Type::STRUCT:
                UploadUniformStruct(uniform, buffer.Data, offset);
                break;
            default:
                IK_CORE_ASSERT(false, "Unknown uniform type!");
        }
    }

    // ******************************************************************************
    // ******************************************************************************
    void OpenGLShader::ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field, std::byte* data, int32_t offset)
    {
        switch (field.GetType())
        {
            case OpenGLShaderUniformDeclaration::Type::FLOAT32:
                SetUniformFloat1(field.GetLocation(), *(float*)&data[offset]);
                break;
            case OpenGLShaderUniformDeclaration::Type::INT32:
                SetUniformInt1(field.GetLocation(), *(int32_t*)&data[offset]);
                break;
            case OpenGLShaderUniformDeclaration::Type::VEC2:
                SetUniformFloat2(field.GetLocation(), *(glm::vec2*)&data[offset]);
                break;
            case OpenGLShaderUniformDeclaration::Type::VEC3:
                SetUniformFloat3(field.GetLocation(), *(glm::vec3*)&data[offset]);
                break;
            case OpenGLShaderUniformDeclaration::Type::VEC4:
                SetUniformFloat4(field.GetLocation(), *(glm::vec4*)&data[offset]);
                break;
            case OpenGLShaderUniformDeclaration::Type::MAT3:
                SetUniformMat3(field.GetLocation(), *(glm::mat3*)&data[offset]);
                break;
            case OpenGLShaderUniformDeclaration::Type::MAT4:
                SetUniformMat4(field.GetLocation(), *(glm::mat4*)&data[offset]);
                break;
            default:
                IK_CORE_ASSERT(false, "Unknown uniform type!");
        }
    }

    // ******************************************************************************
    // ******************************************************************************
    void OpenGLShader::ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform, Buffer buffer)
    {
        //HZ_CORE_ASSERT(uniform->GetLocation() != -1, "Uniform has invalid location!");

        uint32_t offset = uniform->GetOffset();
        switch (uniform->GetType())
        {
            case OpenGLShaderUniformDeclaration::Type::FLOAT32:
                SetUniformFloat1(uniform->GetLocation(), *(float*)&buffer.Data[offset]);
                break;
            case OpenGLShaderUniformDeclaration::Type::INT32:
                SetUniformInt1(uniform->GetLocation(), *(int32_t*)&buffer.Data[offset]);
                break;
            case OpenGLShaderUniformDeclaration::Type::VEC2:
                SetUniformFloat2(uniform->GetLocation(), *(glm::vec2*)&buffer.Data[offset]);
                break;
            case OpenGLShaderUniformDeclaration::Type::VEC3:
                SetUniformFloat3(uniform->GetLocation(), *(glm::vec3*)&buffer.Data[offset]);
                break;
            case OpenGLShaderUniformDeclaration::Type::VEC4:
                SetUniformFloat4(uniform->GetLocation(), *(glm::vec4*)&buffer.Data[offset]);
                break;
            case OpenGLShaderUniformDeclaration::Type::MAT3:
                SetUniformMat3(uniform->GetLocation(), *(glm::mat3*)&buffer.Data[offset]);
                break;
            case OpenGLShaderUniformDeclaration::Type::MAT4:
                SetUniformMat4Array(uniform->GetLocation(), *(glm::mat4*)&buffer.Data[offset], uniform->GetCount());
                break;
            case OpenGLShaderUniformDeclaration::Type::STRUCT:
                UploadUniformStruct(uniform, buffer.Data, offset);
                break;
            default:
                IK_CORE_ASSERT(false, "Unknown uniform type!");
        }
    }

    // ******************************************************************************
    // ******************************************************************************
    void OpenGLShader::ResolveAndSetUniforms(const Ref<OpenGLShaderUniformBufferDeclaration>& decl, Buffer buffer)
    {
        const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
        for (size_t i = 0; i < uniforms.size(); i++)
        {
            OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[i];
            if (uniform->IsArray())
                ResolveAndSetUniformArray(uniform, buffer);
            else
                ResolveAndSetUniform(uniform, buffer);
        }
    }

    // ******************************************************************************
    // ******************************************************************************
    void OpenGLShader::UploadUniformBuffer(const UniformBufferBase& uniformBuffer)
    {
        for (uint32_t i = 0; i < uniformBuffer.GetUniformCount(); i++)
        {
            const UniformDecl& decl = uniformBuffer.GetUniforms()[i];
            switch (decl.Type)
            {
                case UniformType::None:
                {
                    IK_CORE_ASSERT(false, "Invalid");
                }
                case UniformType::Int32:
                {
                    const std::string& name = decl.Name;
                    int32_t value = *(int32_t*)(uniformBuffer.GetBuffer() + decl.Offset);
                    SetUniformInt1(name, value);
                }
                case UniformType::Uint32:
                {
                    const std::string& name = decl.Name;
                    uint32_t value = *(uint32_t*)(uniformBuffer.GetBuffer() + decl.Offset);
                    SetUniformInt1(name, value);
                }
                case UniformType::Float:
                {
                    const std::string& name = decl.Name;
                    float value = *(float*)(uniformBuffer.GetBuffer() + decl.Offset);
                    SetUniformFloat1(name, value);
                }
                case UniformType::Float3:
                {
                    const std::string& name = decl.Name;
                    glm::vec3& values = *(glm::vec3*)(uniformBuffer.GetBuffer() + decl.Offset);
                    SetUniformFloat3(name, values);
                }
                case UniformType::Float2:
                {
                    const std::string& name = decl.Name;
                    glm::vec2& values = *(glm::vec2*)(uniformBuffer.GetBuffer() + decl.Offset);
                    SetUniformFloat2(name, values);
                }
                case UniformType::Float4:
                {
                    const std::string& name = decl.Name;
                    glm::vec4& values = *(glm::vec4*)(uniformBuffer.GetBuffer() + decl.Offset);
                    SetUniformFloat4(name, values);
                }
                case UniformType::Matrix3x3:
                {
                    const std::string& name = decl.Name;
                    glm::mat3& values = *(glm::mat3*)(uniformBuffer.GetBuffer() + decl.Offset);
                    SetUniformMat3(name, values);
                }
                case UniformType::Matrix4x4:
                {
                    const std::string& name = decl.Name;
                    glm::mat4& values = *(glm::mat4*)(uniformBuffer.GetBuffer() + decl.Offset);
                    SetUniformMat4(name, values);
                }
            }
        }
    }

    // ******************************************************************************
    // ******************************************************************************
    void OpenGLShader::SetVSMaterialUniformBuffer(Buffer buffer)
    {
        glUseProgram(m_RendererId);
        ResolveAndSetUniforms(m_VSMaterialUniformBuffer, buffer);
    }

    // ******************************************************************************
    // ******************************************************************************
    void OpenGLShader::SetPSMaterialUniformBuffer(Buffer buffer)
    {
        glUseProgram(m_RendererId);
        ResolveAndSetUniforms(m_PSMaterialUniformBuffer, buffer);
    }

    // ******************************************************************************
    // ******************************************************************************
    void OpenGLShader::UploadUniformStruct(OpenGLShaderUniformDeclaration* uniform, std::byte* buffer, uint32_t offset)
    {
        const ShaderStruct& s = uniform->GetShaderUniformStruct();
        const auto& fields = s.GetFields();
        for (size_t k = 0; k < fields.size(); k++)
        {
            OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
            ResolveAndSetUniformField(*field, buffer, offset);
            offset += field->m_Size;
        }
    }
    
    //-------------------------------- Uniforms --------------------------------
    void OpenGLShader::SetUniformInt1(const std::string& name, int value)
    {
        glUniform1i(GetUniformLocation(name), value);
    }
    
    void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
    {
        glUniform1iv(GetUniformLocation(name), count, values);
    }
    
    void OpenGLShader::SetUniformMat4(const std::string& name, const glm::mat4& value)
    {
        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void OpenGLShader::SetUniformMat3(const std::string& name, const glm::mat3& value)
    {
        glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }
    
    void OpenGLShader::SetUniformFloat1(const std::string& name, float value)
    {
        glUniform1f(GetUniformLocation(name), value);
    }
    
    void OpenGLShader::SetUniformFloat2(const std::string& name, const glm::vec2& value)
    {
        glUniform2f(GetUniformLocation(name), value.x, value.y);
    }
    
    void OpenGLShader::SetUniformFloat3(const std::string& name, const glm::vec3& value)
    {
        glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
    }
    
    void OpenGLShader::SetUniformFloat4(const std::string& name, const glm::vec4& value)
    {
        glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
    }

    void OpenGLShader::SetUniformInt1(uint32_t location, int value)
    {
        glUniform1i(location, value);
    }

    void OpenGLShader::SetIntArray(uint32_t location, int *values, uint32_t count)
    {
        glUniform1iv(location, count, values);
    }

    void OpenGLShader::SetUniformMat4(uint32_t location, const glm::mat4 &value)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void OpenGLShader::SetUniformMat3(uint32_t location, const glm::mat3 &value)
    {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void OpenGLShader::SetUniformMat4Array(uint32_t location, const glm::mat4& values, uint32_t count)
    {
        glUniformMatrix4fv(location, count, GL_FALSE, glm::value_ptr(values));
    }

    void OpenGLShader::SetUniformFloat1(uint32_t location, float value)
    {
        glUniform1f(location, value);
    }

    void OpenGLShader::SetUniformFloat2(uint32_t location, const glm::vec2 &value)
    {
        glUniform2f(location, value.x, value.y);
    }

    void OpenGLShader::SetUniformFloat3(uint32_t location, const glm::vec3 &value)
    {
        glUniform3f(location, value.x, value.y, value.z);
    }

    void OpenGLShader::SetUniformFloat4(uint32_t location, const glm::vec4 &value)
    {
        glUniform4f(location, value.x, value.y, value.z, value.w);
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
