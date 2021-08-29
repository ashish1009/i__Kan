// ******************************************************************************
// File         : Utils.h
// Description  : Utils API for common functions
// Project      : iKan : Core
//
// Created by Ashish on 25/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

namespace iKan {

    class Utils
    {
    public:
        static std::string GetNameFromFilePath(const std::string& path);
        static std::string GetExtensionFromFilePath(const std::string& path);
        static std::string GetBlock(const char* str, const char** outPosition);
        static std::string GetStatement(const char* str, const char** outPosition);

        static const char* FindToken(const char* str, const std::string& token);

        static std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters);
        static std::vector<std::string> SplitString(const std::string& string, const char delimiter);
        static std::vector<std::string> Tokenize(const std::string& string);
        static std::vector<std::string> GetLines(const std::string& string);
        
        static bool StartsWith(const std::string& string, const std::string& start);
        
        static int32_t GetFirstSetBit(uint32_t value);

    };

}
