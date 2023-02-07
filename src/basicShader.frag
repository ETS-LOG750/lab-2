#version 400 core
in vec3 fNormal;
in vec3 fPosition;

out vec4 oColor;

void main()
{
    // Affichage normale (exprimee dans l'espace camera)
    vec3 nNormal = normalize(fNormal);
    oColor = vec4(nNormal * 0.5 + 0.5, 1);
}