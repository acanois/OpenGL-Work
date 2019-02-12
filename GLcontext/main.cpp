//
//  main.cpp
//  GLcontext
//
//  Created by David Richter on 2/11/19.
//  Copyright © 2019 David Richter. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <SDL2/SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/glew.h>  // Has to be included first
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

/// Main rendering loop
void run(SDL_Window* window, Shader shaderProgram, GLuint vao, GLuint texture1, GLuint texture2, glm::vec3 cubePositions[])
{
    const int SCR_WIDTH = 1280;
    const int SCR_HEIGHT = 720;
    
    float rotationAngle = 0.0f;
    
    bool loop = true;
    while (loop)
    {
        SDL_Event event;
        SDL_PollEvent(&event);
        
        if (event.type == SDL_QUIT) loop = false;
        
        glClearColor(0.2f, 0.2f, 0.8f, 1.0f);
        // Clear colorbuffer bit for depth test
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        
        shaderProgram.use(); // Activate shader
        
        // create transformations
        glm::mat4 view          = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 projection    = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view       = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        // pass transformation matrices to the shader
        shaderProgram.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        shaderProgram.setMat4("view", view);

        // Render cubes
        glBindVertexArray(vao);
        
        for (auto i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            // float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(rotationAngle * (i/2.0f)), glm::vec3(1.0f, 0.3f, 0.5f));
            shaderProgram.setMat4("model", model);
            
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        SDL_GL_SwapWindow(window);
        
        rotationAngle += 0.1;
    }
}

/// Initialize main SDL window
SDL_Window* windowInit()
{
    SDL_Window* window = nullptr;
    SDL_Surface* surface = nullptr;
    
    // Initialize SDL
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        std::cout << "Failed to initialize SDL\n SDL Error: " << SDL_GetError() << std::endl;

    }
    else
    {
        // Create Window
        window = SDL_CreateWindow("SDL Test", 0, 0, 1280, 960, SDL_WINDOW_OPENGL);
        if (window == nullptr)
        {
            std::cout << "Failed to create window\n SDL Error: " << SDL_GetError() << std::endl;        }
        else
        {
            // Get window surface
            surface = SDL_GetWindowSurface(window);
        }
    }
    
    return window;
}

/// Initialize OpenGL
SDL_GLContext initGLContext(SDL_Window* window)
{
    // Set opengl version and use core profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    // Set up rendering context
    SDL_GLContext context = SDL_GL_CreateContext(window);
    
    return context;
}

/// Cleanup
void close(SDL_GLContext context, SDL_Window* window)
{
    std::cout << "Cleaning up..." << std::endl;

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/// Initialize GLEW
void initGLEW()
{
    glewExperimental = GL_TRUE;
    glewInit();
}

/// Main
int main(int argc, const char * argv[])
{
    SDL_Window* mainWindow = windowInit();
    SDL_GLContext mainContext = initGLContext(mainWindow);
    initGLEW();
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    
    /// Load Shaders
    const char* vertPath = "/Users/acanois/src/graphics/open_gl_stuff/GLcontext/GLcontext/shaders/vertShader.vert";
    const char* fragPath = "/Users/acanois/src/graphics/open_gl_stuff/GLcontext/GLcontext/shaders/fragShader.frag";
    
    /// Create shader
    Shader mainShader(vertPath, fragPath);
    
    /// Draw stuff
//    glClearColor(0.0, 0.0, 0.0, 1.0);
//    glClear(GL_COLOR_BUFFER_BIT);
//    SDL_GL_SwapWindow(mainWindow);
//
//    /// Create vertices and indices
//    float vertices[] = {
//        // positions          // texture coords
//        0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
//        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
//        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
//        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left
//    };
//    GLuint indices[] = {
//        0, 1, 3,  // first triangle
//        1, 2, 3   // second triange
//    };
    
    /// CUBE VERTICES
    /// ====================================================================
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    
    GLuint vbo, vao;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    /// WATCH THIS SECTION, IT CAN WRECK POSITIONING
    // Position attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coord attributes
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    //// GENERATING A TEXTURE
    //// ===========================================================
    GLuint texture1, texture2;
    
    /// Texture 1
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    
    // Set texture Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Sets nearest (sharper) filtering when textures get smaller, linear when magnified
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    //// Load and generate texture
    int width, height, numColorChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* image = stbi_load("/Users/acanois/src/graphics/open_gl_stuff/GLcontext/GLcontext/assets/container.jpg", &width, &height, &numColorChannels, 0);
    if (image)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Texture1 did not load correctly!" << std::endl;
    }
    stbi_image_free(image); // Clear the image data
    /// Texture 2
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    image = stbi_load("/Users/acanois/src/graphics/open_gl_stuff/GLcontext/GLcontext/assets/awesomeface.png", &width, &height, &numColorChannels, 0);
    if (image)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Texture2 did not load correctly!" << std::endl;
    }
    
    mainShader.use();
    mainShader.setInt("texture1", 0);
    mainShader.setInt("texture2", 1);
    
    run(mainWindow, mainShader, vao, texture1, texture2, cubePositions);
    
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    
    close(mainContext, mainWindow);
    
    return 0;
}
