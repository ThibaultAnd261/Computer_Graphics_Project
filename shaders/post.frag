#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D screenTexture;
uniform int postEffect;

void main() {
    vec3 col = texture(screenTexture, TexCoords).rgb;

    if (postEffect == 1) {
        // Grayscale
        float gray = dot(col, vec3(0.2125, 0.7154, 0.0721));
        col = vec3(gray);
    }
    else if (postEffect == 2) {
        // Inversion
        col = vec3(1.0) - col;
    }
    else if (postEffect == 3) {
        // Sépia
        col = vec3(
            dot(col, vec3(0.393, 0.769, 0.189)),
            dot(col, vec3(0.349, 0.686, 0.168)),
            dot(col, vec3(0.272, 0.534, 0.131))
        );
    }

    FragColor = vec4(col, 1.0);
}
