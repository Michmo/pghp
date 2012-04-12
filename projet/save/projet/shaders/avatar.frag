#version 150

in vec3 normal;
in vec2 texcoord;
in vec3 scenePosition;

out vec4 out_color;

uniform sampler2D chevalSampler;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float shininess;
uniform vec3 lightPos;
uniform vec3 posCam;



float specularCoeff(in vec3 n, in vec3 l, in vec3 v, in float shininess){
float x = pow(dot(n, normalize(v + l)), shininess);
return max(x,0);
}



float diffusionCoeff(in vec3 n, in vec3 l){
return max(dot(n,l),0);
}

void main()
{
    out_color = texture(chevalSampler, texcoord)
              +0.3
              * vec4(diffusionCoeff(normal.xyz, normalize(lightDir))
              * lightColor
              + vec3(specularCoeff(normal.xyz, normalize(lightDir), normalize(posCam - scenePosition),
                shininess)),1.0);
}
