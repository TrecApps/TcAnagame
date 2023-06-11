#version 450

// Input is points in the form of Vec 2 and Colors
layout (location = 0) in vec2 input_point;
layout (location = 1) in vec4 input_color;

// Set the output for reception by the Fragment Shader
layout (location = 0) out vec4 output_color;

void main() {
	// Assign the output of the Vertex shader the point we got
	gl_Position  = vec4(input_point, 0.0, 1.0);
	
	// set the color for the Fragment Shader
	output_color = input_color;
}