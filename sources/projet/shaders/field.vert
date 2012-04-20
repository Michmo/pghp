#version 150

in vec3 vtx_position;
in vec3 vtx_normal;
in vec2 vtx_texcoord;

out vec3 normal;
out vec2 texcoord;
out vec3 scenePosition;
out float fogFactor;

uniform mat4 matriceVue;
uniform mat4 matriceProjection;
uniform mat4 matriceScene;
uniform mat3 matriceNormale;


void main()
{
    scenePosition = vec3(matriceScene *vec4(vtx_position,1));
    gl_Position=  matriceProjection * matriceVue * matriceScene * vec4(vtx_position.xyz,1);
    texcoord = vtx_position.xy/2 + vec2(0.5);
    normal = matriceNormale * vtx_normal;
}
