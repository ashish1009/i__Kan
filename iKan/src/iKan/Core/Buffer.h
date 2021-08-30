// ******************************************************************************
// File         : Buffer.cpp
// Project      : iKan : Core
// Description  : Implementation of iKan Buffer
//
// Created by Ashish on 31/07/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Core/Log.h>

namespace iKan {

    struct Buffer
    {
        std::byte* Data;
        uint32_t Size;

        Buffer()
        : Data(nullptr), Size(0) {}

        Buffer(std::byte* data, uint32_t size)
        : Data(data), Size(size) {}

        static Buffer Copy(void* data, uint32_t size)
        {
            Buffer buffer;
            buffer.Allocate(size);
            memcpy(buffer.Data, data, size);
            return buffer;
        }
        
        void Clear()
        {
            if (Data)
                delete[] Data;
            
            Size = 0;
            Data = nullptr;
        }

        void Allocate(uint32_t size)
        {
            delete[] Data;
            Data = nullptr;

            if (size == 0)
                return;

            Data = new std::byte[size];
            Size = size;
        }

        void ZeroInitialize()
        {
            if (Data)
                memset(Data, 0, Size);
        }

        template<typename T>
        T& Read(uint32_t offset = 0) { return *(T*)(Data + offset);}

        void Write(void* data, uint32_t size, uint32_t offset = 0)
        {
            IK_CORE_ASSERT(offset + size <= Size, "Buffer overflow!");
            memcpy(Data + offset, data, size);
        }

        operator bool() const { return Data; }

        std::byte& operator[](int index) { return Data[index]; }
        std::byte operator[](int index) const { return Data[index]; }

        template<typename T>
        T* As() { return (T*)Data; }

        uint32_t GetSize() const { return Size; }
    };

}
