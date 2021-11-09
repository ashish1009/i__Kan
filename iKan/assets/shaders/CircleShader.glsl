// ******************************************************************************
// File         : BatchRenderer.glsl
// Project      : i-Kan : Assets
// Description  : Shader code for Batch renderer
//
// Created on   : 29/04/21.
// ******************************************************************************

// ******************************************************************************
// vertex Shader
// ******************************************************************************
#type vertex
#version 330 core

layout(location = 0) in vec3  a_Position;
layout(location = 1) in vec4  a_Color;
layout(location = 2) in vec2  a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in int   a_ObjectID;

uniform mat4 u_ViewProjection;

out VS_OUT
{
    vec4  Color;
    vec2  TexCoord;
    float TexIndex;
    float TilingFactor;
    float ObjectID;
} vs_out;

void main()
{
    vs_out.Color         = a_Color;
    vs_out.TexCoord      = a_TexCoord;
    vs_out.TexIndex      = a_TexIndex;
    vs_out.TilingFactor  = a_TilingFactor;
    vs_out.ObjectID      = float(a_ObjectID);
    
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

// ******************************************************************************
// Fragment Shader
// ******************************************************************************
#type fragment
#version 330 core
layout(location = 0) out vec4 o_Color;
layout(location = 1) out int  o_IDBuffer;

in VS_OUT
{
    vec4  Color;
    vec2  TexCoord;
    float TexIndex;
    float TilingFactor;
    float ObjectID;
} fs_in;

uniform sampler2D u_Textures[16];

void main()
{
    vec4 texColor = fs_in.Color;
    

    if(texColor.a < 0.1)
        discard;
    o_Color = texColor;
    o_IDBuffer = int(fs_in.ObjectID);
}
