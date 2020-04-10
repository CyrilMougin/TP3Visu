#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <math.h>

using namespace std;

void construct_points(float pasEchantillonage);

void setNewDatas_source(int sample);
void setNewDatas_powerline(int sample, double alpha, int number_point);

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

bool s_is_press = false;
bool S_is_press = false;
bool mod = false;
bool a_is_press = false;
bool space_is_press = false;
bool add_is_press = false;
bool subtract_is_press = false;

typedef struct
{
    GLfloat x, y, z;
} Data;

unsigned int VBO,VBO_colors, VAO;


const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"

"uniform float offset_x;\n"
"uniform float scale_x;\n"

"void main()\n"
"{\n"
"   gl_Position = vec4((aPos.x + 0.0f) / scale_x, (aPos.y + 0.0f) / scale_x, aPos.z, 1.0);\n"

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
    int sample = 10;
    int step_sample = 2;

    // Angle alpha
    double alpha = M_PI / 2;
    double step_alpha = M_PI / 5;

    // Nombre de points pour les lignes de courant
    int number_point = 5;
    int step_point = 1;

    // Point de vue
    int view = 0;

    // Ajuster les différents paramètres
    int input_ch;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // Input
        input_ch = processInput(window);

        if (input_ch == 1) {
            number_point += step_point;

        }
        if (input_ch == 2) {
            number_point -= step_point;
            
        }
        if (input_ch == 3) {
            sample += step_sample;

        }
        if (input_ch == 4) {
            sample -= step_sample;

        }
        if (input_ch == 5) {
            alpha += step_alpha; 
        }

        std::cout << "Nb echantillons : " << sample << " | Angle : " << alpha <<  " | Nb points : " << number_point << " | Mode : " << view << std::endl;

        // Actualiser les donnees
        if (input_ch == 6) {
            if (view < 2) {
                view += 1;
            }else {
                view = 0;
            }
        }

        if (view == 0) {
            setNewDatas_source(sample);
        }else if (view == 1) {
            setNewDatas_powerline(sample, alpha, number_point);
        }else {
            // A FAIRE
        }

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        // draw our first triangle
        glUseProgram(shaderProgram);
        glPointSize(1);

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, 1000000);

        glDisableVertexAttribArray(0);

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
        std::cout << "C'est plus 'add'" << std::endl;
        add_is_press = true;
        return 1;
    }

    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS && !subtract_is_press) {
        // Cas du '-'
        std::cout << "C'est moins 'subtract'" << std::endl;
        subtract_is_press = true;
        return 2;
    }

    // Detecter si une des touches 'LEFT_SHIFT', 'RIGHT_SHIFT' ou 'CAPS_LOCK' est enfonce
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS) {
        mod = true;
    }else {
        mod = false;
    }

    // 'Debloquer' les touches 's' et 'S'
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE && S_is_press) {
        if (mod) {
            std::cout << "Ca release 'S'" << std::endl;
            S_is_press = false;   
        }
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE && s_is_press) {
        if (!mod) {
            std::cout << "Ca release 's'" << std::endl;
            s_is_press = false;
        }
    }

    // 'Bloquer' les touches 's' et 'S' & Envoie du signal
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !S_is_press) {
        if (mod) {
            std::cout << "C'est plus 'S'" << std::endl;
            S_is_press = true;
            return 3; 
        }
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !s_is_press) {
        if (!mod) {
            std::cout << "C'est moins 's'" << std::endl;
            s_is_press = true;
            return 4;
        }
    }

    // 'Debloquer' la touche 'a'
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE && a_is_press) {
        std::cout << "Ca release 'a'" << std::endl;
        a_is_press = false;
    }

    // 'Bloquer' la touche 'a' & Envoie du signal
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !a_is_press) {
        std::cout << "C'est plus 'a'" << std::endl;
        a_is_press = true;
        return 5;
    }

    // 'Debloquer' la touche 'space'
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && space_is_press) {
        std::cout << "Ca release 'space'" << std::endl;
        space_is_press = false;
    }

    // 'Bloquer' la touche 'space' & Envoie du signal
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !space_is_press) {
        std::cout << "C'est plus 'space'" << std::endl;
        space_is_press = true;
        return 6;
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

void setNewDatas_source(int sample) {
    int x = 0;
    int y = 0;

    Data data[sample][sample];

    for (int i = 0; i < sample; i += 1) {
        y = 0;

        for (int j = 0; j < sample; j += 1) {

            // Conversion de l'ensemble {0, sample} a l'ensemble {-5, 5}
            double x_data = (i - 0.5 * (double)sample) / (0.1 * (double)sample);
            double y_data = (j - 0.5 * (double)sample) / (0.1 * (double)sample);
            //float z_data = function_calc(x_data, y_data);

            // On stocke les sources dans la matrice
            data[x][y].x = x_data;
            data[x][y].y = y_data;
            data[x][y].z = 0.0f;

            // On actualise les donnees
            y += 1;
        
        }
        x += 1;
    } 

    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    GLuint sourcebuffer;
	glGenBuffers(1, &sourcebuffer);
	glBindBuffer(GL_ARRAY_BUFFER, sourcebuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

}

void setNewDatas_powerline(int sample, double alpha, int number_point) {
    //std::cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << std::endl;
    int x = 0;
    int y = 0;
    int next_x = 0;
    int next_y = 0;
    double delta = 0.0001;
    double coeff = 0.1;

    Data data[sample][sample * number_point];

    for (int i = 0; i < sample; i += 1) {
        y = 0;

        for (int j = 0; j < sample; j += 1) {             

            // Conversion de l'ensemble {0, sample} a l'ensemble {-5, 5}
            double x_data = (i - 0.5 * (double)sample) / (0.1 * (double)sample);
            double y_data = (j - 0.5 * (double)sample) / (0.1 * (double)sample);
            //float z_data = function_calc(x_data, y_data);

            // On stocke les sources dans la matrice
            data[x][y].x = x_data;
            data[x][y].y = y_data;
            data[x][y].z = 0.0f;

            //std::cout << "########## Nouvelle ligne de courant ##########" << std::endl;
            
            for (int k = 0; k < number_point; k++) {

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

    glBindVertexArray(VAO);

    GLuint powerlinebuffer;
	glGenBuffers(1, &powerlinebuffer);
	glBindBuffer(GL_ARRAY_BUFFER, powerlinebuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

}