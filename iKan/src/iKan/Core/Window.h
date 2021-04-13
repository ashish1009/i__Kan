// ******************************************************************************
// File         : Window.h
// Description  : Interface for Window Class
// Project      : iKan : Core
//
// Created on   : 13/04/21.
// ******************************************************************************

#pragma once

namespace iKan {
    
    // ******************************************************************************
    // Interface for Creating window. Implementation is Platform dependent
    // ******************************************************************************
    class Window
    {
    public:
        // ******************************************************************************
        // Wrapper to store the property of a Window
        // ******************************************************************************
        struct Property
        {
            static const uint32_t DefaultWidth  = 1280;
            static const uint32_t DefaultHeight = 720;
            
            std::string Title = "iKan";
            uint32_t Width = DefaultWidth, Height = DefaultHeight;
            
            Property(const std::string& title = "iKan", uint32_t width = DefaultWidth, uint32_t height = DefaultHeight)
            : Title(title), Width(width), Height(height)
            {
                
            }
            
            ~Property() = default;
        };
        
    public:
        virtual ~Window() = default;
        
        virtual void Update() = 0;
        virtual void Shutdown() = 0;
        
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        virtual const std::string& GetTitle() const = 0;

        virtual void* GetNativeWindow() = 0;
        
        static Scope<Window> Create(const Window::Property& props = Window::Property());
    };
    
}
