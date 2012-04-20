#version 150

in vec3 vtx_position;
in vec3 vtx_normal;
in vec2 vtx_texcoord;

out vec3 normal;
out vec2 texcoord;
out vec3 scenePosition;

uniform mat4 matriceVueAvatar;
uniform mat4 matriceProjectionAvatar;
uniform mat4 matriceSceneAvatar;
uniform mat3 matriceNormaleAvatar;
uniform vec3 move;
uniform mat4 matriceTransformationAvatar;

void main()
{
  vec3 test = vec3(vtx_position.x, vtx_position.y, vtx_position.z);
  //vec3 test = vec3(vtx_position.x, vtx_position.y, vtx_position.z);
  vec3 position = test + move;
  scenePosition = vec3(matriceSceneAvatar *vec4(position,1));
  gl_Position =  matriceProjectionAvatar * matriceVueAvatar * matriceSceneAvatar * vec4(position.xyz,1) ;
  texcoord = position.xy/2 + vec2(0.5);
  normal = matriceNormaleAvatar * vtx_normal;
}
