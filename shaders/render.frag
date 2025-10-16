#version 410 core
in vec2 vUV;
uniform sampler2D screenTexture;
out vec4 FragColor;

void main(){
    FragColor = texture(screenTexture, vUV);
}
