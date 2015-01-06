/* References for understanding OpenGL
 * - http://www.learnopengl.com/ (Really good)
 * - https://open.gl/ (provides source code)
 * - http://www.opengl-tutorial.org/ (some bits don't work or just aren't explained well)
 * - http://mbsoftworks.sk/index.php?page=tutorials&series=1 (Haven't looked at it much, but has some cool topics)
 * - https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/index.php (to understand shaders and GLSL, but some of the syntax is out of date)
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

GLuint width = 640;
GLuint height = 480;
bool showWireframe = false;
bool firstMouse = true;
GLfloat lastX = width / 2;
GLfloat lastY = height / 2; 

enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera {
	public:
		glm::vec3 position;
		glm::vec3 front;
		glm::vec3 up;
		glm::vec3 right;
		glm::vec3 worldUp;

		GLfloat yaw;
		GLfloat pitch;

		GLfloat movementSpeed;
		GLfloat mouseSensitivity;
		GLfloat zoom;

		Camera(GLuint viewportWidth, GLuint viewportHeight, glm::vec3 position = glm::vec3(0.f, 0.f, 0.f), glm::vec3 worldUp = glm::vec3(0.f, 1.f, 0.f), GLfloat yaw = -90.f, GLfloat pitch = 0.f){
			this->viewportWidth = viewportWidth;
			this->viewportHeight = viewportHeight;
			this->position = position;
			this->worldUp = worldUp;
			this->yaw = yaw;
			this->pitch = pitch;
			this->movementSpeed = 0.05f;
			this->mouseSensitivity = 0.25f;
			this->zoom = 45.f;
			this->updateCameraVectors();
		}

		// WASD
		void moveCamera(CameraMovement direction) {
			if (direction == FORWARD)
				this->position += this->front * movementSpeed;
			if (direction == BACKWARD)
				this->position -= this->front * movementSpeed;
			if (direction == LEFT)
				this->position -= this->right * movementSpeed;
			if (direction == RIGHT)
				this->position += this->right * movementSpeed;
		}

		// Mouse cursor
		void tiltCamera(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = true) {
			xOffset *= this->mouseSensitivity;
			yOffset *= this->mouseSensitivity;

			this->yaw 	+= xOffset;
			this->pitch += yOffset;

			if(constrainPitch) {
				if (this->pitch > 89.f)
					this->pitch = 89.f;
				if (this->pitch < -89.f)
					this->pitch = -89.f;
			}
			this->updateCameraVectors();
		}

		// Mouse wheel
		void zoomCamera(GLfloat yOffset) {
			if (this->zoom >= 1.f && this->zoom <= 45.f)
				this->zoom -= yOffset;
			if (this->zoom <= 1.f)
				this->zoom = 1.f;
			if (this->zoom >= 45.f)
				this->zoom = 45.f;
		}

		glm::mat4 getViewMatrix() {
			return glm::lookAt(this->position, this->position + this->front, this->worldUp);
		}

		glm::mat4 getProjMatrix() {
			return glm::perspective(
				this->zoom,			// Field of View (in degrees)
				(GLfloat)viewportWidth / (GLfloat)viewportHeight,		// Aspect ratio (should be widnow width / height)
				0.01f,				// Near clipping plane
				1000.f				// Far clipping plane
			);
		}

	private:
		GLuint viewportWidth;
		GLuint viewportHeight;
		void updateCameraVectors() {
			glm::vec3 front;
			front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
			front.y = sin(glm::radians(this->pitch));
			front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

			this->front = glm::normalize(front);
			this->right = glm::normalize(glm::cross(this->front, this->worldUp));
			this->up 	= glm::normalize(glm::cross(this->right, this->front));
		}
};


Camera camera(width, height, glm::vec3(0.f, 0.f, 1.f));

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xOffset = xpos - lastX;
	GLfloat yOffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.tiltCamera(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	camera.zoomCamera(yOffset);
}

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
	window = glfwCreateWindow(width, height, title, NULL, NULL);
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

	glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);



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


	// Model, View, Projection Uniforms
	GLuint uniModel = glGetUniformLocation(shaderProgram, "model");
	GLuint uniView = glGetUniformLocation(shaderProgram, "view");
	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");

	// Model (Constant)
	glm::mat4 model = glm::mat4();	// Identity matrix
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));



	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			showWireframe = !showWireframe;
			if (showWireframe) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			} else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.moveCamera(FORWARD);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.moveCamera(BACKWARD);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.moveCamera(LEFT);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.moveCamera(RIGHT);


		// View & Projection (Camera)
		glm::mat4 view = camera.getViewMatrix();
		glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
		glm::mat4 proj = camera.getProjMatrix();
		glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

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