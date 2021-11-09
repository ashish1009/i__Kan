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
layout(location = 3) in float a_Thickness;
layout(location = 4) in float a_Fade;
layout(location = 5) in int   a_ObjectID;

uniform mat4 u_ViewProjection;

out VS_OUT
{
    vec4  Color;
    vec2  TexCoord;
    float Thickness;
    float Fade;
    float ObjectID;
} vs_out;

void main()
{
    vs_out.Color         = a_Color;
    vs_out.TexCoord      = a_TexCoord;
    vs_out.Thickness     = a_Thickness;
    vs_out.Fade          = a_Fade;
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
    float Thickness;
    float Fade;
    float ObjectID;
} fs_in;

void main()
{
    float distance = 1.0 - length(fs_in.TexCoord);
    float color = smoothstep(0.0, fs_in.Fade, distance);
    color *= smoothstep(fs_in.Thickness + fs_in.Fade, fs_in.Thickness, distance);
    
    o_Color = fs_in.Color;
    o_Color.a *= color;
    
    o_IDBuffer = int(fs_in.ObjectID);
}


////    float distance = 1.0 - length(fs_in.LocalPos);
////    vec3 color = vec3(smoothstep(0.0, fs_in.Fade, distance));
////    color *= vec3(smoothstep(fs_in.Thickness + fs_in.Fade, fs_in.Thickness, distance));
//
//    o_Color = fs_in.Color;
////    o_Color.rgb *= color;
//
//    o_IDBuffer = int(fs_in.ObjectID);
