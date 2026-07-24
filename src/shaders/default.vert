#version 430 core

out vec2 uv;

void main() {
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);
    gl_Position = vec4(x, y, 0.0, 1.0);

    uv = gl_Position.xy * 0.5 + 0.5;
}