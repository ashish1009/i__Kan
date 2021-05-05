// ******************************************************************************
// File         : UUID.h
// Description  : Generates the Universal unique ID integer
// Project      : iKan : Core
//
// Created by Ashish on 01/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

namespace iKan {

    // ******************************************************************************
    // "UUID" (universally unique identifier) or GUID is (usually) a 128-bit integer
    // used to "uniquely" identify information. In iKan Engine, even though we use
    // the term GUID and UUID, at the moment we're simply using a randomly
    // generated 64-bit integer, as the possibility of a clash is low enough for now.
    // This may change in the future.
    // ******************************************************************************
    class UUID
    {
    public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID& other);
        
        operator uint64_t () { return m_UUID; }
        operator const uint64_t () const { return m_UUID; }
    private:
        uint64_t m_UUID;
    };
    
}

namespace std {
    
    // ******************************************************************************
    // Userdefined Hash ID
    // ******************************************************************************
    template <>
    struct hash<iKan::UUID>
    {
        std::size_t operator()(const iKan::UUID& uuid) const
        {
            return hash<uint64_t>()((uint64_t)uuid);
        }
    };
}
