// ******************************************************************************
// File         : TimeStep.h
// Project      : iKan
// Description  : Class to store the time in seconds, millisecons and microSeconds
//
// Created on   : 24/04/21.
// ******************************************************************************

#pragma once

namespace iKan {
    
    // ******************************************************************************
    // Class to store the time.
    // Note: we have to update the time periodically. this class will store the time,
    // time can not be updated in this calss, we need to update time each frame
    // manually
    // ******************************************************************************
    class Timestep
    {
    public:
        Timestep(float time = 0.0f)
        : m_Time(time)
        {
            
        }
        
        ~Timestep() = default;
        
        operator float() const { return m_Time; }
        
        float GetSeconds() const { return m_Time; }
        float GetMilliSeconds() const { return m_Time * 1000.0f; }
        float GetMicroSeconds() const { return m_Time * 1000000.0f; }
      
    private:
        float m_Time = 0.0f;
    };
    
}
