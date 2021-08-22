// ******************************************************************************
// File         : Event.h
// Description  : Base event class and Dispatcher
// Projectt     : iKan : Core : Event
//
// Created by Ashish on 17/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

namespace iKan {
    
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }     \
virtual EventType GetEventType() const override { return GetStaticType(); }                     \
virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int32_t GetCategoryFlags() const override { return category; }

    // ******************************************************************************
    // event categories
    // ******************************************************************************
    enum class EventType
    {
        // Invalid type
        None = 0,
        
        // Application Events type
        WindowClose, WindowResize, WindowFocus, WindowMoved,
        
        // Keyboard Events type
        KeyPressed, KeyReleased, KeyTyped,
        
        // Mouse Event type
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };
    
    // ******************************************************************************
    // event categories
    // ******************************************************************************
    enum EventCategory
    {
        None = 0,
        EventCategoryApplication    = BIT(0),
        EventCategoryInput          = BIT(1),
        EventCategoryKeyboard       = BIT(2),
        EventCategoryMouse          = BIT(3),
        EventCategoryMouseButton    = BIT(4)
    };
    
    // ******************************************************************************
    // Base class for all events, there must be a base class which can be sent via
    // applciatin Event arugment (common for all events)
    // ******************************************************************************
    class Event
    {
    public:
        virtual ~Event() = default;
        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        virtual int GetCategoryFlags() const = 0;

        bool IsInCategory(EventCategory category)
        {
            return GetCategoryFlags() & category;
        }
        
    public:
        bool Handled = false;
    };
    
    // ******************************************************************************
    // Class to dispatch event dynamically. Event type can be checked with current
    // event and perform the functionality
    // ******************************************************************************
    class EventDispatcher
    {
    public:
        EventDispatcher(Event& event)
        : m_Event(event) {}

        // F will be deduced by the compiler
        template<typename T, typename F>
        bool Dispatch(const F& func)
        {
            if (m_Event.GetEventType() == T::GetStaticType())
            {
                m_Event.Handled |= func(static_cast<T&>(m_Event));
                return true;
            }

            return false;
        }

    private:
        Event& m_Event;
    };
    
}
