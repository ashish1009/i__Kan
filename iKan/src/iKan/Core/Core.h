// ******************************************************************************
// File         : Core.h
// Description  : Core functionalities or API to be defined here
// Project      : iKan : Core
//
// Created by Ashish on 13/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

// ******************************************************************************
// Assertion API defined for iKan Engine
// ******************************************************************************
#ifdef IK_ENABLE_ASSERT

// x is conditional statement. Assert if x is false
#define IK_CORE_ASSERT(x, ...)                                  \
if (!x)                                                         \
{                                                               \
    IK_CORE_CRITICAL("Assertion Failed {0}", __VA_ARGS__);      \
    assert(x); \
}

#define IK_ASSERT(x, ...)                                       \
if (!x)                                                         \
{                                                               \
    IK_CRITICAL("Assertion Failed {0}", __VA_ARGS__);           \
    assert(x); \
}

#else

#define IK_CORE_ASSERT(x, ...)
#define IK_ASSERT(x, ...)

#endif

// API to get bit positioned value
#define BIT(x) (1 << x)

// API to bind class member function to std function pointer
#define IK_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace iKan {

    // ******************************************************************************
    // Smart pointer iKan API
    // ******************************************************************************
    template<typename T>
    using Scope = std::unique_ptr<T>;
    
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;
    
    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

}
