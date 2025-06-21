#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 WorldMatrix;

out vec3 ReflectDir;
out vec3 ViewDir;

void main()
{
    vec4 worldPos = WorldMatrix * model * vec4(aPos, 1.0);
    vec3 fragPos = vec3(worldPos);
    vec3 normal = mat3(transpose(inverse(WorldMatrix * model))) * aNormal;

    vec3 viewPos = vec3(inverse(view)[3]);
    ViewDir = normalize(fragPos - viewPos);
    ReflectDir = reflect(ViewDir, normalize(normal));

    gl_Position = projection * view * worldPos;
}
