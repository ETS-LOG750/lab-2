#version 400 core

uniform mat4 mvMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormal;

out vec3 fNormal;
out vec3 fPosition;

void main()
{
     // Multiplication model view matrix
     // position maintenant exprimee dans l'espace camera
     vec4 vEyeCoord = mvMatrix * vPosition;
     // Multiplication par la matrice de projection
     // point maintenant exprimee dans NDC
     gl_Position = projMatrix * vEyeCoord;

     // Passage de ces variables au fragment shader
     fPosition = vEyeCoord.xyz;
     fNormal = normalMatrix*vNormal; 
}

