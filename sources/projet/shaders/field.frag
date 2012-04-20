#version 150

in vec3 normal;
in vec2 texcoord;
in vec3 scenePosition;
in float fogFactor;

out vec4 out_color;

uniform sampler2D fieldSampler;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float shininess;
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
    out_color = texture(fieldSampler, texcoord)
            +0.3
            * vec4(diffusionCoeff(normal.xyz, normalize(lightDir))
            * lightColor
            + vec3(specularCoeff(normal.xyz, normalize(lightDir), normalize(posCam - scenePosition) , shininess)),1.0);

    //brouillard
    const float LOG2 = 1.442695;
    float far = 700;
    float fogCoord = (gl_FragCoord.z / gl_FragCoord.w) / far;
    float fogDensity = 6;
    float fog = fogDensity * fogCoord;
    vec4 fogColor = vec4(1,1,1,0);
    out_color = mix(fogColor, out_color, clamp(1.0-fog, 0.0,1.0));


}
