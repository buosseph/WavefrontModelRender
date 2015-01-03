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
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

const GLchar* vertexSource =
	"#version 150 core\n"
	"in vec3 position;"
	"uniform mat4 model;"
	"uniform mat4 view;"
	"uniform mat4 proj;"
	"void main() {"
	"   gl_Position = proj * view * model * vec4(position, 1.0);"
	"}";

const GLchar* fragmentSource =
	"#version 150 core\n"
	"out vec4 outColor;"
	"void main() {"
	"   outColor = vec4(0.0, 0.5, 1.0, 1.0);"
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
	GLuint pyramid_vbo;
	glGenBuffers(1, &pyramid_vbo);
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,

		0.f, 0.5f, 0.f
	};
	glBindBuffer(GL_ARRAY_BUFFER, pyramid_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Elements
	GLuint pyramid_ebo;
	glGenBuffers(1, &pyramid_ebo);
	GLuint elements[] = {
		0, 1, 2,
		2, 3, 0,

		0, 1, 4,
		1, 2, 4,
		2, 3, 4,
		3, 0, 4,
	};
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramid_ebo);
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
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);


	// Model View Projection
	// View
	glm::mat4 view = glm::lookAt(
		glm::vec3(0.f, 1.2f, 3.f),		// camera position
		glm::vec3(0.f, 0.f, 0.f),		// target
		glm::vec3(0.f, 1.f, 0.f)		// up
	);
	GLuint uniView = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	// Projection
	glm::mat4 proj = glm::perspective(
		45.f,			// Field of View (in degrees)
		4.f / 3.f,		// Aspect ratio
		0.01f,			// Near clipping plane
		10.f			// Far clipping plane
	);
	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	do {
		// screen clear
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Model transforms
		glm::mat4 model;
		model = glm::rotate(
			model,
			(float)clock() / (float)CLOCKS_PER_SEC * 90.f,
			glm::vec3(0.f, 1.f, 0.f)
		);
		GLuint uniModel = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

		// draw element(s)
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

		// swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
		&& glfwWindowShouldClose(window) == 0);


	// Deallocate
	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	glDeleteBuffers(1, &pyramid_ebo);
	glDeleteBuffers(1, &pyramid_vbo);

	glDeleteVertexArrays(1, &vao);

	return 0;
}