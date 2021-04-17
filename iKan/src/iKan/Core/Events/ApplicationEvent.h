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
        WindowResizeEvent() = default;
        virtual ~WindowResizeEvent() = default;
        
        WindowResizeEvent(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height)
        {
            
        }
        
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        
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
        
    private:
    };

}
