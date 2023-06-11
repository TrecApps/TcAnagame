#version 450

layout(binding = 0) uniform sampler2D texSampler;

layout (location = 0) in vec2 input_coords;

layout (location = 0) out vec4 output_color;

void main(){
	output_color = texture(texSampler, input_coords);
}