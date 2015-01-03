/* References for understanding OpenGL
 * - https://open.gl/ (provides source code)
 * - http://www.opengl-tutorial.org/ (some bits don't work or just aren't explained well)
 * - http://mbsoftworks.sk/index.php?page=tutorials&series=1 (Haven't looked at it much, but has some cool topics)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>	// Window & keyboard, contains OpenGL
#include <glm/glm.hpp>	// 3D math

using namespace glm;

const GLchar* vertexSource =
	"#version 150 core\n"
	"in vec2 position;"
	"void main() {"
	"   gl_Position = vec4(position, 0.0, 1.0);"
	"}";
const GLchar* fragmentSource =
	"#version 150 core\n"
	"out vec4 outColor;"
	"void main() {"
	"   outColor = vec4(1.0, 1.0, 1.0, 1.0);"
	"}";

int init(const char* title) {
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// Want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	// For MacOSX
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window;
	window = glfwCreateWindow(640, 480, title, NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.  If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = true;	// Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// VAO
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);


	// VBOs
	GLuint square_vbo;
	glGenBuffers(1, &square_vbo);
	float vertices[] = {
		-0.5f, 0.5f,
		0.5f, 0.5f,
		0.5f, -0.5f,
		-0.5f, -0.5f,
	};
	glBindBuffer(GL_ARRAY_BUFFER, square_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Elements
	GLuint square_ebo;
	glGenBuffers(1, &square_ebo);
	GLuint elements[] = {
		0, 1, 2,
		2, 3, 0
	};
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, square_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);


	// Vertex Shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	// Fragment Shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	// Link Vertex and Fragment Shaders into Program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	// Specify Layout of Vertex Data
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);


	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	do {
		// screen clear
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw element(s)
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
		&& glfwWindowShouldClose(window) == 0);
	return 0;
}