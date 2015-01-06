/* References for understanding OpenGL
 * - https://open.gl/ (provides source code)
 * - http://www.opengl-tutorial.org/ (some bits don't work or just aren't explained well)
 * - http://mbsoftworks.sk/index.php?page=tutorials&series=1 (Haven't looked at it much, but has some cool topics)
 * - https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/index.php (to understand shaders and GLSL, but some of the syntax is out of date)
 * - http://www.learnopengl.com/
 */

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>	// Window & keyboard, contains OpenGL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

const GLchar* vertexSource =
	"#version 330 core\n"
	"in vec3 position;"
	"uniform mat4 model;"
	"uniform mat4 view;"
	"uniform mat4 proj;"
	"void main() {"
	"   gl_Position = proj * view * model * vec4(position, 1.0);"
	"}";

const GLchar* fragmentSource =
	"#version 330 core\n"
	"in vec3 normal;"
	"out vec4 outColor;"
	"void main() {"
	"   outColor = vec4(0.0, 0.5, 1.0, 1.0);"
	// "   vec3 surfaceColor = vec3(0.0, 0.5, 1.0, 1.0);"
	// "	vec3 lightColor = vec4(1.0, 1.0, 1.0);"
	// "	float cosTheta = clamp(dot(normal, 1), 0, 1);"
	// "	outColor = vec4(surfaceColor * lightColor * cosTheta, 1.0);"
	"}";

struct  DirectionalLight {
	glm::vec3 color;
	glm::vec3 direction;
	float ambientIntensity;
} sunlight;

bool showWireframe = false;

int render(const char* title, uint numVertices, float* vertices, uint numFaces, int* faces) {
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
	if (!window) {
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
	GLuint model_vbo;
	glGenBuffers(1, &model_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, model_vbo);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(float), vertices, GL_STATIC_DRAW);

	// Elements
	GLuint model_ebo;
	glGenBuffers(1, &model_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numFaces * sizeof(int), faces, GL_STATIC_DRAW);



	// Vertex Shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	// Fragment Shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	// Check Shader Complilation
	GLint compiled;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
	if (compiled != GL_TRUE) {
		fprintf(stderr, "Error compiling vertex shader\n");

		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(vertexShader, 1024, &log_length, message);
		fprintf(stderr, "%s", message);

		glDeleteShader(vertexShader);
		glDeleteBuffers(1, &model_ebo);
		glDeleteBuffers(1, &model_vbo);
		glDeleteVertexArrays(1, &vao);

		return -1;
	}
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
	if (compiled != GL_TRUE) {
		fprintf(stderr, "Error compiling fragment shader\n");

		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(fragmentShader, 1024, &log_length, message);
		fprintf(stderr, "%s", message);

		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteBuffers(1, &model_ebo);
		glDeleteBuffers(1, &model_vbo);
		glDeleteVertexArrays(1, &vao);

		return -1;
	}

	// Link Shaders to Program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);

	// Confirm Linking and Use Program
	GLint linked;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
	if (linked != GL_TRUE) {
		fprintf(stderr, "Error linking shader program\n");

		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(shaderProgram, 1024, &log_length, message);
		fprintf(stderr, "%s", message);

		glDeleteProgram(shaderProgram);
		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteBuffers(1, &model_ebo);
		glDeleteBuffers(1, &model_vbo);
		glDeleteVertexArrays(1, &vao);

		return -1;
	}
	glUseProgram(shaderProgram);

	// Specify Layout of Vertex Data
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);	// Here because attribute is static


	// // Generate & Link Normals
	// GLuint normals_vbo;
	// glGenBuffers(1, &normals_vbo);
	// glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
	// std::vector<glm::vec3> normals;
	// glm::vec3 x;
	// glm::vec3 y;
	// for (int i = 0; i < numFaces; i += 3) {
	// 	x = glm::vec3(
	// 		vertices[faces[i+1] * 3] - vertices[faces[i] * 3],
	// 		vertices[faces[i+1] * 3 + 1] - vertices[faces[i] * 3 + 1],
	// 		vertices[faces[i+1] * 3 + 2] - vertices[faces[i] * 3 + 2]
	// 	);
	// 	y = glm::vec3(
	// 		vertices[faces[i+2] * 3] - vertices[faces[i] * 3],
	// 		vertices[faces[i+2] * 3 + 1] - vertices[faces[i] * 3 + 1],
	// 		vertices[faces[i+2] * 3 + 2] - vertices[faces[i] * 3 + 2]
	// 	);
	// 	glm::vec3 normal = glm::normalize(glm::cross(x,y));
	// 	normals.push_back(normal);
	// }
	// glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	// GLint normalsAttrib = glGetAttribLocation(shaderProgram, "normal");
	// glVertexAttribPointer(normalsAttrib, 3, GL_FLOAT, GL_TRUE, 0, 0);
	// glEnableVertexAttribArray(normalsAttrib);

	// Background Color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	// Face Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// // Directional Light
	// sunlight.color = glm::vec3(1.f, 1.f, 1.f);
	// sunlight.direction = glm::vec3(0.f, -1.f, 0.f);
	// sunlight.ambientIntensity = 0.2f;
	// GLuint uniSunlight = glGetUniformLocation(shaderProgram, "sunlight");
	// glUniformMatrix4fv(uniSunlight, 1, GL_FALSE, glm::value_ptr(sunlight));



	// Model View Projection
	// View (Constant)
	glm::mat4 view = glm::lookAt(
		glm::vec3(0.f, 1.2f, 3.f),		// camera position
		glm::vec3(0.f, 0.f, 0.f),		// target
		glm::vec3(0.f, 1.f, 0.f)		// up
	);
	GLuint uniView = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	// Projection (Constant)
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			showWireframe = !showWireframe;
			if (showWireframe) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			} else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}

		// Model transforms
		glm::mat4 model;
		model = glm::rotate(
			model,
			(float)clock() / (float)CLOCKS_PER_SEC * 90.f,
			glm::vec3(0.f, 1.f, 0.f)
		);
		GLuint uniModel = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));


		glDrawElements(GL_TRIANGLES, numFaces, GL_UNSIGNED_INT, 0);


		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
		&& glfwWindowShouldClose(window) == 0);



	// Deallocate
	// glDisableVertexAttribArray(normalsAttrib);
	glDisableVertexAttribArray(posAttrib);

	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	glDeleteBuffers(1, &model_ebo);
	glDeleteBuffers(1, &model_vbo);

	glDeleteVertexArrays(1, &vao);

	glfwTerminate();
	return 0;
}