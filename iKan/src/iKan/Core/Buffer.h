// ******************************************************************************
// File         : Buffer.h
// Description  : Generates the Universal unique ID integer
// Project      : iKan : Core
//
// Created by Ashish on 06/07/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Core/Core.h>

namespace iKan {

    class Buffer
    {
    public:
        std::byte* Data;
        uint32_t Size;

    public:
        // ******************************************************************************
        // Default COnstructor
        // ******************************************************************************
        Buffer()
        : Data(nullptr), Size(0)
        {

        }

        // ******************************************************************************
        // Parameter constructor
        // ******************************************************************************
        Buffer(std::byte* data, uint32_t size)
        : Data(data), Size(size)
        {

        }

        // ******************************************************************************
        // Copy the data and return the constructed buffer
        // ******************************************************************************
        static Buffer Copy(void* data, uint32_t size)
        {
            Buffer buffer;
            buffer.Allocate(size);
            memcpy(buffer.Data, data, size);
            return buffer;
        }

        // ******************************************************************************
        // Allocate the buffer of size
        // ******************************************************************************
        void Allocate(uint32_t size)
        {
            delete[] Data;
            Data = nullptr;

            if (size == 0)
                return;

            Data = new std::byte[size];
            Size = size;
        }

        // ******************************************************************************
        // Set the allocated data to zero
        // ******************************************************************************
        void ZeroInitialize()
        {
            if (Data)
                memset(Data, 0, Size);
        }

        // ******************************************************************************
        // Read the data at offset of type T
        // ******************************************************************************
        template<typename T>
        T& Read(uint32_t offset = 0)
        {
            return *(T*)(Data + offset);
        }

        // ******************************************************************************
        // Write the data to memory
        // ******************************************************************************
        void Write(void* data, uint32_t size, uint32_t offset = 0)
        {
            IK_CORE_ASSERT(offset + size <= Size, "Buffer overflow!");
            memcpy(Data + offset, data, size);
        }

        // ******************************************************************************
        // bool operator
        // ******************************************************************************
        operator bool() const
        {
            return Data;
        }

        // ******************************************************************************
        // [] operator
        // ******************************************************************************
        byte& operator[](int index)
        {
            return Data[index];
        }

        // ******************************************************************************
        // [] operator
        // ******************************************************************************
        byte operator[](int index) const
        {
            return Data[index];
        }

        // ******************************************************************************
        // Read the data as T
        // ******************************************************************************
        template<typename T>
        T* As()
        {
            return (T*)Data;
        }

        // ******************************************************************************
        // Get the size
        // ******************************************************************************
        uint32_t GetSize() const { return Size; }
    };

}
