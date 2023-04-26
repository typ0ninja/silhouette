
// tessellation control shader
#version 410 core

// Output data ; will be interpolated for each fragment.
// varying input from vertex shader
in vec2 UV_vt[];
in vec3 Position_worldspace_vt[];
in vec3 Normal_cameraspace_vt[];
in vec3 EyeDirection_cameraspace_vt[];
in vec3 LightDirection_cameraspace_vt[];

// Output data ; will be interpolated for each fragment.
out vec2 UV_tcs[];
patch out vec2 initialUV;
patch out vec3 Position_worldspace_tcs;
patch out vec3 Normal_cameraspace_tcs;
patch out vec3 EyeDirection_cameraspace_tcs;
patch out vec3 LightDirection_cameraspace_tcs;

// specify number of control points per patch output
// this value controls the size of the input and output arrays
layout (vertices=3) out;


// varying output to evaluation shader
//out vec2 TextureCoord[];

void main()
{
    // ----------------------------------------------------------------------
    // pass attributes through
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    //TextureCoord[gl_InvocationID] = UV[gl_InvocationID];

    UV_tcs[gl_InvocationID] = UV_vt[gl_InvocationID];
    initialUV = UV_vt[gl_InvocationID];
    Position_worldspace_tcs = Position_worldspace_vt[gl_InvocationID];
    Normal_cameraspace_tcs = Normal_cameraspace_vt[gl_InvocationID];
    EyeDirection_cameraspace_tcs = EyeDirection_cameraspace_vt[gl_InvocationID];
    LightDirection_cameraspace_tcs = LightDirection_cameraspace_vt[gl_InvocationID];
    
    
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
