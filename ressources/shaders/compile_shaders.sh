#!/bin/bash

# Shader Compiler Script für Vulkan / SPIR-V
# Benötigt: vulkan-devel / shaderc / glslang

# Ordner
SHADER_DIR="/home/christian/CLionProjects/IHS/ressources/shaders/"
OUT_DIR="/home/christian/CLionProjects/IHS/ressources/shaders/bin/"

mkdir -p "$OUT_DIR"

echo "Compiling shaders..."

# Vertex Shader
glslc "$SHADER_DIR/shader.vert" -o "$OUT_DIR/shader.vert.spv"

# Fragment Shader
glslc --target-env=vulkan1.2 "$SHADER_DIR/shader.frag" -o "$OUT_DIR/shader.frag.spv"

echo "Done."