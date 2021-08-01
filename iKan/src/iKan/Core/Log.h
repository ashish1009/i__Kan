// ******************************************************************************
// File         : Log.h
// Description  : Wrapper file for spd logger
// Project      : iKan : Core
//
// Created by Ashish on 13/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <spdlog/spdlog.h>

namespace iKan {
    
    // ******************************************************************************
    // Wrapper to Use spd logger
    // ******************************************************************************
    class Log
    {
    public:
        static void Init();
        
        static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
        
    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
    
}

#ifdef IK_ENABLE_LOG
/* Core log macros */
#define IK_CORE_TRACE(...)    ::iKan::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define IK_CORE_INFO(...)     ::iKan::Log::GetCoreLogger()->info(__VA_ARGS__)
#define IK_CORE_WARN(...)     ::iKan::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define IK_CORE_ERROR(...)    ::iKan::Log::GetCoreLogger()->error(__VA_ARGS__)
#define IK_CORE_CRITICAL(...) ::iKan::Log::GetCoreLogger()->critical(__VA_ARGS__)

/* Client log macros */
#define IK_TRACE(...)          ::iKan::Log::GetClientLogger()->trace(__VA_ARGS__)
#define IK_INFO(...)           ::iKan::Log::GetClientLogger()->info(__VA_ARGS__)
#define IK_WARN(...)           ::iKan::Log::GetClientLogger()->warn(__VA_ARGS__)
#define IK_ERROR(...)          ::iKan::Log::GetClientLogger()->error(__VA_ARGS__)
#define IK_CRITICAL(...)       ::iKan::Log::GetClientLogger()->critical(__VA_ARGS__)

#else

#define IK_CORE_TRACE(...)
#define IK_CORE_INFO(...)
#define IK_CORE_WARN(...)
#define IK_CORE_ERROR(...)
#define IK_CORE_CRITICAL(...)

/* Client log macros */
#define IK_TRACE(...)
#define IK_INFO(...)
#define IK_WARN(...)
#define IK_ERROR(...)
#define IK_CRITICAL(...)

#endif
