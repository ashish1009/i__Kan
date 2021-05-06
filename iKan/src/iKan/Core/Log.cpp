// ******************************************************************************
// File         : Log.cpp
// Description  : Implementation of Log wrapper for sp log
// Project      : iKan : Core
//
// Created by Ashish on 13/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace iKan {
    
    Ref<spdlog::logger> Log::s_CoreLogger;
    Ref<spdlog::logger> Log::s_ClientLogger;
    
    // ******************************************************************************
    // Initialize the logger
    // ******************************************************************************
    void Log::Init()
    {
        std::vector<spdlog::sink_ptr> logSinks;
        logSinks.emplace_back(CreateRef<spdlog::sinks::stdout_color_sink_mt>());
        logSinks.emplace_back(CreateRef<spdlog::sinks::basic_file_sink_mt>("../../../Logs/iKan.log", true));

        logSinks[0]->set_pattern("[%T:%e:%f] [%-8l] [%-4n] : %v");
        logSinks[1]->set_pattern("[%T:%e:%f] [%-8l] [%-4n] : %v");

        s_CoreLogger = CreateRef<spdlog::logger>("iKAN", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_CoreLogger);
        
        s_CoreLogger->set_level(spdlog::level::trace);
        s_CoreLogger->flush_on(spdlog::level::trace);
        
        s_ClientLogger = CreateRef<spdlog::logger>("APP", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_ClientLogger);
        
        s_ClientLogger->set_level(spdlog::level::trace);
        s_ClientLogger->flush_on(spdlog::level::trace);
    }
    
}
