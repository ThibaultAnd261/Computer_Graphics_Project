#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D diffuseMap;

void main()
{
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normalize(Normal), lightDir), 0.0);
    vec3 texColor = texture(diffuseMap, TexCoord).rgb;
    vec3 finalColor = diff * texColor;
    FragColor = vec4(finalColor, 1.0);
}
