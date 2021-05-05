// ******************************************************************************
// File         : KeyEvent.h
// Description  : Key Events to handle Key buttons
// Projectt     : iKan : Core : Event
//
// Created by Ashish on 17/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Core/Events/Event.h>
#include <iKan/Core/KeyCode.h>

namespace iKan {
    
    // ******************************************************************************
    // Key base event class
    // ******************************************************************************
    class KeyEvent : public Event
    {
    public:
        virtual ~KeyEvent() = default;
        KeyCode GetKeyCode() const { return m_Code; }
        
        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

    protected:
        KeyEvent(KeyCode code)
        : m_Code(code)
        {
            
        }
        
    protected:
        KeyCode m_Code;
    };

    // ******************************************************************************
    // Key Pressed Event class
    // ******************************************************************************
    class KeyPressedEvent : public KeyEvent
    {
    public:
        virtual ~KeyPressedEvent() = default;
        
        KeyPressedEvent(KeyCode keycode, int32_t repeatCount)
        : KeyEvent(keycode), m_RepeatCount(repeatCount)
        {
            
        }
        
        int32_t GetRepeatCount() const { return m_RepeatCount; }
        
        EVENT_CLASS_TYPE(KeyPressed);

    private:
        int32_t m_RepeatCount;
    };
    
    // ******************************************************************************
    // Key Released event class
    // ******************************************************************************
    class KeyReleasedEvent : public KeyEvent
    {
    public:
        virtual ~KeyReleasedEvent() = default;
        
        KeyReleasedEvent(KeyCode keycode)
        : KeyEvent(keycode)
        {
            
        }
        
        EVENT_CLASS_TYPE(KeyReleased);
    };
    
    // ******************************************************************************
    // Key typed Event class
    // ******************************************************************************
    class KeyTypedEvent : public KeyEvent
    {
    public:
        virtual ~KeyTypedEvent() = default;
        
        KeyTypedEvent(KeyCode keycode)
        : KeyEvent(keycode)
        {
            
        }
        
        EVENT_CLASS_TYPE(KeyTyped);
    };

}
