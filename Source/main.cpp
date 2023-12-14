#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>
#include <vector>
#include <memory>
#include <ctime>

const char *vertexSource = R"(
	#version 460 core

	layout (location = 0) in vec3 a_Position;
	layout (location = 3) in mat4 a_InstanceMatrix;
	layout (location = 7) in vec3 a_Color;

	layout (location=0) out vec3 out_Color;

	uniform mat4 u_View;

	uniform mat4 u_Projection;

	void main()
	{
		out_Color = a_Color;
		gl_Position = u_Projection * u_View * a_InstanceMatrix * vec4(a_Position, 1.0);
	}
)";

const char *fragmentSource = R"(
	#version 460 core

	layout (location=0) in vec3 in_Color;

	out vec4 out_FragColor;

	void main()
	{
		out_FragColor = vec4(in_Color, 1.0);
	}
)";

float vertices[] = {
	0.5f, 0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, 0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f
};

uint32_t indices[] = {
	4, 2, 0,
	2, 7, 3,
	6, 5, 7,
	1, 7, 5,
	0, 3, 1,
	4, 1, 5,
	4, 6, 2,
	2, 6, 7,
	6, 4, 5,
	1, 3, 7,
	0, 2, 3,
	4, 0, 1
};

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const uint32_t MAX_INSTANCES = 3500000; // 3,5M Cubes (42M Triangles and 28M vertices)

GLFWwindow* handle;
uint32_t program, vertexShader, fragmentShader;
uint32_t vao, ibo, vbo;
uint32_t sceneArea = 2000;
uint32_t currentWidth = WIDTH, currentHeight = HEIGHT;
float halfSceneArea = sceneArea * 0.5f;
float lastTime = 0;

glm::vec3 viewPosition = glm::vec3(0.0f, 0.0f, -1000.0f);
glm::vec3 viewRotation = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 direction = glm::vec3(0.0, 0.0, 1.0);

int main()
{
	if (!glfwInit())
		std::cerr << "Failed to initialize glfw" << '\n';

	glfwSetErrorCallback([](int code, const char *description){ 
		std::cerr << description << '\n'; });

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	handle = glfwCreateWindow(WIDTH, HEIGHT, "OPENGL | Instancing", nullptr, nullptr);
	if (handle == nullptr)
		std::cerr << "Failed to create glfw handle" << '\n';

	glfwSetFramebufferSizeCallback(handle, [](GLFWwindow* window, int width, int height){ 
			currentWidth = width;
			currentHeight = height;
			glViewport(0, 0, width, height); 
		});

	glfwMakeContextCurrent(handle);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		std::cerr << "Failed to initialize glad" << '\n';

	glfwSwapInterval(0);

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, nullptr);
	glCompileShader(vertexShader);

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
	glCompileShader(fragmentShader);

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);

	glUseProgram(program);

	srand((uint32_t)time(nullptr));

	std::shared_ptr<glm::mat4> matrices = std::shared_ptr<glm::mat4>(new glm::mat4[MAX_INSTANCES]);
	std::shared_ptr<glm::vec3> colors = std::shared_ptr<glm::vec3>(new glm::vec3[MAX_INSTANCES]);

	for (uint32_t i = 0; i < MAX_INSTANCES; ++i)
	{
		glm::vec3 position = glm::vec3(rand() % sceneArea - halfSceneArea, rand() % sceneArea - halfSceneArea, rand() % sceneArea - halfSceneArea);
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);

		matrices.get()[i] = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
		colors.get()[i] = glm::vec3(rand() % 256 / 256.0f, rand() % 256 / 256.0f, rand() % 256 / 256.0f);
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	uint32_t matrixBuffer;
	glGenBuffers(1, &matrixBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, matrixBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * sizeof(glm::mat4), &matrices.get()[0], GL_STATIC_DRAW);
		
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	uint32_t colorBuffer;
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * sizeof(glm::vec3), &colors.get()[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glVertexAttribDivisor(7, 1);

	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(handle))
	{
		float currentTime = (float)glfwGetTime();
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glfwGetKey(handle, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(handle, true);

		viewRotation.z += 0.25f * deltaTime;

		if (viewPosition.z >= 1000.0f) 
			direction = glm::vec3(0.0, 0.0, -1.0);
		else if(viewPosition.z <= -1000.0f)
			direction = glm::vec3(0.0, 0.0, 1.0);

		viewPosition += 50.0f * direction * deltaTime;

		float ratio = (float)currentWidth / (float)currentHeight;
		glm::mat4 view = glm::translate(glm::mat4(1.0f), viewPosition) * glm::toMat4(glm::quat(viewRotation));
		glm::mat4 projection = glm::perspective(glm::radians(60.0f), ratio, 0.1f, 10000.0f);

		glUniformMatrix4fv(glGetUniformLocation(program, "u_View"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(program, "u_Projection"), 1, GL_FALSE, &projection[0][0]);

		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, MAX_INSTANCES);

		glfwSwapBuffers(handle);
		glfwPollEvents();
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &ibo);
	glDeleteBuffers(1, &vbo);

	glDeleteBuffers(1, &matrixBuffer);
	glDeleteBuffers(1, &colorBuffer);

	glfwDestroyWindow(handle);
	glfwTerminate();
}