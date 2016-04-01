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

/* User-defined buffers */
GLuint bgvertexbuffer;
GLuint olafvertexbuffer;
GLuint bgcolorbuffer;
GLuint whitecolorbuffer;
GLuint olafcolorbuffer;

/* For coloring objects */
static const GLfloat bg_vertex_buffer_data[] = {
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
static GLfloat olaf_vertex_buffer_data[] = {
    // TODO: Land Olaf on the ground.
    /* Olaf's head : 4 triangles */
    0.300f, 0.656f, 0.1f,  0.444f, 0.711f, 0.1f,  0.578f, 0.578f, 0.1f,
    0.233f, 0.489f, 0.1f,  0.300f, 0.656f, 0.1f,  0.578f, 0.578f, 0.1f,
    0.233f, 0.489f, 0.1f,  0.578f, 0.578f, 0.1f,  0.367f, 0.322f, 0.1f,
    0.367f, 0.322f, 0.1f,  0.578f, 0.578f, 0.1f,  0.622f, 0.356f, 0.1f,
    /* Olaf's upper body : 5 triangles */
    0.367f, 0.322f, 0.1f,  0.444f, 0.144f, 0.1f,  0.333f, 0.244f, 0.1f,
    0.367f, 0.322f, 0.1f,  0.622f, 0.356f, 0.1f,  0.444f, 0.144f, 0.1f,
    0.444f, 0.144f, 0.1f,  0.622f, 0.356f, 0.1f,  0.644f, 0.144f, 0.1f,
    0.622f, 0.356f, 0.1f,  0.711f, 0.300f, 0.1f,  0.644f, 0.144f, 0.1f,
    0.644f, 0.144f, 0.1f,  0.711f, 0.300f, 0.1f,  0.733f, 0.211f, 0.1f,
    /* Olaf's lower body : 6 triangles*/
    0.444f, 0.144f, 0.1f,  0.644f, 0.144f, 0.1f,  0.778f, 0.100f, 0.1f,
    0.444f, 0.144f, 0.1f,  0.778f, 0.100f, 0.1f,  0.333f, 0.044f, 0.1f,
    0.333f, 0.044f, 0.1f,  0.778f, 0.100f, 0.1f,  0.778f, -0.167f, 0.1f,
    0.300f, -0.167f, 0.1f,  0.333f, 0.044f, 0.1f,  0.444f, -0.233f, 0.1f,
    0.333f, 0.044f, 0.1f,  0.778f, -0.167f, 0.1f,  0.444f, -0.233f, 0.1f,
    0.444f, -0.233f, 0.1f,  0.778f, -0.167f, 0.1f,  0.689f, -0.267f, 0.1f,
    /* Olaf's left foot : 3 triangles */
    0.233f, -0.278f, 0.1f,  0.300f, -0.167f, 0.1f,  0.444f, -0.233f, 0.1f,
    0.233f, -0.278f, 0.1f,  0.444f, -0.233f, 0.1f,  0.367f, -0.356f, 0.1f,
    0.233f, -0.278f, 0.1f,  0.367f, -0.356f, 0.1f,  0.300f, -0.356f, 0.1f,
    /* Olaf's right foot : 2 triangles */
    0.578f, -0.244f, 0.1f,  0.689f, -0.267f, 0.1f,  0.689f, -0.356f, 0.1f,
    0.556f, -0.356f, 0.1f,  0.578f, -0.244f, 0.1f,  0.689f, -0.356f, 0.1f,
    /* Olaf's left eye : 4 triangles */
    0.295f, 0.503f, 0.101f,  0.308f, 0.541f, 0.101f,  0.342f, 0.565f, 0.101f,
    0.295f, 0.503f, 0.101f,  0.342f, 0.565f, 0.101f,  0.384f, 0.521f, 0.101f,
    0.295f, 0.503f, 0.101f,  0.384f, 0.521f, 0.101f,  0.377f, 0.464f, 0.101f,
    0.342f, 0.464f, 0.101f,  0.295f, 0.503f, 0.101f,  0.377f, 0.464f, 0.101f,
    /* Olaf's right eye : 4 triangles */
    0.411f, 0.538f, 0.101f,  0.418f, 0.596f, 0.101f,  0.442f, 0.527f, 0.101f,
    0.418f, 0.596f, 0.101f,  0.459f, 0.623f, 0.101f,  0.442f, 0.527f, 0.101f,
    0.442f, 0.527f, 0.101f,  0.459f, 0.623f, 0.101f,  0.493f, 0.589f, 0.101f,
    0.442f, 0.527f, 0.101f,  0.493f, 0.589f, 0.101f,  0.466f, 0.527f, 0.101f,
    /* Olaf's nose : 3 triangles */
    0.425f, 0.486f, 0.101f,  0.442f, 0.527f, 0.101f,  0.466f, 0.527f, 0.101f,
    0.425f, 0.486f, 0.101f,  0.466f, 0.527f, 0.101f,  0.466f, 0.459f, 0.101f,
    0.466f, 0.459f, 0.101f,  0.466f, 0.527f, 0.101f,  0.589f, 0.496f, 0.101f,
    /* Olaf's mouth : 3 triangles */
    0.329f, 0.425f, 0.101f,  0.479f, 0.418f, 0.101f,  0.435f, 0.329f, 0.101f,
    0.435f, 0.329f, 0.101f,  0.479f, 0.418f, 0.101f,  0.527f, 0.349f, 0.101f,
    0.479f, 0.418f, 0.101f,  0.555f, 0.459f, 0.101f,  0.527f, 0.349f, 0.101f,
    /* Olaf's teeth : 3 triangles */
    0.459f, 0.418f, 0.102f,  0.479f, 0.418f, 0.102f,  0.466f, 0.390f, 0.102f,
    0.466f, 0.390f, 0.102f,  0.479f, 0.418f, 0.102f,  0.524f, 0.418f, 0.102f,
    0.479f, 0.418f, 0.102f,  0.521f, 0.440f, 0.102f,  0.524f, 0.418f, 0.102f,
    /* Olaf's first button : 4 triangles */
    0.493f, 0.253f, 0.101f,  0.527f, 0.288f, 0.101f,  0.500f, 0.212f, 0.101f,
    0.500f, 0.212f, 0.101f,  0.527f, 0.288f, 0.101f,  0.534f, 0.199f, 0.101f,
    0.527f, 0.288f, 0.101f,  0.575f, 0.233f, 0.101f,  0.534f, 0.199f, 0.101f,
    0.527f, 0.288f, 0.101f,  0.568f, 0.260f, 0.101f,  0.575f, 0.233f, 0.101f,
    /* Olaf's second button : 3 triangles */
    0.606f, 0.000f, 0.101f,  0.651f, 0.034f, 0.101f,  0.637f, -0.055f, 0.101f,
    0.651f, 0.034f, 0.101f,  0.637f, -0.055f, 0.101f,  0.709f, -0.048f, 0.101f,
    0.651f, 0.034f, 0.101f,  0.705f, 0.014f, 0.101f,  0.709f, -0.048f, 0.101f,
    /* Olaf's third button : 3 triangles */
    0.616f, -0.144f, 0.101f,  0.589f, -0.192f, 0.101f,  0.678f, -0.205f, 0.101f,
    0.616f, -0.144f, 0.101f,  0.678f, -0.205f, 0.101f,  0.692f, -0.171f, 0.101f,
    0.616f, -0.144f, 0.101f,  0.692f, -0.171f, 0.101f,  0.678f, -0.144f, 0.101f,
    /* Olaf's left eyeball : 2 triangles */
    0.319f, 0.511f, 0.102f,  0.339f, 0.531f, 0.102f,  0.361f, 0.514f, 0.102f,
    0.319f, 0.511f, 0.102f,  0.361f, 0.514f, 0.102f,  0.341f, 0.494f, 0.102f,
    /* Olaf's right eyeball : 2 triangles */
    0.435f, 0.582f, 0.102f,  0.438f, 0.555f, 0.102f,  0.466f, 0.582f, 0.102f,
    0.438f, 0.555f, 0.102f,  0.466f, 0.582f, 0.102f,  0.459f, 0.555f, 0.102f,
    /* Olaf's left arm : 10 triangles */
    0.333f, 0.239f, 0.103f,  0.333f, 0.249f, 0.103f,  0.055f, 0.242f, 0.103f,
    0.055f, 0.242f, 0.103f,  0.055f, 0.252f, 0.103f,  0.333f, 0.239f, 0.103f,
    0.050f, 0.247f, 0.103f,  0.060f, 0.247f, 0.103f,  0.055f, 0.274f, 0.103f,
    0.055f, 0.274f, 0.103f,  0.065f, 0.274f, 0.103f,  0.050f, 0.247f, 0.103f,
    0.050f, 0.247f, 0.103f,  0.060f, 0.247f, 0.103f,  0.022f, 0.278f, 0.103f,
    0.022f, 0.278f, 0.103f,  0.032f, 0.278f, 0.103f,  0.050f, 0.247f, 0.103f,
    0.050f, 0.247f, 0.103f,  0.060f, 0.247f, 0.103f,  0.019f, 0.225f, 0.103f,
    0.019f, 0.225f, 0.103f,  0.019f, 0.235f, 0.103f,  0.050f, 0.247f, 0.103f,
    0.050f, 0.247f, 0.103f,  0.060f, 0.247f, 0.103f,  0.055f, 0.205f, 0.103f,
    0.055f, 0.205f, 0.103f,  0.050f, 0.205f, 0.103f,  0.050f, 0.247f, 0.103f,
    /* Olaf's right arm : 10 triangles */
    0.701f, 0.295f, 0.103f,  0.711f, 0.305f, 0.103f,  0.788f, 0.527f, 0.103f,
    0.788f, 0.527f, 0.103f,  0.783f, 0.537f, 0.103f,  0.701f, 0.295f, 0.103f,
    0.788f, 0.527f, 0.103f,  0.783f, 0.537f, 0.103f,  0.822f, 0.514f, 0.103f,
    0.822f, 0.514f, 0.103f,  0.823f, 0.509f, 0.103f,  0.788f, 0.527f, 0.103f,
    0.788f, 0.527f, 0.103f,  0.783f, 0.537f, 0.103f,  0.809f, 0.567f, 0.103f,
    0.809f, 0.567f, 0.103f,  0.803f, 0.568f, 0.103f,  0.790f, 0.527f, 0.103f,
    0.790f, 0.527f, 0.103f,  0.780f, 0.537f, 0.103f,  0.767f, 0.568f, 0.103f,
    0.767f, 0.568f, 0.103f,  0.759f, 0.566f, 0.103f,  0.788f, 0.527f, 0.103f,
    0.790f, 0.527f, 0.103f,  0.780f, 0.537f, 0.103f,  0.745f, 0.537f, 0.103f,
    0.745f, 0.537f, 0.103f,  0.755f, 0.530f, 0.103f,  0.790f, 0.527f, 0.103f,
    /* Olaf's eyebrows : 4 triangles */
    0.281f, 0.568f, 0.103f,  0.288f, 0.562f, 0.103f,  0.336f, 0.644f, 0.103f,
    0.336f, 0.644f, 0.103f,  0.342f, 0.638f, 0.103f,  0.281f, 0.568f, 0.103f,
    0.370f, 0.664f, 0.103f,  0.374f, 0.674f, 0.103f,  0.448f, 0.671f, 0.103f,
    0.448f, 0.671f, 0.103f,  0.455f, 0.667f, 0.103f,  0.370f, 0.664f, 0.103f,
    /* Olaf's hair : 6 triangles */
    0.342f, 0.671f, 0.103f,  0.348f, 0.677f, 0.103f,  0.226f, 0.750f, 0.103f,
    0.226f, 0.750f, 0.103f,  0.230f, 0.758f, 0.103f,  0.342f, 0.671f, 0.103f,
    0.342f, 0.671f, 0.103f,  0.350f, 0.679f, 0.103f,  0.362f, 0.740f, 0.103f,
    0.362f, 0.740f, 0.103f,  0.376f, 0.748f, 0.103f,  0.342f, 0.671f, 0.103f,
    0.342f, 0.671f, 0.103f,  0.348f, 0.677f, 0.103f,  0.202f, 0.680f, 0.103f,
    0.202f, 0.680f, 0.103f,  0.210f, 0.672f, 0.103f,  0.342f, 0.671f, 0.103f,
};
static const GLfloat bg_color_buffer_data[] = {
    0.0f, 0.0f, 0.3f,
    0.0f, 0.0f, 0.3f,
    0.8f, 0.8f, 0.8f,
    0.0f, 0.0f, 0.3f,
    0.8f, 0.8f, 0.8f,
    0.8f, 0.8f, 0.8f,

    0.8f, 0.8f, 0.8f,
    0.8f, 0.8f, 0.8f,
    0.54f, 0.27f, 0.07f,
    0.8f, 0.8f, 0.8f,
    0.54f, 0.27f, 0.07f,
    0.54f, 0.27f, 0.07f,
};
std::vector<float> g_white_buffer_data;
static const GLfloat olaf_color_buffer_data[] = {
    /* Olaf's head : 4 triangles */
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    /* Olaf's upper body : 5 triangles */
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    /* Olaf's lower body : 6 triangles */
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    /* Olaf's left foot : 3 triangles */
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    /* Olaf's right foot : 2 triangles */
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,  0.9f, 0.9f, 0.9f,
    /* Olaf's left eye : 4 triangles */
    0.6f, 0.6f, 0.9f,  0.6f, 0.6f, 0.7f,  0.6f, 0.6f, 0.9f,
    0.6f, 0.6f, 0.9f,  0.6f, 0.6f, 0.7f,  0.6f, 0.6f, 0.9f,
    0.6f, 0.6f, 0.9f,  0.6f, 0.6f, 0.7f,  0.6f, 0.6f, 0.9f,
    0.6f, 0.6f, 0.9f,  0.6f, 0.6f, 0.7f,  0.6f, 0.6f, 0.9f,
    /* Olaf's right eye : 4 triangles */
    0.6f, 0.6f, 0.9f,  0.6f, 0.6f, 0.7f,  0.6f, 0.6f, 0.9f,
    0.6f, 0.6f, 0.9f,  0.6f, 0.6f, 0.7f,  0.6f, 0.6f, 0.9f,
    0.6f, 0.6f, 0.9f,  0.6f, 0.6f, 0.7f,  0.6f, 0.6f, 0.9f,
    0.6f, 0.6f, 0.9f,  0.6f, 0.6f, 0.7f,  0.6f, 0.6f, 0.9f,
    /* Olaf's nose : 3 triangles */
    0.715f, 0.551f, 0.430f,  0.906f, 0.551f, 0.375f,  0.906f, 0.551f, 0.375f,
    0.715f, 0.551f, 0.430f,  0.906f, 0.551f, 0.375f,  0.605f, 0.172f, 0.121f,
    0.605f, 0.172f, 0.121f,  0.906f, 0.551f, 0.375f,  0.742f, 0.297f, 0.160f,
    /* Olaf's mouth : 3 triangles */
    0.6f, 0.6f, 0.9f,  0.6f, 0.6f, 0.7f,  0.6f, 0.6f, 0.9f,
    0.6f, 0.6f, 0.9f,  0.6f, 0.6f, 0.7f,  0.6f, 0.6f, 0.9f,
    0.6f, 0.6f, 0.9f,  0.6f, 0.6f, 0.7f,  0.6f, 0.6f, 0.9f,
    /* Olaf's teeth : 3 triangles */
    0.8f, 0.8f, 0.8f,  0.8f, 0.8f, 0.8f,  0.8f, 0.8f, 0.8f,
    0.8f, 0.8f, 0.8f,  0.8f, 0.8f, 0.8f,  0.8f, 0.8f, 0.8f,
    0.8f, 0.8f, 0.8f,  0.8f, 0.8f, 0.8f,  0.8f, 0.8f, 0.8f,
    /* Olaf's first button : 4 triangles */
    0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,
    0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,
    0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,
    0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,
    /* Olaf's second button : 3 triangles */
    0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,
    0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,
    0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,
    /* Olaf's third button : 3 triangles */
    0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,
    0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,
    0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,
    /* Olaf's left eyeball : 2 triangles */
    0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,
    0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,
    /* Olaf's right eyeball : 2 trianbles */
    0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,
    0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,  0.1f, 0.1f, 0.1f,
    /* Olaf's left arm : 10 triangles */
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    /* Olaf's right arm : 10 triangles */
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    /* Olaf's eyebrows : 4 triangles */
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    /* Olaf's hair : 6 tringles */
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
    0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,  0.430f, 0.359f, 0.309f,
};


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
    Snowflake::curr_angle = 0.0f;
    Snowflake::delta_angle = random_float(-2.0f, 2.0f);
    Snowflake::scale = random_float(MIN_SCALE, MAX_SCALE);
    Snowflake::xcor = random_float(-1.5f, 1.5f);
    Snowflake::ycor = 0.9f;
    Snowflake::direction = random_float(-0.005f, 0.01f);
    Snowflake::speed = random_float(0.002f, 0.01f);

	Snowflake::vertices.push_back(glm::vec3(-0.5f, -0.25f, 0.0f));
	Snowflake::vertices.push_back(glm::vec3(0.5f, -0.25f, 0.0f));
	Snowflake::vertices.push_back(glm::vec3(0.0f, sqrt(0.75f) - 0.25f, 0.0f));
	Snowflake::koch_line(Snowflake::vertices[0], Snowflake::vertices[1], 2);
	Snowflake::koch_line(Snowflake::vertices[1], Snowflake::vertices[2], 2);
	Snowflake::koch_line(Snowflake::vertices[2], Snowflake::vertices[0], 2);
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

    /* Background vertex buffer */
    glGenBuffers(1, &bgvertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, bgvertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bg_vertex_buffer_data), bg_vertex_buffer_data, GL_STATIC_DRAW);

    /* Background color buffer */
    glGenBuffers(1, &bgcolorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, bgcolorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bg_color_buffer_data), bg_color_buffer_data, GL_STATIC_DRAW);

    /* Snowlake White color buffer */
    glGenBuffers(1, &whitecolorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, whitecolorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * g_white_buffer_data.size(), &g_white_buffer_data.front(), GL_STATIC_DRAW);

    /* Olaf vertex buffer */
    glGenBuffers(1, &olafvertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, olafvertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(olaf_vertex_buffer_data), olaf_vertex_buffer_data, GL_STATIC_DRAW);

    /* Olaf color buffer */
    glGenBuffers(1, &olafcolorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, olafcolorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(olaf_color_buffer_data), olaf_color_buffer_data, GL_STATIC_DRAW);
}

// TODO: Draw model
void draw_model()
{
	glUseProgram(programID);
	glBindVertexArray(VAID);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, whitecolorbuffer);
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
    glBindBuffer(GL_ARRAY_BUFFER, bgvertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, bgcolorbuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);


    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 MVP = Projection * View * Model;
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(bg_vertex_buffer_data));

    /* For olaf */
    glBindBuffer(GL_ARRAY_BUFFER, olafvertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, olafcolorbuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  0, (void *) 0);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(bg_vertex_buffer_data) + sizeof(olaf_vertex_buffer_data));


    /* Close enabled buffers */
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
