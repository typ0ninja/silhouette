// tessellation control shader
#version 410 core

// specify number of control points per patch output
// this value controls the size of the input and output arrays
layout (vertices=3) out;
//layout (vertices=4) out;

in vec3 Position_worldspace_vt[];
in vec3 LightDirection_tangentspace_vt[];
in vec3 EyeDirection_tangentspace_vt[];

// varying input from vertex shader
in vec2 TexCoord[];
// varying output to evaluation shader
out vec2 TextureCoord[];

out vec3 Position_worldspace_tcs[];
out vec3 LightDirection_tangentspace_tcs[];
out vec3 EyeDirection_tangentspace_tcs[];

uniform vec3 cameraPos;

void main()
{
    // ----------------------------------------------------------------------
    // pass attributes through
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];
    
    Position_worldspace_tcs[gl_InvocationID] = Position_worldspace_vt[gl_InvocationID];
    LightDirection_tangentspace_tcs[gl_InvocationID] = LightDirection_tangentspace_vt[gl_InvocationID];
    EyeDirection_tangentspace_tcs[gl_InvocationID] = EyeDirection_tangentspace_vt[gl_InvocationID];

    // ----------------------------------------------------------------------
    // invocation zero controls tessellation levels for the entire patch
    if (gl_InvocationID == 0)
    {
        
        vec3 vert0 = gl_in[0].gl_Position.xyz;
        vec3 vert1 = gl_in[1].gl_Position.xyz;
        vec3 vert2 = gl_in[2].gl_Position.xyz;
        
        vec3 dir1 = normalize(vert1 - vert0);
        vec3 dir2 = normalize(vert2 - vert0);

          // Calculate the cross product of the two vectors
        vec3 s_norm = normalize(cross(dir1, dir2));
        vec3 center = vec3((vert0 + vert1 + vert2)) / 3.0;
        vec3 triDir = normalize(center - cameraPos);
        
        float dot = dot(s_norm, triDir);
        /*
        gl_TessLevelOuter[0] = 8;
        gl_TessLevelOuter[1] = 8;
        gl_TessLevelOuter[2] = 8;
        //gl_TessLevelOuter[3] = 1;

        gl_TessLevelInner[0] = 8;
        gl_TessLevelInner[1] = 8;
        //gl_TessLevelInner[2] = 1;
         */
        
        if(dot <= 0 && dot >= -.35){
            gl_TessLevelOuter[0] = 8;
            gl_TessLevelOuter[1] = 8;
            gl_TessLevelOuter[2] = 8;
            //gl_TessLevelOuter[3] = 1;

            gl_TessLevelInner[0] = 8;
            gl_TessLevelInner[1] = 8;
            //gl_TessLevelInner[2] = 1;
        } else if (dot < -.35 && dot >= -.6) {
            gl_TessLevelOuter[0] = 4;
            gl_TessLevelOuter[1] = 4;
            gl_TessLevelOuter[2] = 4;
            //gl_TessLevelOuter[3] = 1;

            gl_TessLevelInner[0] = 4;
            gl_TessLevelInner[1] = 4;
            //gl_TessLevelInner[2] = 1;
        
        } else {
            gl_TessLevelOuter[0] = 1;
            gl_TessLevelOuter[1] = 1;
            gl_TessLevelOuter[2] = 1;
            //gl_TessLevelOuter[3] = 1;

            gl_TessLevelInner[0] = 1;
            gl_TessLevelInner[1] = 1;
            //gl_TessLevelInner[2] = 1;
        }
        
        
    }
}
    
