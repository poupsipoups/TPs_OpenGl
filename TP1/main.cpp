#include <cmath>
#include <cstddef>
#include <cstdint>
#include "glm/fwd.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <filesystem>
#include <glimac/FilePath.hpp>
#include <glimac/Program.hpp>
#include <glimac/glm.hpp>
#include <vector>

int window_width  = 1280;
int window_height = 720;

static void key_callback(GLFWwindow* /*window*/, int /*key*/, int /*scancode*/, int /*action*/, int /*mods*/)
{
}

static void mouse_button_callback(GLFWwindow* /*window*/, int /*button*/, int /*action*/, int /*mods*/)
{
}

static void scroll_callback(GLFWwindow* /*window*/, double /*xoffset*/, double /*yoffset*/)
{
}

static void cursor_position_callback(GLFWwindow* /*window*/, double /*xpos*/, double /*ypos*/)
{
}

static void size_callback(GLFWwindow* /*window*/, int width, int height)
{
    window_width  = width;
    window_height = height;
}

int main()
{
    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

    /* Create a window and its OpenGL context */
#ifdef __APPLE__
    /* We need to explicitly ask for a 3.3 context on Mac */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "TP1", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Intialize glad (loads the OpenGL functions) */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    /* Hook input callbacks */
    glfwSetKeyCallback(window, &key_callback);
    glfwSetMouseButtonCallback(window, &mouse_button_callback);
    glfwSetScrollCallback(window, &scroll_callback);
    glfwSetCursorPosCallback(window, &cursor_position_callback);
    glfwSetWindowSizeCallback(window, &size_callback);

    /*Include the shaders */
    glimac::Program program = glimac::loadProgram(
        std::filesystem::current_path().string() + "/../../TP1/shaders/triangle.vs.glsl",
        std::filesystem::current_path().string() + "/../../TP1/shaders/triangle.fs.glsl");
    program.use();

    // structure de vertexs !!
    struct Vertex2DColor {
        glm::vec2 m_position;
        glm::vec3 m_color;

        Vertex2DColor()
        {
            m_position = glm::vec2(0.);
            m_color    = glm::vec3(1.);
        }

        Vertex2DColor(glm::vec2 position, glm::vec3 color)
        {
            m_position = position;
            m_color    = color;
        }
    };

    // Création d'un VBO
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    const int NB_DIVISIONS = 50;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /* QUAD VERTICES */

    // Vertex2DColor vertices[] = {
    //     Vertex2DColor(glm::vec2(-0.5, 0.5), glm::vec3(1, 0, 0)),
    //     Vertex2DColor(glm::vec2(0.5, -0.5), glm::vec3(0, 1, 0)),
    //     Vertex2DColor(glm::vec2(-0.5, -0.5), glm::vec3(0, 0, 1)),
    //     Vertex2DColor(glm::vec2(-0.5, 0.5), glm::vec3(1, 0, 0)),
    //     Vertex2DColor(glm::vec2(0.5, 0.5), glm::vec3(0, 0, 1)),
    //     Vertex2DColor(glm::vec2(0.5, -0.5), glm::vec3(0, 1, 0))};

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /* CIRCLE EXERCICE */

    /* VERTICES */
    std::vector<Vertex2DColor> vertices;
    for (int i = 0; i < NB_DIVISIONS; i++) {
        vertices.push_back(Vertex2DColor());
        vertices.push_back(Vertex2DColor(glm::vec2(std::cos(i * 2 * M_PI / NB_DIVISIONS), std::sin(i * 2 * M_PI / NB_DIVISIONS)), glm::vec3(1., 1, 0.01 * i)));
        vertices.push_back(Vertex2DColor(glm::vec2(std::cos((i + 1) * 2 * M_PI / NB_DIVISIONS), std::sin((i + 1) * 2 * M_PI / NB_DIVISIONS)), glm::vec3(1., 1., 0.01 * i)));
    }

    /* BUFFER DATA */
    glBufferData(GL_ARRAY_BUFFER, NB_DIVISIONS * 3 * sizeof(Vertex2DColor), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* IBO */
    // => Creation du IBO
    GLuint ibo;
    glGenBuffers(1, &ibo);
    // => On bind sur GL_ELEMENT_ARRAY_BUFFER, cible reservée pour les IBOs
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    /* INDICES */

    // => Tableau d'indices: ce sont les indices des sommets à dessiner
    // On en a 6 afin de former deux triangles
    // Chaque indice correspond au sommet correspondant dans le VBO
    std::vector<uint32_t> indices;

    for (int i = 1; i <= NB_DIVISIONS; i++) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    // we put the last one manually
    indices.push_back(0);
    indices.push_back(NB_DIVISIONS + 1);
    indices.push_back(1);

    /*  buffer data of the indices */
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, NB_DIVISIONS * 3 * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // création d'un VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // => On bind l'IBO sur GL_ELEMENT_ARRAY_BUFFER; puisqu'un VAO est actuellement bindé,
    // cela a pour effet d'enregistrer l'IBO dans le VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    const GLuint VERTEX_ATTR_POSITION = 3;
    const GLuint VERTEX_ATTR_COLOR    = 8;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_COLOR);

    // on bind le vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)offsetof(Vertex2DColor, m_position));

    glVertexAttribPointer(VERTEX_ATTR_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)(offsetof(Vertex2DColor, m_color)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.f, 0.5f, 0.5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // DESSIN
        glBindVertexArray(vao);
        // glDrawArrays(GL_TRIANGLES, 0, NB_DIVISIONS * 3);
        glDrawElements(GL_TRIANGLES, NB_DIVISIONS * 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glfwTerminate();
    return 0;
}