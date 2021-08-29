// ******************************************************************************
// File         : Utils.cpp
// Description  : Utils API for common functions
// Project      : iKan : Core
//
// Created by Ashish on 25/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "Utils.h"

namespace iKan {

    // ******************************************************************************
    // Extract file Name from path
    // ******************************************************************************
    std::string Utils::GetNameFromFilePath(const std::string& path)
    {
        auto lastSlash = path.find_last_of("/\\");
        lastSlash      = lastSlash == std::string::npos ? 0 : lastSlash + 1;

        auto lastDot = path.find_last_of('.');
        auto count   = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;

        return path.substr(lastSlash, count);
    }

    // ******************************************************************************
    // Extract file extention from path
    // ******************************************************************************
    std::string Utils::GetExtensionFromFilePath(const std::string& path)
    {
        auto lastDot = path.find_last_of('.') + 1;
        auto count   = lastDot - std::string::npos;

        return path.substr(lastDot, count);
    }

    // ******************************************************************************
    // Return the pointer of first occurance of token, token should be alone
    // (space or nothing on left and right side)
    // NOTE: it is assumed that struct and uniform in the shader code should be alone
    // eg. struct { } or uniform ...
    // str   : base string
    // token : to be searched
    // ******************************************************************************
    const char* Utils::FindToken(const char* str, const std::string& token)
    {
        const char* t = str;
        while ((t = strstr(t, token.c_str())))
        {
            // Check if left character of token is empty
            // TODO: '/' might chnage if wont work for some scenario, using '/' as indicator
            // of first character of line
            bool left = str == t || isspace(t[-1]) || t[-1] == '/';

            // Check if right character of token is empty
            bool right = !t[token.size()] || isspace(t[token.size()]);

            if (left && right)
                return t;

            t += token.size();
        }
        return nullptr;
    }

    // ******************************************************************************
    // Extract the struct code from the shader string "struct { ... }"
    // return the pointer to 's' in "struct { ... }"
    // src         : token
    // outPosition : string pointer of the shader code, that will move to some offset
    //               offset is the "}" of the strcut
    // ******************************************************************************
    std::string Utils::GetBlock(const char* str, const char** outPosition)
    {
        const char* end = strstr(str, "}");
        if (!end)
            return str;

        // offset the shader string to the end of current strcut
        if (outPosition)
            *outPosition = end;

        uint32_t length = (uint32_t)(end - str + 1);
        return std::string(str, length);
    }

    // ******************************************************************************
    // Extract the Code statement before ";"
    // ******************************************************************************
    std::string Utils::GetStatement(const char* str, const char** outPosition)
    {
        const char* end = strstr(str, ";");
        if (!end)
            return str;

        if (outPosition)
            *outPosition = end;
        uint32_t length = (uint32_t)(end - str + 1);
        return std::string(str, length);
    }

    // ******************************************************************************
    // Break the string. Break points are stored in delimiter, any character in the
    // delimeter will be cosnider as break points
    // ******************************************************************************
    std::vector<std::string> Utils::SplitString(const std::string& string, const std::string& delimiters)
    {
        size_t start = 0;
        size_t end = string.find_first_of(delimiters);

        std::vector<std::string> result;

        while (end <= std::string::npos)
        {
            std::string token = string.substr(start, end - start);
            if (!token.empty())
                result.push_back(token);

            if (end == std::string::npos)
                break;

            start = end + 1;
            end = string.find_first_of(delimiters, start);
        }

        return result;
    }

    std::vector<std::string> Utils::SplitString(const std::string& string, const char delimiter)
    {
        return SplitString(string, std::string(1, delimiter));
    }

    std::vector<std::string> Utils::Tokenize(const std::string& string)
    {
        return SplitString(string, " \t\n");
    }

    std::vector<std::string> Utils::GetLines(const std::string& string)
    {
        return SplitString(string, "\n");
    }

    // ******************************************************************************
    // Is string start with start
    // ******************************************************************************
    bool Utils::StartsWith(const std::string& string, const std::string& start)
    {
        return string.find(start) == 0;
    }

    // ******************************************************************************
    // get the fist set bit returns -1 if value is 0
    // 1 means Most LSB and So on...
    // ******************************************************************************
    int32_t Utils::GetFirstSetBit(uint32_t value)
    {
        if (!value)
            return 0;
        
        for (int32_t pos = 0; value >> pos; pos++)
            if (value & BIT(pos))
                return pos + 1;
        
        return -1;
    }

}
