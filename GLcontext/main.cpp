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
void run(SDL_Window* window, GLuint shaderProgram, GLuint vao)
{
    bool loop = true;
    
    while (loop)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) loop = false;
            
            glClearColor(0.2f, 0.2f, 0.8f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            glUseProgram(shaderProgram);
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            
            SDL_GL_SwapWindow(window);
        }
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
    
    /// Load Shaders
    const char* vertPath = "/Users/acanois/src/graphics/open_gl_stuff/GLcontext/GLcontext/shaders/vertShader.vert";
    const char* fragPath = "/Users/acanois/src/graphics/open_gl_stuff/GLcontext/GLcontext/shaders/fragShader.frag";
    
    std::string vertSource;
    std::string fragSource;
    std::ifstream vertFile;
    std::ifstream fragFile;
    
    vertFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        // open files
        vertFile.open(vertPath);
        fragFile.open(fragPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vertFile.rdbuf();
        fShaderStream << fragFile.rdbuf();
        // close file handlers
        vertFile.close();
        fragFile.close();
        // convert stream into string
        vertSource   = vShaderStream.str();
        fragSource   = fShaderStream.str();
    }
    catch (std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    
    // Load shader will return these
    const char* vShaderCode = vertSource.c_str();
    const char* fShaderCode = fragSource.c_str();
    /// End shader loading
    
    /// Draw stuff
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(mainWindow);
    
    /// SHADER COMPILATION
    // For error check logging
    int success;
    char infoLog[512];
    
    // Vertex Shader
    GLuint vertexShader;
    // vertexShader = glCreateShader(GL_VERTEX_SHADER);
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // Fragment Shader
    GLuint  fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // Shader Program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram(); // Creates a new program and returns the ID reference
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    /// Create vertices and indices
    float vertices[] = {
        // positions         // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,    // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,    // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
    };
    GLuint indices[] = {
        0, 1, 3,  // first triangle
        1, 2, 3   // second triange
    };
    
    GLuint vbo, vao, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Position attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attributes
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Texture coord attributes
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    //// GENERATING A TEXTURE
    //// ===========================================================
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // Set texture Parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    
    // Sets nearest (sharper) filtering when textures get smaller, linear when magnified
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Set mipmapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    
    //// Only needed to specify a border color when using GL_CLAMP_TO_EDGE
    // float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    //// Load and generate the texture
    int width, height, numColorChannels;
    unsigned char* image = stbi_load("/Users/acanois/src/graphics/sdl_stuff/sdl_test/sdl_test/assets/container.jpg", &width, &height, &numColorChannels, 0);
    if (image)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Texture did not load correctly!" << std::endl;
    }
    stbi_image_free(image); // Clear the image data
    
    run(mainWindow, shaderProgram, vao);
    
    close(mainContext, mainWindow);
    
    return 0;
}
