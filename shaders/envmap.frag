#version 330 core
in vec3 ReflectDir;
out vec4 FragColor;

uniform samplerCube envMap;

void main()
{
    vec3 color = texture(envMap, ReflectDir).rgb;
    FragColor = vec4(color, 1.0);
}
