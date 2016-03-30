// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// Shader library
#include <common/shader.hpp>

#define BUFFER_OFFSET( offset ) ((GLvoid*) (offset))

/* User-defined classes to generate multiple Koch-curves */
class Snowflake {
public:
	Snowflake();
	~Snowflake();
	void destroy();

	std::vector<glm::vec3> vertices;
	float curr_angle, delta_angle;
	float scale;
	float xcor, ycor, zindex;
	float direction, speed;

private:
	void koch_line(glm::vec3, glm::vec3, int);
};

GLFWwindow* window;

GLuint programID;
GLuint VAID;
GLuint VBID;

/* For coloring objects */
GLuint vertexbuffer;
GLuint colorbuffer;
GLuint whitebuffer;
static const GLfloat g_vertex_buffer_data[] = {
    -1.2f, 1.0f, 0.0f,
    1.2f, 1.0f, 0.0f,
    1.2f, -0.5f, 0.0f,
    -1.2f, 1.0f, 0.0f,
    1.2f, -0.5f, 0.0f,
    -1.2f, -0.5f, 0.0f,

    -1.2f, -0.5f, 0.0f,
    1.2f, -0.5f, 0.0f,
    1.2f, -1.0f, 0.0f,
    -1.2f, -0.5f, 0.0f,
    1.2f, -1.0f, 0.0f,
    -1.2f, -1.0f, 0.0f,
};
// std::vector<float> g_color_buffer_data;
static const GLfloat g_color_buffer_data[] = {
    0.0f, 0.0f, 0.3f,
    0.0f, 0.0f, 0.3f,
    1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 0.3f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,

    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    0.54f, 0.27f, 0.07f,
    1.0f, 1.0f, 1.0f,
    0.54f, 0.27f, 0.07f,
    0.54f, 0.27f, 0.07f,
};
std::vector<float> g_white_buffer_data;


std::vector<Snowflake> flakes;

glm::mat4 Projection;
glm::mat4 View;

static const int NUM_FLAKES = 30;
static const int MAX_NUM_FLAKES = 800;
static const float MIN_SCALE = 0.01f;
static const float MAX_SCALE = 0.03f;
static const float WIND_MIN = -0.15f;
static const float WIND_MAX = 0.2f;
static const float WIND_SCALE = 0.001f;
static const float GRAVITY_MIN = 0.005f;
static const float GRAVITY_SCALE = 0.00002f;
float wind, current;
float gravity, acceleration;

float random_float(float floor, float ceiling) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = ceiling - floor;
	float r = random * diff;
	return floor + r;
}

/* Begin Snowflake class function definitions */
Snowflake::Snowflake() {
	Snowflake::vertices.push_back(glm::vec3(-0.5f, -0.25f, 0.0f));
	Snowflake::vertices.push_back(glm::vec3(0.5f, -0.25f, 0.0f));
	Snowflake::vertices.push_back(glm::vec3(0.0f, sqrt(0.75f) - 0.25f, 0.0f));
	Snowflake::koch_line(Snowflake::vertices[0], Snowflake::vertices[1], 2);
	Snowflake::koch_line(Snowflake::vertices[1], Snowflake::vertices[2], 2);
	Snowflake::koch_line(Snowflake::vertices[2], Snowflake::vertices[0], 2);

	Snowflake::curr_angle = 0.0f;
	Snowflake::delta_angle = random_float(-2.0f, 2.0f);
	Snowflake::scale = random_float(MIN_SCALE, MAX_SCALE);
	Snowflake::xcor = random_float(-1.5f, 1.5f);
	Snowflake::ycor = 0.9f;
	Snowflake::zindex = rand()/100;
	Snowflake::direction = random_float(-0.005f, 0.01f);
	Snowflake::speed = random_float(0.002f, 0.01f);
}

Snowflake::~Snowflake() {
	Snowflake::vertices.clear();
}

void Snowflake::destroy() {
	Snowflake::~Snowflake();
}

// TODO: Implement koch snowflake
/* koch_line() has been change to a Snowflake member function */
void Snowflake::koch_line(glm::vec3 a, glm::vec3 b, int iter)
{
	if (iter < 0)
		return;

	glm::vec3 normal = glm::vec3(-(a.y - b.y), -(b.x - a.x), 0.0f);
	glm::vec3 c = (float)(sqrt(3.0f) / 6.0f) * normal + (a + b) / 2.0f;
	glm::vec3 c1 = (float)(sqrt(3.0f) / 6.0f) * -1 * normal + (a + b) / 2.0f;
	glm::vec3 d = (2.0f * a + b) / 3.0f;
	glm::vec3 e = (a + 2.0f * b) / 3.0f;

	Snowflake::vertices.push_back(c);
	Snowflake::vertices.push_back(d);
	Snowflake::vertices.push_back(e);
	Snowflake::vertices.push_back(c1);
	Snowflake::vertices.push_back(d);
	Snowflake::vertices.push_back(e);

	Snowflake::koch_line(a, d, iter - 1);
	Snowflake::koch_line(b, e, iter - 1);
	Snowflake::koch_line(c, d, iter - 1);
	Snowflake::koch_line(d, e, iter - 1);
	Snowflake::koch_line(e, c, iter - 1);
	Snowflake::koch_line(c1, d, iter - 1);
	Snowflake::koch_line(d, e, iter - 1);
	Snowflake::koch_line(e, c1, iter - 1);
}
/* End of Snowflake class function definitions */

// TODO: Initialize model
void init_model(void)
{
    /* Generate color vertex array */
    Snowflake flake;
    for (int i=0; i<flake.vertices.size() * 3; i++) {
        g_white_buffer_data.push_back(1.0f);
    }
    std::cout<<g_white_buffer_data.size();
	/* Generate multiple Koch-curves */
	for (int i = 0; i < NUM_FLAKES; i++) {
		Snowflake flake;
		flakes.push_back(flake);
	}

	// Generates Vertex Array Objects in the GPU's memory and passes back their identifiers
	// Create a vertex array object that represents vertex attributes stored in a vertex buffer object.
	glGenVertexArrays(1, &VAID);
	glBindVertexArray(VAID);

	// Create and initialize a buffer object. Generates our buffers in the GPU's memory
	glGenBuffers(1, &VBID);
	glBindBuffer(GL_ARRAY_BUFFER, VBID);
	for (int i = 0; i < flakes.size(); i++) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * flakes[i].vertices.size(), &flakes[i].vertices[0], GL_STATIC_DRAW);
	}

    /* For coloring objects */
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * g_color_buffer_data.size(), &g_color_buffer_data[0], GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &whitebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, whitebuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * g_white_buffer_data.size(), &g_white_buffer_data.front(), GL_STATIC_DRAW);
}

// TODO: Draw model
void draw_model()
{
	glUseProgram(programID);
	glBindVertexArray(VAID);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, whitebuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));

	/* Set wind and update current */
	if (wind > 0) {
		current += WIND_SCALE;
		if (current > wind) {
			wind = random_float(WIND_MIN, WIND_MAX);
		}
	}
	else {
		current -= 0.001;
		if (current < wind) {
			wind = random_float(WIND_MIN, WIND_MAX);
		}
	}
	/* Set additional gravity and acceleration */
	if (gravity > 0) {
		acceleration += GRAVITY_SCALE;
		if (acceleration > gravity) {
			gravity = 0.0f;
		}
	}
	else {
		acceleration -= GRAVITY_SCALE;
		if (acceleration < 0.0f) {
			gravity = random_float(0.0f, GRAVITY_MIN);
		}
	}

	int num_destroyed = 0;
	for (int i = 0; i < flakes.size(); i++) {
		/* Update Snowflake x-directions subject to wind current */
		flakes[i].direction = 0.7f * flakes[i].direction + current / flakes[i].scale * MIN_SCALE * MAX_SCALE;
		flakes[i].xcor += flakes[i].direction;
		flakes[i].ycor -= flakes[i].speed + acceleration;
		/* Count landed Snowflakes*/
		if (flakes[i].ycor < -1.0f) {
			num_destroyed++;
		}
		flakes[i].curr_angle += flakes[i].delta_angle;
		glm::mat4 rotation = glm::rotate(flakes[i].curr_angle, glm::vec3(0, 0, 1));
		glm::mat4 translation = glm::translate(glm::mat4(1.0f),
			glm::vec3(flakes[i].xcor, flakes[i].ycor, 0.0f));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), 
			glm::vec3(flakes[i].scale, flakes[i].scale, 0.0f));
		glm::mat4 RBT = translation * rotation;
		glm::mat4 MVP = Projection * View * RBT * scale;

		GLuint MatrixID = glGetUniformLocation(programID, "MVP");
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, flakes[i].vertices.size());
	}
	/* Destroy landed Snowflakes */
	for (int i = 0; i < flakes.size(); i++) {
		if (flakes[i].ycor < -1.0f) {
			flakes.erase(flakes.begin() + i);
		}
	}
	/* Create new Snowflakes */
	if (random_float(0.0f, 1.0f) < 0.2f) {
		Snowflake flake;
		flakes.push_back(flake);
	}
	for (int i = 0; i < num_destroyed; i++) {
		if (flakes.size() < MAX_NUM_FLAKES) {
			Snowflake flake;
			flakes.push_back(flake);
		}
	}

    /* For background objects */
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 MVP = Projection * View * Model;
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(g_vertex_buffer_data));
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

int main(int argc, char* argv[])
{
	/* Generate random seed and run a few samples to make a difference*/
	srand(time(NULL));
	for (int i = 0; i < rand(); i++) {
		rand();
	}

	// Step 1: Initialization
	if (!glfwInit())
	{
		return -1;
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// TODO: GLFW create window and context
	window = glfwCreateWindow(1024, 768, "Lab 1", NULL, NULL);
	if (window == NULL) {
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// END

	// TODO: Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		return -1;
	}
	// END

	Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	View = glm::lookAt(glm::vec3(0, 0, 2),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	// TODO: Initialize OpenGL and GLSL
	// glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// For Macbook Pro Retina screen scaling
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	programID = LoadShaders("VertexShader.glsl", "fragmentShader.glsl");
	GLuint MatrixId = glGetUniformLocation(programID, "MVP");
	// END
	init_model();

	// Step 2: Main event loop
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		draw_model();

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (!glfwWindowShouldClose(window));

	// Step 3: Termination
	for (int i = 0; i < flakes.size(); i++) {
		flakes[i].vertices.clear();
		flakes.erase(flakes.begin() + i);
	}

	glDeleteBuffers(1, &VBID);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VAID);

	glfwTerminate();

	return 0;
}
