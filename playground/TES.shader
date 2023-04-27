// tessellation evaluation shader
#version 410 core

//layout (quads, fractional_odd_spacing, ccw) in;
layout (triangles, equal_spacing, ccw) in;
/*

uniform sampler2D heightMap;  // the texture corresponding to our height map
uniform mat4 model;           // the model matrix
uniform mat4 view;            // the view matrix
uniform mat4 projection;      // the projection matrix
 */

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform mat3 MV3x3;
uniform vec3 LightPosition_worldspace;
uniform sampler2D DiffuseTextureSampler;
uniform sampler2D NormalTextureSampler;
uniform sampler2D SpecularTextureSampler;


// received from Tessellation Control Shader - all texture coordinates for the patch vertices
in vec2 TextureCoord[];

in vec3 Position_worldspace_tcs[];
in vec3 LightDirection_tangentspace_tcs[];
in vec3 EyeDirection_tangentspace_tcs[];

out vec2 UV;

out vec3 Position_worldspace_tes;
out vec3 LightDirection_tangentspace_tes;
out vec3 EyeDirection_tangentspace_tes;

vec3 bilerpPatch(vec3 pos0, vec3 pos1, vec3 pos2, vec3 bary) {
    
    vec3 xinterp = vec3((pos1 - pos0) * bary.x + pos0);
    
    return (pos2 - pos0) * bary.y + xinterp;
}

void main()
{
    // get barycentric coordinate
    vec3 barycentric = gl_TessCoord;

    // ----------------------------------------------------------------------
    // retrieve control point texture coordinates
    vec2 t0 = TextureCoord[0];
    vec2 t1 = TextureCoord[1];
    vec2 t2 = TextureCoord[2];

    // bilinearly interpolate texture coordinate across patch
    vec2 texCoord = (t1 - t0) * barycentric.x + t0;
    texCoord = (t2 - t0) * barycentric.y + texCoord;
    UV = texCoord;

    // lookup texel at patch coordinate for height and scale + shift as desired
    float height = texture(NormalTextureSampler, texCoord).y;// * 64.0 - 16.0;

    // ----------------------------------------------------------------------
    // retrieve control point position coordinates
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;
    vec4 p2 = gl_in[2].gl_Position;

    // compute patch surface normal
    vec4 uVec = p1 - p0;
    vec4 vVec = p2 - p0;
    vec4 normal = normalize( vec4(cross(vVec.xyz, uVec.xyz), 0) );

    // bilinearly interpolate position coordinate across patch
    vec4 p = (p1 - p0) * barycentric.x + p0;
    p = (p2 - p0) * barycentric.y + p;
    
    Position_worldspace_tes = bilerpPatch(Position_worldspace_tcs[0],
        Position_worldspace_tcs[1], Position_worldspace_tcs[2], barycentric);

    LightDirection_tangentspace_tes = bilerpPatch(LightDirection_tangentspace_tcs[0],
        LightDirection_tangentspace_tcs[1], LightDirection_tangentspace_tcs[2], barycentric);
    
    EyeDirection_tangentspace_tes = bilerpPatch(EyeDirection_tangentspace_tcs[0],
        EyeDirection_tangentspace_tcs[1], EyeDirection_tangentspace_tcs[2], barycentric);
    
    // displace point along normal
    p += normal * height * 0.1;

    // ----------------------------------------------------------------------
    // output patch point position in clip space
    //gl_Position = projection * view * model * p;
    gl_Position = MVP * p;
}
