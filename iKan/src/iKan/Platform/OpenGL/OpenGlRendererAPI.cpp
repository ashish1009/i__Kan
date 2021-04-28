// ******************************************************************************
// File         : OpenGLRendererAPI.cpp
// Description  : Implementation of Open GL Granphic Renderer API
// Project      : iKan : Platform
//
// Created on   : 16/04/21.
// ******************************************************************************

#include <iKan/Platform/OpenGL/OpenGlRendererAPI.h>
#include <glad/glad.h>

namespace iKan {
    
    // ******************************************************************************
    // Initialize the Open GL Renderer
    // ******************************************************************************
    void OpenGlRendererAPI::Init()
    {
        IK_CORE_INFO("Initialize the Open GL Renderer");

        glEnable(GL_MULTISAMPLE);

        Depth(true);
        Blend(true);

        auto& caps = RendererAPI::GetCapabilities();
        
        caps.Vendor   = (const char*)glGetString(GL_VENDOR);
        caps.Renderer = (const char*)glGetString(GL_RENDERER);
        caps.Version  = (const char*)glGetString(GL_VERSION);
    }
    
    // ******************************************************************************
    // Setuo the Background color each frame
    // ******************************************************************************
    void OpenGlRendererAPI::SetClearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }
    
    // ******************************************************************************
    // Clar all Open GL Buffers each frame
    // ******************************************************************************
    void OpenGlRendererAPI::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    // ******************************************************************************
    // Update the Depth field. Enable or Disablex
    // ******************************************************************************
    void OpenGlRendererAPI::Depth(bool state)
    {
        if (state)
        {
            IK_CORE_INFO("Eanble Open GL Depth Feild");
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            IK_CORE_INFO("Disable Open GL Depth Feild");
            glDisable(GL_DEPTH_TEST);
        }
    }
    
    // ******************************************************************************
    // Update the Blend field. Enable or Disablex
    // ******************************************************************************
    void OpenGlRendererAPI::Blend(bool state)
    {
        if (state)
        {
            IK_CORE_INFO("Eanble Open GL Blend Feild");
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            IK_CORE_INFO("Disable Open GL Blend Feild");
            glDisable(GL_BLEND);
        }
    }
    
    // ******************************************************************************
    // Set the Open GL Viie port
    // ******************************************************************************
    void OpenGlRendererAPI::SetViewPort(uint32_t widht, uint32_t height)
    {
        IK_CORE_INFO("Set the Open GL View port to {0} x {1}", widht, height);
        glViewport(0, 0, widht, height);
    }
    
    // ******************************************************************************
    // Draw the Triangle with vertex array
    // ******************************************************************************
    void OpenGlRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count)
    {
        vertexArray->Bind();
        uint32_t numIndices = count == 0 ? vertexArray->GetIndexBuffer()->GetCount() : count;
        
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        vertexArray->Unbind();
    }

    // ******************************************************************************
    // Draw the Triangle without vertex array
    // ******************************************************************************
    void OpenGlRendererAPI::DrawIndexed(uint32_t count)
    {
        glDrawArrays(GL_TRIANGLES, 0, count);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

}
