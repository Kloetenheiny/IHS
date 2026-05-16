#version 450
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference2 : require

// Layout muss exakt deiner ShaderData-Struktur entsprechen
//buffer_refernce sagt aus, dass das kein normaler Uniform block ist, sondern ein pointer typ zu einem uint64 buffer
//std430 = speicherlayout, 16 bit alignment
//buffer_reference_align = minimales alignment der adresse selbst
layout(buffer_reference, std430, buffer_reference_align = 16) readonly buffer ShaderData
{
    mat4 projection;
    mat4 view;
    mat4 model;
};

layout(push_constant) uniform PushConstants {
    ShaderData shaderData;  // ist intern eine uint64 (device adfdress)
} pc;

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main()
{
    mat4 mvp = pc.shaderData.projection * pc.shaderData.view * pc.shaderData.model;
    gl_Position = mvp * vec4(inPos, 0.0, 1.0);
    fragColor = inColor;
}