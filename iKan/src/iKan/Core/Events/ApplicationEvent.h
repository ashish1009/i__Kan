// ******************************************************************************
// File         : ApplicationEvent.h
// Description  : Application Events to handle window application
// Projectt     : iKan : Core
//
// Created on   : 17/04/21.
// ******************************************************************************

#pragma once

#include <iKan/Core/Events/Event.h>

namespace iKan {
    
    // ******************************************************************************
    // Window Resize event
    // ******************************************************************************
    class WindowResizeEvent : public Event
    {
    public:
        virtual ~WindowResizeEvent() = default;
        
        WindowResizeEvent(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height)
        {
            
        }
        
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        
        EVENT_CLASS_TYPE(WindowResize);
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
        uint32_t m_Width, m_Height;
    };

    // ******************************************************************************
    // Window Close event
    // ******************************************************************************
    class WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent() = default;
        virtual ~WindowCloseEvent() = default;

        EVENT_CLASS_TYPE(WindowClose);
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };
    
    // ******************************************************************************
    // Window Close event
    // ******************************************************************************
    class WindowFocusEvent : public Event
    {
    public:
        virtual ~WindowFocusEvent() = default;
        WindowFocusEvent(bool iconified)
        : m_Iconified(iconified)
        {
            
        }
        
        bool GetIconified() const { return m_Iconified; }
        
        EVENT_CLASS_TYPE(WindowFocus);
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
        // `GLFW_TRUE` if the window was iconified, or `GLFW_FALSE` if it was restored.
        // GLFW_TRUE is aliaced as true and GLFW_FALSE as false
        bool m_Iconified = false;
    };
    
    // ******************************************************************************
    // Window Close event
    // ******************************************************************************
    class WindowMovedEvent : public Event
    {
    public:
        WindowMovedEvent() = default;
        virtual ~WindowMovedEvent() = default;
    
        EVENT_CLASS_TYPE(WindowMoved);
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };
    

}
