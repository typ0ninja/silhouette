// tessellation evaluation shader
#version 410 core
in vec2 UV_tcs[];
patch in vec2 initialUV;
patch in vec3 Position_worldspace_tcs;
patch in vec3 Normal_cameraspace_tcs;
patch in vec3 EyeDirection_cameraspace_tcs;
patch in vec3 LightDirection_cameraspace_tcs;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

layout (quads, fractional_odd_spacing, ccw) in;

uniform sampler2D heightMap;  // the texture corresponding to our height map
uniform mat4 model;           // the model matrix
uniform mat4 view;            // the view matrix
uniform mat4 projection;      // the projection matrix

// received from Tessellation Control Shader - all texture coordinates for the patch vertices
//in vec2 TextureCoord[];

// send to Fragment Shader for coloring
out float Height;

void main()
{
    //Pass through:
    UV = initialUV;
    Position_worldspace = Position_worldspace_tcs;
    Normal_cameraspace = Normal_cameraspace_tcs;
    EyeDirection_cameraspace = EyeDirection_cameraspace_tcs;
    LightDirection_cameraspace = LightDirection_cameraspace_tcs;
    
    //for (int i = 0; i < UV_out.length(); i++) {
    //    UV[i] = UV_out[i];
    //}
    //out vec3 Position_worldspace[] = Position_worldspace_out;
    //out vec3 Normal_cameraspace[] = Normal_cameraspace_out;
    //out vec3 EyeDirection_cameraspace[] = EyeDirection_cameraspace_out;
    //out vec3 LightDirection_cameraspace[] = LightDirection_cameraspace_out;
    // get patch coordinate
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    
    // ----------------------------------------------------------------------
    // retrieve control point texture coordinates
    vec2 t00 = UV_tcs[0];
    vec2 t01 = UV_tcs[1];
    vec2 t10 = UV_tcs[2];
    //vec2 t11 = UV_tcs[3];
    
    UV = initialUV;

    // bilinearly interpolate texture coordinate across patch
    //vec2 t0 = (t01 - t00) * u + t00;
    //vec2 t1 = (t11 - t10) * u + t10;
    //vec2 texCoord = (t1 - t0) * v + t0;

    // lookup texel at patch coordinate for height and scale + shift as desired
    // Height = texture(heightMap, texCoord).y * 64.0 - 16.0;

    // ----------------------------------------------------------------------
    // retrieve control point position coordinates
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    //vec4 p11 = gl_in[3].gl_Position;

    // compute patch surface normal
    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vec4 normal = normalize( vec4(cross(vVec.xyz, uVec.xyz), 0) );

    // bilinearly interpolate position coordinate across patch
    vec4 p0 = (p01 - p00) * u + p00;
    //vec4 p1 = (p11 - p10) * u + p10;
    //vec4 p = (p1 - p0) * v + p0;

    // displace point along normal
    //p += normal * Height;
    

    // ----------------------------------------------------------------------
    // output patch point position in clip space
    gl_Position = projection * view * model * p0;
}
