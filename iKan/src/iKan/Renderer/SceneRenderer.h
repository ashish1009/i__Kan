// ******************************************************************************
// File         : SceneRenderer.h
// Project      : iKan : Renderer
// Description  : API Wrapper for rendering at Client
//
// Created on   : 28/04/21.
// ******************************************************************************

#pragma once

namespace iKan {
    
    class SceneRenderer
    {
    public:
        static void Init();
        static void Shutdown();
        
        static void SetShaader(const std::string &path);
    };
    
}
