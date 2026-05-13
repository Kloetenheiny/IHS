#version 450

layout(push_constant) uniform PC
{
    vec2 pos[2];
    vec3 color[3];
} pc;

layout(location = 0) out vec3 fragColor;

void main()
{
    gl_Position = vec4(pc.pos[gl_VertexIndex], 0.0, 1.0);
    fragColor   = pc.color[gl_VertexIndex].rgb;
}