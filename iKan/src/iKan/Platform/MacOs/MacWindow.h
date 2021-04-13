// ******************************************************************************
// File         : MacWindow.h
// Description  : Mac Window Class
// Project      : iKan : Platform
//
// Created on   : 13/04/21.
// ******************************************************************************

#pragma once

#include <iKan/Core/Window.h>

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
            
            Data(const std::string& title = "Mac Window", uint32_t width = 800, uint32_t height = 400)
            : Title(title), Width(width), Height(height)
            {
                
            }
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
        
        virtual void* GetNativeWindow() override { return (void*)nullptr; }
        
    private:
        void Init(const Window::Property& prop);
        
    private:
        Data m_Data;
    };
    
}
