#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <math.h>

using namespace std;

void construct_points(float pasEchantillonage);
void setNewDatas(int sample);

float function_calc(float x, float y);
float Gradx(float x, float y, float delta);
float Grady(float x, float y, float delta);
float Vex(float x, float y, float alpha, float delta);
float Vey(float x, float y, float alpha, float delta);

float i;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
int processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


GLint uniform_offset_x;
GLint uniform_scale_x;

const float scale_x = 5.;

bool add_is_press = false;
bool subtract_is_press = false;

typedef struct
{
    GLfloat x, y, z;
} Data;

unsigned int VBO,VBO_colors, VAO;


const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"

"uniform float offset_x;\n"
"uniform float scale_x;\n"
"out vec3 color;\n"

"void main()\n"
"{\n"
"   gl_Position = vec4((aPos.x + 0.0f) / scale_x, (aPos.y + 0.0f) / scale_x, aPos.z, 1.0);\n"
"   color = aColor;\n"

"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"

"void main()\n"
"{\n"
"   FragColor = vec4(1., 1., 1., 1.);\n"

"}\n\0";

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

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    int id_scale_x = glGetUniformLocation(shaderProgram, "scale_x");
    glUseProgram(shaderProgram);
    glUniform1f(id_scale_x, scale_x);

    // Nombre d'echantillons
    int sample = 1;

    // Ajuster le nombre d'echantillons
    int input_ch;
    int step_sample = 10;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // Input
        input_ch = processInput(window);

        if (input_ch == 1) {
            sample += step_sample;
        }
        if (input_ch == 2) {
            sample -= step_sample;
        }

        std::cout << "Nombre d'echantillons : " << sample << std::endl;

        // Actualiser les donnees
        setNewDatas(sample);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        // draw our first triangle
        glUseProgram(shaderProgram);
        

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, 1000000);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

        //break;
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
int processInput(GLFWwindow* window) { 
    
    // Fermer la fenetre
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
       
        return -1;
    }

    // 'Debloquer' les touches '+' et '-'
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_RELEASE && add_is_press) {
        std::cout << "Ca release add" << std::endl;
        add_is_press = false;

    }

    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_RELEASE && subtract_is_press) {
        std::cout << "Ca release subtract" << std::endl;
        subtract_is_press = false;

    }

    // 'Bloquer' les touches '+' et '-' & Envoie du signal
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS && !add_is_press) {
        // Cas du '+'
        std::cout << "C'est plus" << std::endl;
        add_is_press = true;

        return 1;
    }

    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS && !subtract_is_press) {
        // Cas du '-'
        std::cout << "C'est moins" << std::endl;
        subtract_is_press = true;

        return 2;
    }

    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

float function_calc(float x, float y){
    return 8 * exp(-((pow(x, 2) + pow(y, 2)) / 5)) * 100;
}

float Gradx(float x, float y, float delta) {
    return (function_calc(x + delta, y) - function_calc(x, y)) / delta;
}

float Grady(float x, float y, float delta) {
    return (function_calc(x, y + delta) - function_calc(x, y)) / delta;
}

float Vex(float x, float y, float alpha, float delta) {
    return cos(alpha) * Gradx(x, y, delta) - sin(alpha) * Grady(x, y, delta);
}

float Vey(float x, float y, float alpha, float delta) {
    return sin(alpha) * Gradx(x, y, delta) - cos(alpha) * Grady(x, y, delta);
}

float norme_vecteur(float vex, float vey) {
    return sqrt(pow(vex, 2) + pow(vey, 2));
}

void setNewDatas(int sample) {

    int next_x;
    int next_y;
    float delta = 0.75;
    float alpha = M_PI / 2;

    Data data[sample][sample];

    for (int x = 0; x <= sample ; x += 1) {
        for (int y = 0; y <= sample; y += 1) {
            
            // Conversion de l'ensemble {0, sample} a l'ensemble {-5, 5}
            float x_data = (x - 0.5 * (float)sample) / (0.1 * (float)sample);
            float y_data = (y - 0.5 * (float)sample) / (0.1 * (float)sample);
            //float z_data = function_calc(x_data, y_data);
            
            next_x = x_data;
            next_y = y_data;
            
            for (int i = 0; i < 2; i++) {
                if(next_x <= 5 && -5 <= next_x && next_y <= 5 && -5 <= next_y) {
                    // Calcul des vecteurs associes a la position
                    float x_vecteur = Vex(next_x, next_y, alpha, delta);
                    float y_vecteur = Vey(next_x, next_y, alpha, delta);
                    float x_grad = Gradx(next_x, next_y, delta);
                    float y_grad = Grady(next_x, next_y, delta);

                    //std::cout << "Vex : " << x_vecteur << " | Vey : " << y_vecteur << std::endl;

                    float norme = norme_vecteur(x_vecteur, y_vecteur);
                    x_vecteur /= norme;
                    y_vecteur /= norme;

                    std::cout << "next_x : " << next_x << " | next_y : " << next_y << std::endl;
                    std::cout << "x_data : " << x_data << " | y_data : " << y_data << std::endl;
                    std::cout << "Vex : " << x_vecteur << " | Vey : " << y_vecteur << std::endl;
                    //std::cout << "Gradx : " << x_grad << " | Grady : " << y_grad << std::endl;
                    std::cout << "" << std::endl;

                    // Memorisation des donnees 
                    // A MODIFIER -- IL FAUT RECONVERTIR next_x ET next_y
                    data[next_x][next_y].x = next_x;
                    data[next_x][next_y].y = next_y;
                    data[next_x][next_y].z = 0.0f;

                    // Mise a jour du point courant
                    next_x += (x_vecteur * 10);
                    next_y += (y_vecteur * 10);
                }
            }
        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VBO_colors);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_colors);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
   
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);  

    glBindVertexArray(0);

}