#version 450

// Input is primarily from Vertex in 2D points
layout (location = 0) in vec2 input_point;

// Set the output for reception by the Fragment Shader
layout (location = 0) out vec4 output_color;

// Single Color will come from a single source
uniform vec4 single_color;

void main() {
	// Assign the output of the Vertex shader the point we got
	gl_Position  = vec4(input_point, 0.0, 1.0);
	
	// set the color for the Fragment Shader
	output_color = single_color;
}