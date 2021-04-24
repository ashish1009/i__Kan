// ******************************************************************************
// File         : MouseEvent.h
// Description  : Mouse Events to handle Mouse movements and button
// Projectt     : iKan : Core
//
// Created on   : 17/04/21.
// ******************************************************************************

#pragma once

#include <iKan/Core/Events/Event.h>
#include <iKan/Core/MouseCode.h>

namespace iKan {
    
    // ******************************************************************************
    // Mouse base Event class
    // ******************************************************************************
    class MouseMovedEvent : public Event
    {
    public:
        virtual ~MouseMovedEvent() = default;
        
        MouseMovedEvent(float x, float y)
        : m_MouseX(x), m_MouseY(y)
        {
            
        }
        
        float GetX() const { return m_MouseX; }
        float GetY() const { return m_MouseY; }
        
        EVENT_CLASS_TYPE(MouseMoved);
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    private:
        float m_MouseX, m_MouseY;
    };
    
    // ******************************************************************************
    // Mouse scrol event
    // ******************************************************************************
    class MouseScrolledEvent : public Event
    {
    public:
        virtual ~MouseScrolledEvent() = default;
        
        MouseScrolledEvent(float xOffset, float yOffset)
        : m_XOffset(xOffset), m_YOffset(yOffset)
        {
            
        }
        
        float GetXOffset() const { return m_XOffset; }
        float GetYOffset() const { return m_YOffset; }
        
        EVENT_CLASS_TYPE(MouseScrolled);
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    private:
        float m_XOffset, m_YOffset;
    };
    
    // ******************************************************************************
    // Mouse button base calss
    // ******************************************************************************
    class MouseButtonEvent : public Event
    {
    public:
        virtual ~MouseButtonEvent() = default;
        MouseCode GetMouseButton() const { return m_Button; }
        
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)

    protected:
        MouseButtonEvent(MouseCode button)
        : m_Button(button)
        {
            
        }
        
        MouseCode m_Button;
    };
    
    // ******************************************************************************
    // Mouse button pressed
    // ******************************************************************************
    class MouseButtonPressedEvent : public MouseButtonEvent
    {
    public:
        virtual ~MouseButtonPressedEvent() = default;
        
        MouseButtonPressedEvent(MouseCode button)
        : MouseButtonEvent(button)
        {
            
        }
        
        EVENT_CLASS_TYPE(MouseButtonPressed);
    };
    
    // ******************************************************************************
    // Mouse button released
    // ******************************************************************************
    class MouseButtonReleasedEvent : public MouseButtonEvent
    {
    public:
        virtual ~MouseButtonReleasedEvent() = default;
        
        MouseButtonReleasedEvent(MouseCode button)
        : MouseButtonEvent(button)
        {
            
        }
        
        EVENT_CLASS_TYPE(MouseButtonReleased);
    };

}
