#pragma warning(disable:4996)
#include <glad.h>
#include <glfw3.h>
#include <stdio.h>
#include <math.h> 
#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>




/* ---------------------------------- SHADER --------------------------------*/
// Shader con soporte para texturas
const char* vertexShaderSource = "#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aNormal;\n"   // Asegúrate de tener esto para normales
"layout(location = 2) in vec2 aTexCoord;\n" // Coordenadas de textura
"   \n"
"out vec3 Normal;\n"
"out vec3 FragPos;\n"
"out vec2 TexCoord;\n"
"   \n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"   \n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"   FragPos = vec3(model * vec4(aPos, 1.0));\n"
"   Normal = mat3(transpose(inverse(model))) * aNormal;\n"
"   TexCoord = aTexCoord;\n"
"}";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 color;
uniform bool useTexture;
uniform sampler2D texture_diffuse;
uniform vec3 lightPos;   // Sol position
uniform vec3 lightColor; // Sol color
uniform bool lightEnabled; // Whether the sol is enabled

void main() {
    // Propiedades del material
    vec3 objectColor;
    if (useTexture) {
        objectColor = texture(texture_diffuse, TexCoord).rgb;
    } else {
        objectColor = color;
    }
    
    // Propiedades de iluminación
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    // Iluminación difusa
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Resultado final
    vec3 result;
    if (lightEnabled) {
        result = (ambient + diffuse) * objectColor;
    } else {
        result = (ambient) * objectColor; // Solo iluminación ambiente si la luz está apagada
    }
    
    FragColor = vec4(result, 1.0);
}
)";


/* ---------------------------------- SOl --------------------------------*/
const char* vertexShaderSourceSun = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSourceSun = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 sunColor;
uniform float sunIntensity;

void main() {
    // Calculate distance from center for circular gradient
    vec2 centeredCoord = 2.0 * gl_PointCoord - 1.0;
    float dist = length(centeredCoord);
    
    // Create a radial gradient for the sun
    float alpha = max(0.0, 1.0 - dist * dist);
    
    // Final sun color with glow effect
    FragColor = vec4(sunColor * sunIntensity, alpha);
}
)";


/* ---------------------------------- SKYBOX --------------------------------*/

// Vertex shader
const char* vertexShaderSourceSkybox =
R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    out vec3 texCoords;

    uniform mat4 projection;    
    uniform mat4 view;

    void main()
    {
        vec4 pos = projection * view * vec4(aPos, 1.0f);
        // Having z equal w will always result in a depth of 1.0f
        gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
        // We want to flip the z axis due to the different coordinate systems (left hand vs right hand)
        texCoords = vec3(aPos.x, aPos.y, -aPos.z);
    }    
)";

// Fragment shader
const char* fragmentShaderSourceSkybox =
R"(
    #version 330 core
    out vec4 FragColor;

    in vec3 texCoords;

    uniform samplerCube skybox;

    void main()
    {
        FragColor = texture(skybox, texCoords);
    }
)";













#pragma once
