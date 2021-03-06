// ******************************************************************************
// File         : MacWindow.h
// Description  : Mac Window Class
// Project      : iKan : Platform
//
// Created by Ashish on 13/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Core/Window.h>
#include <iKan/Renderer/GraphicsContext.h>

namespace iKan {
    
    // ******************************************************************************
    // Class to store MAC window handle. Creates the window and update each frame
    // ******************************************************************************
    class MacWindow : public Window
    {
    private:
        // ******************************************************************************
        // Wrapper to store MAC Window data
        // ******************************************************************************
        struct Data
        {
            std::string Title = "Mac Window";
            uint32_t    Width = 800, Height = 400;
            bool        VSync = true;
            
            EventCallbackFn EventCallback;

            Data(const std::string& title, uint32_t width, uint32_t height, bool vSync, EventCallbackFn EventCallback)
            : Title(title), Width(width), Height(height), VSync(vSync)
            {
                
            }
            
            Data() = default;
            ~Data() = default;
        };
        
    public:
        MacWindow(const Window::Property& prop);
        virtual ~MacWindow();
        
        virtual void Update() override;
        virtual void Shutdown() override;

        virtual uint32_t GetWidth()  const override { return m_Data.Width; }
        virtual uint32_t GetHeight() const override { return m_Data.Height; }

        virtual const std::string& GetTitle() const override { return m_Data.Title; }
        
        virtual void* GetNativeWindow() override { return (void*)m_Window; }
        virtual void SetVSync(bool enabled) override;
        virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; };

        virtual const Timestep& GetTime() const override { return m_Timestep; }

        virtual bool IsVSync() const override;
        
    private:
        void Init();
        void SetCallBacks();
        
    private:
        Data        m_Data;
        GLFWwindow* m_Window;
        Timestep    m_Timestep;
        GLFWcursor* m_ImGuiMouseCursors[ImGuiMouseCursor_COUNT] = { 0 };
        Scope<GraphicsContext> m_Context;
        float m_LastFrame = 0.0f;
    };
    
}
