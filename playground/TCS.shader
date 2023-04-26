// tessellation control shader
#version 410 core

// specify number of control points per patch output
// this value controls the size of the input and output arrays
layout (vertices=3) out;

in vec2 UV_vt[];
in vec3 Position_worldspace_vt[];
in vec3 EyeDirection_cameraspace_vt[];
in vec3 LightDirection_cameraspace_vt[];
in vec3 LightDirection_tangentspace_vt[];
in vec3 EyeDirection_tangentspace_vt[];

// varying input from vertex shader
in vec2 TexCoord[];
// varying output to evaluation shader
out vec2 TextureCoord[];

void main()
{
    // ----------------------------------------------------------------------
    // pass attributes through
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];

    // ----------------------------------------------------------------------
    // invocation zero controls tessellation levels for the entire patch
    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = 4;
        gl_TessLevelOuter[1] = 4;
        gl_TessLevelOuter[2] = 4;
        //gl_TessLevelOuter[3] = 16;

        gl_TessLevelInner[0] = 4;
        gl_TessLevelInner[1] = 4;
    }
}
    
