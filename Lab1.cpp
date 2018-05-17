//Lab 2
//modified from http://learnopengl.com/

#include "stdafx.h"

#include "glew\glew.h"	// include GL Extension Wrangler
#include "glfw\glfw3.h"	// include GLFW helper library
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#define STB_IMAGE_IMPLEMENTATION //define for STB image loading library
#include "stb_image.h" // include for image loader library

using namespace std;



// Window dimensions
const GLuint windowWidth = 800, windowHeight = 800;

glm::vec3 triangle_scale;
glm::vec3 camera_translation = glm::vec3(0.0f, 0.0f, -1.0f);

const float TRIANGLE_MOVEMENT_STEP = 0.1f;
const float CAMERA_PAN_STEP = 0.2f;

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

}


// The MAIN function, from here we start the application and run the game loop
int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Heightmap", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);


	// Build and compile our shader program
	// Vertex shader

	// Read the Vertex Shader code from the file
	string vertex_shader_path = "vertex.shader";
	string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, ios::in);

	if (VertexShaderStream.is_open()) {
		string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?\n", vertex_shader_path.c_str());
		getchar();
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	string fragment_shader_path = "fragment.shader";
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path, std::ios::in);

	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory?\n", fragment_shader_path.c_str());
		getchar();
		exit(-1);
	}

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(vertexShader, 1, &VertexSourcePointer, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(fragmentShader, 1, &FragmentSourcePointer, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader); //free up memory
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);


	triangle_scale = glm::vec3(1.0f/100, 1.0f/100, 1.0f); //shorthand, initializes all 4 components to 1.0f


	//change of coordinates between model 

	

	//IMAGE/TEXTURE PROCESSING
	//loading the image 
	int nrChannels;
	unsigned char *data = stbi_load("bwimage.png", &width, &height, &nrChannels, 0);

	unsigned int texture;
	glGenTextures(1, &texture);

	//text filtering options
	glBindTexture(GL_TEXTURE_2D, texture);

	//test image size is within contstraints, else exit
	if (width > 1024 || height > 1024 || width < 256 || height < 256)
	{
		std::cout << "failed to load image because it is not within size constraints. Image is sized:" << height << " " << width << std::endl;
		exit(0);
	}
	if (data)
	{	//turn image into a texter
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "successful load" << std::endl;
		std::cout << height << std::endl;
		std::cout << width << std::endl;
	}
	else
	{
		std::cout << "failed to load texture" << std::endl;
	}

	int numtriangles = ((width - 1)*(height - 1) * 2); //see pixel index notes
	int numvertices = numtriangles * 3;
	GLfloat *vertices = new GLfloat[numvertices*3]; // allocate memory dynamically 

	//looping through image to add to VBO 

	for (int i=0; i < width-1; i++) //exclude last row
	{
		for (int j = 0; j< height-1; j++) // exclude last column 
		{
			int heightmapvalue= data[(width*i)+j];  // get each individual colour value 
			
			float v[] = {
				data[(width*i) + j]/255.0,
				data[((width*i) + j) + 1]/255.0,
				data[((width*(i + 1)) + j)]/255.0, // add 1 to i 
				data[((width*(i + 1)) + j) + 1]/255
			};

			float x = i - (0.5*width); // centers in window

			float y = j - (0.5*height);


			GLfloat pixelvertices[] =
			{
				x, y, v[0], // first triangle 
				x+1, y, v[1], 
				x, y+1, v[2], 
				x+1, y+1, v[3],
				x, y + 1, v[2],
				x + 1, y, v[1], // fix order

			};

			//float z = (heightmapvalue / 255.0); // make a floating point between 0 and 1
			for (int k = 0; k < sizeof(pixelvertices) / sizeof(GLfloat); k++)
			{

				vertices[(((width-1)*i) + j) *18+k ]= pixelvertices[k]; // coordinates per vertex times number of vertices in two triangles + k (relative offset)
				
			}
			//std::cout << heightmapvalue << std::endl;
		}
	}

	

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//glGenBuffers(2, &VBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, (numvertices*3)*sizeof(GLfloat), vertices, GL_STATIC_DRAW); // multiply by the size of GL float (4 bits) because Specifies the size in bytes of the buffer object's new data store.

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	GLuint transformLoc = glGetUniformLocation(shaderProgram, "model_matrix");
	GLuint viewMatrixLoc = glGetUniformLocation(shaderProgram, "view_matrix");
	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection_matrix");


	//free image memory
	stbi_image_free(data);


	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat4 model_matrix;
		model_matrix = glm::scale(model_matrix, triangle_scale);
		//model_matrix = glm::rotate(model_matrix, glm::mesh_rotation(1.0f, 0.0f, 0.0f));

		glm::mat4 view_matrix;
		view_matrix = glm::lookAt(glm::vec3(0.0f, -10.0f, 3.0f), //camera positioned here CAMERA is 5 units back
			glm::vec3(0.0f, 0.0f, 0.0f), //looks at origin
			glm::vec3(0.0f, 1.0f, 0.0f)); //up vector

		glm::mat4 projection_matrix;
		projection_matrix = glm::perspectiveFov(45.0f, (GLfloat)windowWidth, (GLfloat)windowHeight, 0.0f, 100.0f);

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix)); //broadcast the uniform value to the shaders
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, numvertices); // number of triangles to draw
		glBindVertexArray(0);

		struct vertex_array {

		};

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
