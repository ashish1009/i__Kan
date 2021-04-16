// ******************************************************************************
// File         : Renderer.h
// Description  : Renderer API Visible to Core and client
// Project      : iKan : Renderer
//
// Created on   : 16/04/21.
// ******************************************************************************

#pragma once

namespace iKan {
    
    class Renderer
    {
    public:
        enum class API { None = 0, OpenGL = 1 };

    public:
        static void SetAPI(API api) { s_API = api; }
        static API GetAPI() { return s_API; }
        
    private:
        static API s_API;
    };
    
}
