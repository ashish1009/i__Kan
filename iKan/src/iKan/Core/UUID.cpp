// ******************************************************************************
// File         : UUID.h
// Description  : Generates the Universal unique ID integer
// Project      : iKan : Core
//
// Created by Ashish on 01/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include <iKan/Core/UUID.h>
#include <random>

namespace iKan {
    
    static std::random_device s_RandomDevice;           //Will be used to obtain a seed for the random number engine
    static std::mt19937_64 s_Enginge(s_RandomDevice()); //Standard mersenne_twister_engine seeded with rd()
    static std::uniform_int_distribution<uint64_t> s_UniformDistribution;
    
    UUID::UUID()
    : m_UUID(s_UniformDistribution(s_Enginge))
    {
    }
    
    UUID::UUID(uint64_t uuid)
    : m_UUID(uuid)
    {
    }
    
    UUID::UUID(const UUID& other)
    : m_UUID(other.m_UUID)
    {
    }
    
}
