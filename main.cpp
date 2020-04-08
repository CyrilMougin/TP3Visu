#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <math.h>

using namespace std;

void construct_points(float pasEchantillonage);
void setNewDatas(int sample, double alpha);

double function_calc(double x, double y);
double Gradx(double x, double y, double delta);
double Grady(double x, double y, double delta);
double Vex(double x, double y, double alpha, double delta);
double Vey(double x, double y, double alpha, double delta);

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
    int sample = 200;

    // Angle alpha
    double alpha = M_PI / 2;

    // Ajuster le nombre d'echantillons
    int input_ch;
    int step_sample = 10;
    double step_alpha = 0.125;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // Input
        input_ch = processInput(window);

        if (input_ch == 1) {
            sample += step_sample;
            //alpha += step_alpha;

        }
        if (input_ch == 2) {
            sample -= step_sample;
            //alpha -= step_alpha;
        }

        std::cout << "Nombre d'echantillons : " << sample << " | Angle alpha : " << alpha << std::endl;

        // Actualiser les donnees
        setNewDatas(sample, alpha);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        // draw our first triangle
        glUseProgram(shaderProgram);
        glPointSize(1);

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

double function_calc(double x, double y){
    return 8 * exp(-((pow(x, 2) + pow(y, 2)) / 5)) * 100;
}

double Gradx(double x, double y, double delta) {
    return (function_calc(x + delta, y) - function_calc(x, y)) / delta;
}

double Grady(double x, double y, double delta) {
    return (function_calc(x, y + delta) - function_calc(x, y)) / delta;
}

double Vex(double x, double y, double alpha, double delta) {
    return cos(alpha) * Gradx(x, y, delta) - sin(alpha) * Grady(x, y, delta);
}

double Vey(double x, double y, double alpha, double delta) {
    return sin(alpha) * Gradx(x, y, delta) - cos(alpha) * Grady(x, y, delta);
}

double norme_vecteur(double vex, double vey) {
    return sqrt(pow(vex, 2) + pow(vey, 2));
}

void setNewDatas(int sample, double alpha) {
    //std::cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << std::endl;
    int x = 0;
    int y = 0;
    int next_x = 0;
    int next_y = 0;
    double delta = 0.0001;
    double coeff = 0.1;

    Data data[sample][sample];

    for (int i = 0; i < sample/10; i += 1) {
        y = 0;

        for (int j = 0; j < sample/10; j += 1) {             

            // Conversion de l'ensemble {0, sample} a l'ensemble {-5, 5}
            double x_data = (i * 10 - 0.5 * (double)sample) / (0.1 * (double)sample);
            double y_data = (j * 10 - 0.5 * (double)sample) / (0.1 * (double)sample);
            //float z_data = function_calc(x_data, y_data);

            // On stocke les sources dans la matrice
            data[x][y].x = x_data;
            data[x][y].y = y_data;
            data[x][y].z = 0.0f;

            //std::cout << "########## Nouvelle ligne de courant ##########" << std::endl;
            
            for (int k = 0; k < 10; k++) {

                // Calcul des vecteurs associes a la position
                double x_vecteur = Vex(x_data, y_data, alpha, delta);
                double y_vecteur = Vey(x_data, y_data, alpha, delta);

                // Normalisation du vecteur
                double norme = norme_vecteur(x_vecteur, y_vecteur);
                x_vecteur /= norme;
                y_vecteur /= norme;

                // Mise a jour du point courant
                double new_x_data = x_data + coeff * x_vecteur;
                double new_y_data = y_data + coeff * y_vecteur;
                
                //std::cout << "coord : " << x << " ; " << y << " | data : " << new_x_data << " ; " << new_y_data << std::endl; 
                //std::cout << "Vex : " << x_vecteur << " | Vey : " << y_vecteur << std::endl;

                // On stocke chaque point de la courbe dans la matrice
                data[x][y].x = new_x_data;
                data[x][y].y = new_y_data;
                data[x][y].z = 0.0f;

                // On actualise les donnees
                y += 1;
                
                x_data = new_x_data;
                y_data = new_y_data;
            
            }          
        }
        x += 1;
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