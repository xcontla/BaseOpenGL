#version 410 core
in vec2 vUV;
out vec4 FragColor;

void main(){
    FragColor = vec4(vUV, 1.0, 1.0);
}
