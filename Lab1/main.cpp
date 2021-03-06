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

GLFWwindow* window;
int windowWidth, windowHeight;

GLuint programID;
GLuint VAID;
GLuint VBID;

std::vector<glm::vec3> g_vertex_buffer_data;

glm::mat4 Projection;
glm::mat4 View;
float degree = 0.0f;

// TODO: Implement koch snowflake
void koch_line(glm::vec3 a, glm::vec3 b, int iter)
{
    if (iter < 0)
        return;

    glm::vec3 normal = glm::vec3(-(a.y - b.y), -(b.x - a.x), 0.0f);
    glm::vec3 c = (float)(sqrt(3.0f) / 6.0f) * normal + (a + b) / 2.0f;
    glm::vec3 c1 = (float)(sqrt(3.0f) / 6.0f) * -1 * normal + (a + b) / 2.0f;
    glm::vec3 d = (2.0f * a + b) / 3.0f;
    glm::vec3 e = (a + 2.0f * b) / 3.0f;

    g_vertex_buffer_data.push_back(c);
    g_vertex_buffer_data.push_back(d);
    g_vertex_buffer_data.push_back(e);
    g_vertex_buffer_data.push_back(c1);
    g_vertex_buffer_data.push_back(d);
    g_vertex_buffer_data.push_back(e);

    koch_line(a, d, iter-1);
    koch_line(b, e, iter-1);
    koch_line(c, d, iter-1);
    koch_line(d, e, iter-1);
    koch_line(e, c, iter-1);
    koch_line(c1, d, iter-1);
    koch_line(d, e, iter-1);
    koch_line(e, c1, iter-1);
}

// TODO: Initialize model
void init_model(void)
{
    g_vertex_buffer_data = std::vector<glm::vec3>();

    g_vertex_buffer_data.push_back(glm::vec3(-0.5f, -0.25f, 0.0f));
    g_vertex_buffer_data.push_back(glm::vec3(0.5f, -0.25f, 0.0f));
    g_vertex_buffer_data.push_back(glm::vec3(0.0f, sqrt(0.75)-0.25f, 0.0f));

    koch_line(g_vertex_buffer_data[0], g_vertex_buffer_data[1], 5);
    koch_line(g_vertex_buffer_data[1], g_vertex_buffer_data[2], 5);
    koch_line(g_vertex_buffer_data[2], g_vertex_buffer_data[0], 5);

    // Generates Vertex Array Objects in the GPU's memory and passes back their identifiers
    // Create a vertex array object that represents vertex attributes stored in a vertex buffer object.
    glGenVertexArrays(1, &VAID);
    glBindVertexArray(VAID);

    // Create and initialize a buffer object. Generates our buffers in the GPU's memory
    glGenBuffers(1, &VBID);
    glBindBuffer(GL_ARRAY_BUFFER, VBID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*g_vertex_buffer_data.size(),
                 &g_vertex_buffer_data[0], GL_STATIC_DRAW);
}

// TODO: Draw model
void draw_model()
{
    glUseProgram(programID);
    glBindVertexArray(VAID);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));

    degree += 0.5f;
    glm::mat4 rotation = glm::rotate(degree, glm::vec3(0, 0, 1));
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.0f));
    glm::mat4 RBT = translation * rotation;
    // glm::mat4 Model = glm::mat4(1.0f);
    // glm::mat4 MVP = Projection * View * Model;
    glm:mat4 MVP = Projection * View * RBT * scale;

    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, g_vertex_buffer_data.size());

    glDisableVertexAttribArray(0);
}

int main(int argc, char* argv[])
{
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
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // For Macbook Pro Retina screen scaling
    int width, height;
    glfwGetFramebufferSize(window,&width, &height);
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
    g_vertex_buffer_data.clear();

    glDeleteBuffers(1, &VBID);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VAID);

    glfwTerminate();

    return 0;
}