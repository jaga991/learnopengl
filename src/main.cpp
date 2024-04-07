#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "glad.c"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include "shader_s.h"
#include "camera.h"

#include "Cube.h"
#include <vector>

#include "../collision/sat.h"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool wasRightMouseButtonPressed = false;


float floorVertices[] = {
    // positions          // texture Coords 
     5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

     5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
     5.0f, -0.5f, -5.0f,  2.0f, 2.0f
};

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


//store a list of cubes
std::vector<Cube*> cubeList;


//frame rate
const float desiredFrameTime = 1.0f / 60.0f; // 60 FPS




int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("Linking\\shader\\shader.vs", "Linking\\shader\\shader.fs");

    unsigned int floorVAO, floorVBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

	// load and create a texture
    // -------------------------
    


    Cube* cube1 = new Cube();
    cube1->SetPosition(glm::vec3(-2.0f,-1.3f,-1.3f));
    cube1->SetVelocity(glm::vec3(0.1f, 0.1f,0.1f));
    cube1->SetRotation(glm::vec3(45.0f,45.0f,45.0f));
    cubeList.push_back(cube1);

    Cube* cube2 = new Cube();
    cube2->SetPosition(glm::vec3(0.0f, 0.0, 0.0f));
    cube2->SetRotation(glm::vec3(0.0f,20.0f,0.0f));
    cubeList.push_back(cube2);


    unsigned int texture1, texture2;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* data = stbi_load("Linking\\texture\\wall.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("Linking\\texture\\awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
    // or set it via the texture class
    ourShader.setInt("texture2", 1);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        //for camera movement
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        processInput(window);

        //frame rate
        float frameStart = glfwGetTime();


        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // activate shader
        ourShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);
        glBindVertexArray(floorVAO);
        glm::mat4 model = glm::mat4(1.0f);
        ourShader.setMat4("model", model);

        //draw floor
        //glDrawArrays(GL_TRIANGLES, 0, 6);


        
        for (int i = 0; i < cubeList.size(); i++)
        {
            cubeList[i]->SetPosition(cubeList[i]->GetPosition() + cubeList[i]->GetVelocity() * deltaTime);
            // Draw the cube
            cubeList[i]->Draw(ourShader);
        }
        //check each pair of cube for collision
        for (int i = 0; i < cubeList.size(); i++) {

            for (int j = i + 1; j < cubeList.size(); j++)
            {
                //if distance of cube center is more than 3, skip the collision check
                if (glm::distance(cubeList[i]->GetPosition(), cubeList[j]->GetPosition()) > 3.0f) {
                    continue;
                }

                std::pair<bool, std::pair<glm::vec3, float>> result = areCubesColliding(*cubeList[i], *cubeList[j]);
                if (result.first)
                {
                    //get minimum translation vector and magnitude
                    glm::vec3 mtv = result.second.first;
                    float mtvMagnitude = result.second.second;

                    std::cout << "MTV: " << glm::to_string(mtv) << " Magnitude: " << mtvMagnitude << std::endl;
                    //print face normals of cube1
                    std::vector<glm::vec3> normals1 = cubeList[i]->GetFaceNormals();
                    std::cout << "Cube1 Face Normals: " << std::endl;
                    for (glm::vec3& normal : normals1) {
                        std::cout << glm::to_string(normal) << std::endl;
                    }

                    //print face normals of cube2
                    std::vector<glm::vec3> normals2 = cubeList[j]->GetFaceNormals();
                    std::cout << "Cube2 Face Normals: " << std::endl;
                    for (glm::vec3& normal : normals2) {
                        std::cout << glm::to_string(normal) << std::endl;
                    }
                    //check collision type
                    int collisionType = detectCollisionType(*cubeList[i], *cubeList[j], mtv, mtvMagnitude);
                    std::cout << "collition type: " << collisionType << std::endl;
                    //print collision type
                    if (collisionType == 1) {
						std::cout << "Face-Vertex Collision" << std::endl;
					}
					else if (collisionType == 2) {
						std::cout << "Edge-Edge Collision" << std::endl;
					}
                    //get contact point
                    glm::vec3 contactPoint = getContactPoint(*cubeList[i], *cubeList[j], collisionType, mtv, mtvMagnitude);
                    std::cout << "Contact Point: " << glm::to_string(contactPoint) << std::endl;
                    cubeList[i]->SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
                    cubeList[j]->SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
                }
                else
                {
                    cubeList[i]->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
                    cubeList[j]->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
                }
            }
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();


        // Frame limiting
        float frameEnd = glfwGetTime();
        float frameTime = frameEnd - frameStart; // Time taken for this frame
        if (frameTime < desiredFrameTime)
        {
            float timeToSleep = desiredFrameTime - frameTime; // Remaining time to reach desired frame time
            int timeToSleepInMs = static_cast<int>(timeToSleep * 1000);
            std::this_thread::sleep_for(std::chrono::milliseconds(timeToSleepInMs)); // Sleep for the remaining time
        }
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // 
    // ------------------------------------------------------------------
    for (int i = 0; i < cubeList.size(); i++)
    {
        delete cubeList[i];
    }
    cubeList.clear();

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    bool isRightMouseButtonPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    if (!wasRightMouseButtonPressed && isRightMouseButtonPressed)
    {
        // The right mouse button was just clicked
        Cube* newCube = new Cube();
        newCube->SetPosition(camera.Position);
        cubeList.push_back(newCube);
    }

    wasRightMouseButtonPressed = isRightMouseButtonPressed;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
    else
    {
        firstMouse = true;
    }


}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}