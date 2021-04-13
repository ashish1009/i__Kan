// ******************************************************************************
// File         : Log.cpp
// Description  : Implementation of Log wrapper for sp log
// Project      : iKan
//
//  Created by Ashish . on 13/04/21.
// ******************************************************************************

#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace iKan {
    
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
    
    // ******************************************************************************
    // Initialize the logger
    // ******************************************************************************
    void Log::Init()
    {
        std::vector<spdlog::sink_ptr> logSinks;
        logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("../../Logs/iKan.log", true));
        
        logSinks[0]->set_pattern("%^[%T] [%-8l] %n: %v%$");
        logSinks[1]->set_pattern("[%T] [%-8l] %n: %v");
        
        s_CoreLogger = std::make_shared<spdlog::logger>("iKAN", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_CoreLogger);
        
        s_CoreLogger->set_level(spdlog::level::trace);
        s_CoreLogger->flush_on(spdlog::level::trace);
        
        s_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_ClientLogger);
        
        s_ClientLogger->set_level(spdlog::level::trace);
        s_ClientLogger->flush_on(spdlog::level::trace);
    }
    
}
