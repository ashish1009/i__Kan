// ******************************************************************************
// File         : RenderCommamndQueue.h
// Project      : iKan : Renderer
// Description  : Stores the render command and excute them one by one
//
// Created by Ashish on 31/07/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

namespace iKan {

    class RenderCommandQueue
    {
    public:
        typedef void(*RenderCommandFn)(void*);

        RenderCommandQueue();
        ~RenderCommandQueue();

        void* Allocate(RenderCommandFn func, uint32_t size);
        void Execute();

    private:
        uint8_t* m_CommandBuffer;
        uint8_t* m_CommandBufferPtr;
        uint32_t m_CommandCount = 0;
    };

}
