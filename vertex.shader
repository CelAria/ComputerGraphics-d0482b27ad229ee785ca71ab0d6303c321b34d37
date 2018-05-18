
#version 330 core
  
layout (location = 0) in vec3 position;
//layout (location = 1) in vec3 color; 

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vec3 color; 
void main()
{
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position.x, position.y, position.z, 1.0);
	//scaling by whatever is in model matrix, then whatever is in the view matrix, then by the projection matrix
	//col = color; 
}