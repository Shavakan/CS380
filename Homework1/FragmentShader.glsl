#version 330 core

/* Input color from VertexShader */
in vec3 fragmentColor;
// Ouput color
out vec3 color;

void main(){
	// Output color = color specified in the vertex shader
	// color = vec3(1.0, 1.0, 1.0);
	color = fragmentColor;
}