#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include <iostream>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

GLuint VAO, VBO, EBO, FBO, RBO, texture;
GLFWwindow* window = NULL;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int initFrameworks() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Ejemplo RBO - Plano Azul + HUD", NULL, NULL);
    if (!window) {
        std::cerr << "Error al crear la ventana GLFW\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error al inicializar GLEW\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    return 0;
}

void initGeometry()
{

    // --- Vértices del plano ---
    float planeVertices[] = {
        // posiciones         // texcoords
        -5.0f, 0.0f, -5.0f,   0.0f, 0.0f,
         5.0f, 0.0f, -5.0f,   1.0f, 0.0f,
         5.0f, 0.0f,  5.0f,   1.0f, 1.0f,
        -5.0f, 0.0f,  5.0f,   0.0f, 1.0f
    };
    unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

}

int initRenderBufferObject() {

    //Activamos cual framebuffer vamos a ligar 
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Creamos una textura donde guardar la información del renderizado
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);


    //Se crea un framebuffer object para renderizar
    // Este Framebuffer puede escribir y leer pixeles de algún lugar
    glGenFramebuffers(1, &RBO);
    //Utilizamos el FBO
    glBindFramebuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, WIDTH, HEIGHT);
    //Asignamos nuestro Framebuffer al buffer de renderizado RBO que ya tiene asignada la textura
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, RBO);

    //Verificamos que todo se asigne correctamente
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        return -1;
    }
       //Se quita el framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return 0;
}

int main() {
    
    if (initFrameworks() < 0)
        return -1;
    initGeometry();
    if(initRenderBufferObject() < 0)
        return -1;

    Shader shader("./shaders/plane.vert", "./shaders/plane.frag");
    Shader shader2("./shaders/render.vert", "./shaders/render.frag");

    // --- Cámara básica ---
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(10.0f,10.0f, 10.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 proj = glm::perspective(glm::radians(45.0f),
        (float)WIDTH / HEIGHT,
        0.1f, 100.0f);

    // --- Render loop ---
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        std::cout << deltaTime << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        /*Pintar la escena principal*/
        shader.use();
        shader.setMat4("uModel", &model[0][0]);
        shader.setMat4("uView", &view[0][0]);
        shader.setMat4("uProj", &proj[0][0]);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader2.use();
        shader2.setMat4("uModel",&model[0][0]);
        shader2.setMat4("uView", &view[0][0]);
        shader2.setMat4("uProj", &proj[0][0]);
        shader2.setInt("screenTexture", 0);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        /*Pintar el RenderBufferObject*/

        glfwSwapBuffers(window);
    }

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glfwTerminate();
    return 0;
}
