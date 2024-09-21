#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLM/gtc/type_ptr.hpp>
#include <iostream>
#include "shader.h"
#include "camera.h"
#include "model.h"

#define GLEW_STATIC

using namespace std;

//for planetary revolution
bool revolve;

//for cursor visibility
bool cursorVisible;

// Screen width and height for windowed mode
bool isFullscreen = false;
int windowPosX;
int windowPosY;
GLint WIDTH = 1400, HEIGHT = 800;

// Camera
Camera camera(glm::vec3(9.0f, 0.0f, 20.0f));

//keycallback variables
bool keys[1024];
string cameraType = "";

//mousecallback variables
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

// Light attribute
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
glm::mat4 projection(1);

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat speed = 1.0f;
const double PI = (22 / 7);

//callback functions
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

//other functions
void ToggleFullscreen(GLFWwindow* window);
void DoMovement();
void showMenu();

int main() {

    showMenu();
    revolve = true;
    cursorVisible = false;

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "The solar system", nullptr, nullptr);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);

    Shader* shader = new Shader("resources/shaders/modelLoading.vs", "resources/shaders/modelLoading.frag");
    Shader* directionalShader = new Shader("resources/shaders/directional.vs", "resources/shaders/directional.frag");
    Shader* lampShader = new Shader("resources/shaders/lamp.vs", "resources/shaders/lamp.frag");

    // Load models
    Model* earthModel = new Model((GLchar*)"resources/models/earth/Earth.obj");
    Model* space = new Model((GLchar*)"resources/models/space/space.obj");
    Model* sunModel = new Model((GLchar*)"resources/models/sun/sun.obj");
    Model* mercuryModel = new Model((GLchar*)"resources/models/mercury/mercury.obj");
    Model* venusModel = new Model((GLchar*)"resources/models/venus/venus.obj");
    Model* marsModel = new Model((GLchar*)"resources/models/mars/mars.obj");
    Model* jupiterModel = new Model((GLchar*)"resources/models/jupiter/jupiter.obj");
    Model* saturnModel = new Model((GLchar*)"resources/models/saturn/13906_Saturn_v1_l3.obj");
    Model* uranusModel = new Model((GLchar*)"resources/models/uranus/13907_Uranus_v2_l3.obj");
    Model* neptuneModel = new Model((GLchar*)"resources/models/neptune/13908_Neptune_v2_l3.obj");

    projection = glm::perspective(camera.GetZoom(), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

    double radii[8] = {70.0f, 80.0f, 90.0f, 110.0f, 140.0f, 180.0f, 210.0f, 240.0f};

    GLfloat scale = 0.1f,  angle;
    GLuint i = 0;
    while (!glfwWindowShouldClose(window))
    {
        if (!cursorVisible) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        i++;
        glfwPollEvents();
        DoMovement();
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view(1);
        view = camera.GetViewMatrix();

        directionalShader->Use();

        // Set lights properties
        glUniform3f(glGetUniformLocation(directionalShader->Program, "light.position"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(directionalShader->Program, "light.ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(directionalShader->Program, "light.diffuse"), 1.5f, 1.5f, 1.5f);
        glUniform3f(glGetUniformLocation(directionalShader->Program, "light.specular"), 0.0f, 0.0f, 0.0f);

        glUniformMatrix4fv(glGetUniformLocation(directionalShader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(directionalShader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Model

        // SPACE
        glm::mat4 model(1);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(20.0f));
        glUniformMatrix4fv(glGetUniformLocation(directionalShader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        space->Draw(*directionalShader);

        GLfloat radius, x, y;

        shader->Use();
        GLint viewPosLoc = glGetUniformLocation(shader->Program, "viewPos");
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

        // Set lights properties
        glUniform3f(glGetUniformLocation(shader->Program, "light.position"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(shader->Program, "light.ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(shader->Program, "light.diffuse"), 1.5f, 1.5f, 1.5f);
        glUniform3f(glGetUniformLocation(shader->Program, "light.specular"), 0.0f, 0.0f, 0.0f);
        glUniform1f(glGetUniformLocation(shader->Program, "light.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(shader->Program, "light.linear"), 0.02f);
        glUniform1f(glGetUniformLocation(shader->Program, "light.quadratic"), 0.006f);

        glUniformMatrix4fv(glGetUniformLocation(shader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // MERCURY
        model = glm::mat4(1);
        if (revolve) {
            angle = 0.008f * i * speed;
            radius = radii[0] * scale;
            x = radius * sin(PI * 2 * angle / 360);
            y = radius * cos(PI * 2 * angle / 360);
            model = glm::translate(model, glm::vec3(x, 0.0f, y));
        }
        else {
            model = glm::translate(model, glm::vec3(70.0f * scale, 0.0f, 0.0f));
        }

        if (cameraType == "Mercury") {
            camera.SetPosition(glm::vec3(x + 0.5f, 0.0f, y + 0.5f));
        }

        model = glm::scale(model, glm::vec3(0.3f * scale));
        angle = 0.001f * i;
        model = glm::rotate(model, angle, glm::vec3(0.0f, 0.1f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        mercuryModel->Draw(*shader);

        // VENUS
        model = glm::mat4(1);
        if (revolve) {
            angle = 0.007f * i * speed;
            radius = radii[1] * scale;
            x = radius * sin(PI * 2 * angle / 360);
            y = radius * cos(PI * 2 * angle / 360);
            model = glm::translate(model, glm::vec3(x, 0.0f, y));
        }
        else {
            model = glm::translate(model, glm::vec3(80.0f * scale, 0.0f, 0.0f));
        }

        if (cameraType == "Venus") {
            camera.SetPosition(glm::vec3(x + 0.5f, 0.0f, y + 0.5f));
        }

        model = glm::scale(model, glm::vec3(0.5f * scale));
        angle = 0.001f * i;
        model = glm::rotate(model, angle, glm::vec3(0.0f, 0.1f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        venusModel->Draw(*shader);

        // EARTH
        model = glm::mat4(1);
        if (revolve) {
            angle = 0.006f * i * speed;
            radius = radii[2] * scale;
            x = radius * sin(PI * 2 * angle / 360);
            y = radius * cos(PI * 2 * angle / 360);
            model = glm::translate(model, glm::vec3(x, 0.0f, y));
        }
        else {
            model = glm::translate(model, glm::vec3(90.0f * scale, 0.0f, 0.0f));
        }

        if (cameraType == "Earth") {
            camera.SetPosition(glm::vec3(x + 0.5f, 0.0f, y + 0.5f));
        }

        model = glm::scale(model, glm::vec3(0.5f * scale));
        angle = 0.001f * i;
        model = glm::rotate(model, angle, glm::vec3(0.0f, 0.1f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        earthModel->Draw(*shader);

        // MARS
        model = glm::mat4(1);
        if (revolve) {
            angle = 0.005f * i * speed;
            radius = radii[3] * scale;
            x = radius * sin(PI * 2 * angle / 360);
            y = radius * cos(PI * 2 * angle / 360);
            model = glm::translate(model, glm::vec3(x, 0.0f, y));
        }
        else {
            model = glm::translate(model, glm::vec3(100.0f * scale, 0.0f, 0.0f));
        }

        if (cameraType == "Mars") {
            camera.SetPosition(glm::vec3(x + 0.5f, 0.0f, y + 0.5f));
        }

        model = glm::scale(model, glm::vec3(0.3f * scale));
        angle = 0.0005f * i;
        model = glm::rotate(model, angle, glm::vec3(0.0f, 0.1f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        marsModel->Draw(*shader);

        // JUPITER
        model = glm::mat4(1);
        if (revolve) {
            angle = 0.0045f * i * speed;
            radius = radii[4] * scale;
            x = radius * sin(PI * 2 * angle / 360);
            y = radius * cos(PI * 2 * angle / 360);
            model = glm::translate(model, glm::vec3(x, 0.0f, y));
        }
        else {
            model = glm::translate(model, glm::vec3(120.0f * scale, 0.0f, 0.0f));
        }

        if (cameraType == "Jupiter") {
            camera.SetPosition(glm::vec3(x + 0.5f, 0.0f, y + 2.5f));
        }

        model = glm::scale(model, glm::vec3(4.0f * scale));
        angle = 0.0005f * i;
        model = glm::rotate(model, angle, glm::vec3(0.0f, 0.1f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        jupiterModel->Draw(*shader);

        // SATURN
        model = glm::mat4(1);
        if (revolve) {
            angle = 0.0040f * i * speed;
            radius = radii[5] * scale;
            x = radius * sin(PI * 2 * angle / 360);
            y = radius * cos(PI * 2 * angle / 360);
            model = glm::translate(model, glm::vec3(x, 0.0f, y));
        }
        else {
            model = glm::translate(model, glm::vec3(160.0f * scale, 0.0f, 0.0f));
        }

        if (cameraType == "Saturn") {
            camera.SetPosition(glm::vec3(x + 0.5f, 0.0f, y + 2.5f));
        }

        model = glm::scale(model, glm::vec3(0.032f * scale));
        angle = 0.0001f * i;
        model = glm::rotate(model, 90.0f + angle, glm::vec3(0.0f, 0.1f, 0.5f));
        glUniformMatrix4fv(glGetUniformLocation(shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        saturnModel->Draw(*shader);

        // Uranus
        model = glm::mat4(1);
        if (revolve) {
            angle = 0.0035f * i * speed;
            radius = radii[6] * scale;
            x = radius * sin(PI * 2 * angle / 360);
            y = radius * cos(PI * 2 * angle / 360);
            model = glm::translate(model, glm::vec3(x, 0.0f, y));
        }
        else {
            model = glm::translate(model, glm::vec3(190.0f * scale, 0.0f, 0.0f));
        }

        if (cameraType == "Uranus") {
            camera.SetPosition(glm::vec3(x + 0.5f, 0.0f, y + 2.0f));
        }

        model = glm::scale(model, glm::vec3(0.03f * scale));
        angle = 0.00001f * i;
        model = glm::rotate(model, 160.0f + angle, glm::vec3(0.0f, 0.1f, 0.5f));
        glUniformMatrix4fv(glGetUniformLocation(shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        uranusModel->Draw(*shader);

        // NEPTUNE
        model = glm::mat4(1);
        if (revolve) {
            angle = 0.003f * i * speed;
            radius = radii[7] * scale;
            x = radius * sin(PI * 2 * angle / 360);
            y = radius * cos(PI * 2 * angle / 360);
            model = glm::translate(model, glm::vec3(x, 0.0f, y));
        }
        else {
            model = glm::translate(model, glm::vec3(220.0f * scale, 0.0f, 0.0f));
        }

        if (cameraType == "Neptune") {
            camera.SetPosition(glm::vec3(x + 0.5f, 0.0f, y + 2.0f));
        }

        model = glm::scale(model, glm::vec3(0.03f * scale));
        angle = 0.00001f * i;
        model = glm::rotate(model, 130.0f + angle, glm::vec3(0.0f, 0.1f, 0.5f));
        glUniformMatrix4fv(glGetUniformLocation(shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        neptuneModel->Draw(*shader);

        // SUN
        lampShader->Use();
        GLint modelLoc = glGetUniformLocation(lampShader->Program, "model");
        GLint viewLoc = glGetUniformLocation(lampShader->Program, "view");
        GLint projLoc = glGetUniformLocation(lampShader->Program, "projection");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        model = glm::mat4(1);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(20.0f * scale));
        glUniformMatrix4fv(glGetUniformLocation(shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        sunModel->Draw(*lampShader);

        if (cameraType == "Up") {
            camera.SetPosition(glm::vec3(0, 40, 0));
        }

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void showMenu() {
    std::cout << "Menu Options:\n\n";

    std::cout << "Control revolution speed of planets:\n";
    std::cout << "  Actions:\n";
    std::cout << "  Increase speed: Press M.\n";
    std::cout << "  Decrease speed: Press N.\n\n";

    std::cout << "Control Simulation Speed:\n";
    std::cout << "  Increase or decrease the simulation speed.\n";
    std::cout << "  Actions:\n";
    std::cout << "    Increase speed: Press +. \n";
    std::cout << "    Decrease speed: Press -. \n\n";

    std::cout << "Change Camera View:\n";
    std::cout << "  Switch between different planet cameras for an immersive experience.\n";
    std::cout << "  Actions:\n";
    std::cout << "    View Mercury: Press 1.\n";
    std::cout << "    View Venus:   Press 2.\n";
    std::cout << "    View Earth:   Press 3.\n";
    std::cout << "    View Mars:    Press 4.\n";
    std::cout << "    View Jupiter: Press 5.\n";
    std::cout << "    View Saturn:  Press 6.\n";
    std::cout << "    View Uranus:  Press 7.\n";
    std::cout << "    View Neptune: Press 8.\n";
    std::cout << "    Free Camera:  Press 0.\n\n";

    std::cout << "Pause and Resume Revolution Simulation:\n";
    std::cout << "  Actions: Press P.\n\n";

    std::cout << "Toggle cursor visibility\n";
    std::cout << "  Actions: Press X.\n";

    std::cout << "Toggle Fullscreen:\n";
    std::cout << "  Action: Press F11.\n\n";

    std::cout << "Exit the Simulation:\n";
    std::cout << "  Action: Press ESC.\n\n\n";
}


void DoMovement() {

    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (keys[GLFW_KEY_MINUS]) {
        camera.DecreaseSpeed();
    }

    if (keys[GLFW_KEY_EQUAL]) {
        camera.IncreaseSpeed();
    }

    if (keys[GLFW_KEY_1]) {
        cameraType = "Mercury";
    }
    else if (keys[GLFW_KEY_2]) {
        cameraType = "Venus";
    }
    else if (keys[GLFW_KEY_3]) {
        cameraType = "Earth";
    }
    else if (keys[GLFW_KEY_4]) {
        cameraType = "Mars";
    }
    else if (keys[GLFW_KEY_5]) {
        cameraType = "Jupiter";
    }
    else if (keys[GLFW_KEY_6]) {
        cameraType = "Saturn";
    }
    else if (keys[GLFW_KEY_7]) {
        cameraType = "Uranus";
    }
    else if (keys[GLFW_KEY_8]) {
        cameraType = "Neptune";
    }
    else if (keys[GLFW_KEY_0]) {
        cameraType = "";
    }
    else if (keys[GLFW_KEY_U]) {
        cameraType = "Up";
    } 
    else if (keys[GLFW_KEY_P]) {
        if (speed == 0) {
            speed = 0.5f;
        }
        else {
            speed = 0;
        }
    }
    else if (keys[GLFW_KEY_M]) {
        if (speed <= 100)
        speed = speed + 0.1f;
    }
    else if (keys[GLFW_KEY_N]) {
        if (speed >= 0.1f)
        speed = speed - 0.1f;
    }
    else if (keys[GLFW_KEY_X]) {
        cursorVisible = !cursorVisible;
    }
}

void ToggleFullscreen(GLFWwindow* window) {
    if (isFullscreen) {
        glfwSetWindowMonitor(window, nullptr, windowPosX, windowPosY, WIDTH, HEIGHT, 0);
    }
    else {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwGetWindowPos(window, &windowPosX, &windowPosY);
        glfwGetWindowSize(window, &WIDTH, &HEIGHT);

        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    isFullscreen = !isFullscreen;
}


void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
        ToggleFullscreen(window);
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            keys[key] = false;
        }
    }
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos) {
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    WIDTH = width;
    HEIGHT = height;

    glViewport(0, 0, WIDTH, HEIGHT);
    glViewport(0, 0, WIDTH, HEIGHT);

    projection = glm::perspective(camera.GetZoom(), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
}