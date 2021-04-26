// ******************************************************************************
// File         : Buffers.h
// Project      : iKan : Renderer
// Description  : Interface for Vertex and Index Buffer class
//
// Created on   : 26/04/21.
// ******************************************************************************

#pragma once

namespace iKan {
    
    // ******************************************************************************
    // Data types of Shader we are going to use
    // ******************************************************************************
    enum class ShaderDataType
    {
        NoType = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
    };
    
    // ******************************************************************************
    // Get the number of bytes in Shader data types. Size
    // ******************************************************************************
    static uint32_t ShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::NoType : return 0;         break;
            case ShaderDataType::Float  : return 4;         break;
            case ShaderDataType::Float2 : return 4 * 2;     break;
            case ShaderDataType::Float3 : return 4 * 3;     break;
            case ShaderDataType::Float4 : return 4 * 4;     break;
            case ShaderDataType::Mat3   : return 4 * 3 * 3; break;
            case ShaderDataType::Mat4   : return 4 * 4 * 5; break;
            case ShaderDataType::Int    : return 4;         break;
            case ShaderDataType::Int2   : return 4 * 2;     break;
            case ShaderDataType::Int3   : return 4 * 3;     break;
            case ShaderDataType::Int4   : return 4 * 4;     break;
            case ShaderDataType::Bool   : return 1;         break;
        }
        IK_CORE_ASSERT(false, "Invalid Type");
        return 0;
    }
        
    // ******************************************************************************
    // Get the number of same data type elements. Count
    // ******************************************************************************
    static uint32_t GetElementCount(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::NoType : return 0;
            case ShaderDataType::Float :  return 1;
            case ShaderDataType::Float2 : return 2;
            case ShaderDataType::Float3 : return 3;
            case ShaderDataType::Float4 : return 4;
            case ShaderDataType::Mat3:    return 3;
            case ShaderDataType::Mat4:    return 4;
            case ShaderDataType::Int :    return 1;
            case ShaderDataType::Int2 :   return 2;
            case ShaderDataType::Int3 :   return 3;
            case ShaderDataType::Int4 :   return 4;
            case ShaderDataType::Bool :   return 1;
        }
        IK_CORE_ASSERT(false, "Unkown Shader datatype!! ");
    }
    
    // ******************************************************************************
    // Buffer Elements. Stores all the property of a element in a buffer
    // ******************************************************************************
    struct BufferElement
    {
        std::string     Name;
        ShaderDataType  Type;
        uint32_t        Size;
        uint32_t        Count;
        size_t          Offset;
        bool            Normalized;
        
        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
        : Name(name), Type(type), Size(ShaderDataTypeSize(type)), Count(GetElementCount(type)), Offset(0), Normalized(normalized) {}
    };
    
    // ******************************************************************************
    // Stores the layput of buffer elements in a vector
    // ******************************************************************************
    class BufferLayout
    {
    public:
        BufferLayout() = default;
        
        BufferLayout(const std::initializer_list<BufferElement>& elements)
        : m_Elements(elements)
        {
            CalculateOffsetAndStride();
        }
        
        const std::vector<BufferElement> GetElements() const { return m_Elements; }
        
        uint32_t GetStride() const { return m_Stride; }
        
        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
        
    private:
        // ******************************************************************************
        // Computes the offset and stride of buffer elements
        // ******************************************************************************
        void CalculateOffsetAndStride()
        {
            size_t offset = 0;
            m_Stride      = 0;
            for (auto& element : m_Elements)
            {
                element.Offset = offset;
                offset        += element.Size;
                m_Stride      += element.Size;
            }
        }
        
    private:
        std::vector<BufferElement> m_Elements;
        uint32_t                   m_Stride = 0;
    };
    
    // ******************************************************************************
    // Interface of Vertex Buffer class
    // ******************************************************************************
    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer() = default;
        
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetData(const void* data, uint32_t size) = 0;

        virtual void AddLayout(const BufferLayout& layout) = 0;
        virtual const BufferLayout& GetLayout() const = 0;
        
        static Ref<VertexBuffer> Create(uint32_t size);
        static Ref<VertexBuffer> Create(uint32_t size, float* data);
    };
    
    // ******************************************************************************
    // Interface of Index Buffer class
    // ******************************************************************************
    class IndexBuffer
    {
    public:
        virtual ~IndexBuffer() = default;
        
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        
        virtual uint32_t GetCount() const = 0;
        
        static Ref<IndexBuffer> Create(uint32_t count, uint32_t* data);
    };
    
}
