
#define _USE_MATH_DEFINES
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <common\shader.hpp>
#include <iostream>
#include <cmath>
#include <iterator>
#include <algorithm> 
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


float Vex(float x, float y, float alpha, float delta);
float Vey(float x, float y, float alpha, float delta);
float Grady(float x, float y, float delta);
float Gradx(float x, float y, float delta);
float Function(float x, float y);
float poids(int longueur_courante);


typedef struct
{
    GLfloat x, y, z;
} Data;


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    Shader ourShader("StandardShading.vertexshader", "StandardShading.fragmentshader");


    float step = 10.0;
    float delta = step / 100.0;
    Data data[100][100];
    



    for (int x = 0; x < 100; x += 1) {
        for (int y = 0; y < 100; y += 1) {
            float x_data = Vex(-5 + (x / step) , -5 + (y / step) , M_PI / 4,delta );
            float y_data = Vey(-5 + (x / step) , -5 + (y / step) , M_PI / 4, delta);
            float z_data = 0;
            data[x][y].x = x_data;
            data[x][y].y = y_data;
            data[x][y].z = z_data;
          
         

        }
    }


 
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

   

    // 
   //

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(0);





    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(255.0f, 255.0f, 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        ourShader.use();
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_POINTS, 0, 3);
        // glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

float Function(float x, float y) {


    return 8 * exp((-(pow(x, 2) + pow(y, 2)) / 5));
}

float Gradx(float x, float y, float delta) {

    return (Function(x + delta, y) - Function(x, y)) / delta;

}

float Grady(float x, float y, float delta) {

    return (Function(x, y + delta) - Function(x, y)) / delta;

}

float Vex(float x, float y, float alpha, float delta) {

    return cos(alpha) * Gradx(x, y, delta) - sin(alpha) * Grady(x, y, delta);

}

float Vey(float x, float y, float alpha, float delta) {

    return sin(alpha) * Gradx(x, y, delta) - cos(alpha) * Grady(x, y, delta);

}