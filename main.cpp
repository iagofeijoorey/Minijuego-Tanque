/*------------------------ INCLUDES ------------------------*/
#include <glad.h>
#include <glfw3.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



#include "esfera.h"             //Vertices de la esfera
#include "presets.h"            //Valores predefinidos
#include "shaders.h"            //Shaders


// STB Image para cargar texturas
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//      OPCIONES
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 800;

// Variables para mostrar los FPS
double prevTime = 0.0;
double crntTime = 0.0;
double timeDiff;
unsigned int counter = 0;       // Mantiene un seguimiento de la cantidad de frames en el timeDiff


/*------------------------ VARIABLES NECESARIAS PARA EL MANEJO DEL PROGRAMA ------------------------*/   
bool verEjes = false;
bool camaraLibre = false;
bool terceraPersona = true;
bool primeraPersona = false;


/*------------------------ VARIABLES PARA EL MOVIMIENTO DEL TANQUE ------------------------*/
float velocidadTanque = 0.05f;
float rotacionTanque = 0.0f;

float posicionTanqueX = 0.0f;
float posicionTanqueZ = 0.0f;

float posicionRuedaIzqX = 0.0f;
float posicionRuedaIzqZ = 0.0f;

float posicionRuedaDerX = 0.0f;
float posicionRuedaDerZ = 0.0f;

float rotacionTorreta = 0.0f;
float giroTorreta = 0.0f;
float giroCamara3Persona = 0.0f;
float rotacionCanhon = 90.0f;
float inclinacionCanon = 90.0f;

float rotacionCintas = 90.0f;
float rotacionRuedas = 0.0f;
float giroRuedas = 0.0f;

float radioCirucloTanque = ESCALA_COCHE.x / 2.0f;

float rotacionTorretaEnemiga = 0.0f;






/*------------------------ VARIABLES PARA LA ILUMINACION ------------------------*/
glm::vec3 lightPos(0, 50, -30);                       // Posición de la luz
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);             // Color de la luz (blanca)
bool lightEnabled = true;                           // Control para encender/apagar




/*------------------------ SHADERS Y TEXTURAS ------------------------*/
GLuint shaderProgram, skyboxShader, shadowShader;
unsigned int cubemapTexture;

unsigned int generarShaders(const char* vertexShaderInput, const char* fragmentShaderInput) {
    unsigned int vertexShader, fragmentShader, shaderProgram;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderInput, NULL);
    glCompileShader(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderInput, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

unsigned int cargarTextura(const char* rutaArchivo) {
    unsigned int texturaID;
    glGenTextures(1, &texturaID);

    // Vincular textura antes de cargar
    glBindTexture(GL_TEXTURE_2D, texturaID);

    // Establecer parámetros de textura 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Invertir las imágenes en el eje Y
    stbi_set_flip_vertically_on_load(true);

    int ancho, alto, nrCanales;

    // Intentar cargar la textura desde la ruta proporcionada
    unsigned char* datos = stbi_load(rutaArchivo, &ancho, &alto, &nrCanales, 0);
    std::string rutaUtilizada = rutaArchivo;

    // Si falla, intentar desde la carpeta textures/
    if (!datos) {
        std::string rutaAlternativa = "textures/" + std::string(rutaArchivo);
        datos = stbi_load(rutaAlternativa.c_str(), &ancho, &alto, &nrCanales, 0);
        rutaUtilizada = rutaAlternativa;

        // Si aún falla, intentar con solo el nombre del archivo
        if (!datos) {
            std::string nombreArchivo = std::string(rutaArchivo);
            size_t ultimaBarra = nombreArchivo.find_last_of("/\\");
            if (ultimaBarra != std::string::npos) {
                nombreArchivo = nombreArchivo.substr(ultimaBarra + 1);
            }
            rutaAlternativa = "textures/" + nombreArchivo;
            datos = stbi_load(rutaAlternativa.c_str(), &ancho, &alto, &nrCanales, 0);
            rutaUtilizada = rutaAlternativa;
        }
    }

    if (datos) {
        GLenum formato;
        GLenum formatoInterno;

        if (nrCanales == 1) {
            formato = GL_RED;
            formatoInterno = GL_RED;
        }
        else if (nrCanales == 3) {
            formato = GL_RGB;
            formatoInterno = GL_RGB;
        }
        else if (nrCanales == 4) {
            formato = GL_RGBA;
            formatoInterno = GL_RGBA;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, formatoInterno, ancho, alto, 0, formato, GL_UNSIGNED_BYTE, datos);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(datos);
        std::cout << "✅ Textura cargada correctamente: " << rutaUtilizada
            << " (" << ancho << "x" << alto << ", " << nrCanales << " canales)" << std::endl;

        // Comprueba errores después de cargar
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cout << "⚠️ Error de OpenGL después de cargar textura: " << error << std::endl;
        }
    }
    else {
        std::cout << "❌ ERROR: No se pudo cargar la textura: " << rutaArchivo << std::endl;
        std::cout << "  Razón del error: " << stbi_failure_reason() << std::endl;
        std::cout << "  Rutas intentadas: " << rutaArchivo << ", " << rutaUtilizada << std::endl;

        // Generar una textura de fallback colorida de 2x2 pixels
        unsigned char fallbackTextura[] = {
            255, 0, 255, 255,   0, 255, 0, 255,
            0, 0, 255, 255,     255, 255, 0, 255
        };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, fallbackTextura);
        glGenerateMipmap(GL_TEXTURE_2D);

        std::cout << "  Se ha generado una textura de reemplazo (patrón de verificación)" << std::endl;
    }

    return texturaID;
}

void verificarEstadoTextura(unsigned int texturaID, const char* nombreTextura) {
    bool isTexture = glIsTexture(texturaID);

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Verificando textura: " << nombreTextura << std::endl;
    std::cout << "ID de textura: " << texturaID << std::endl;
    std::cout << "Es una textura valida? " << (isTexture ? "SI" : "NO") << std::endl;

    if (isTexture) {
        int width, height;
        glBindTexture(GL_TEXTURE_2D, texturaID);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

        std::cout << "Dimensiones: " << width << "x" << height << std::endl;

        // Verificar parámetros de textura
        int wrapS, wrapT, minFilter, magFilter;
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapS);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapT);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);

        std::cout << "WRAP_S: " << wrapS << " (GL_REPEAT=" << GL_REPEAT << ")" << std::endl;
        std::cout << "WRAP_T: " << wrapT << " (GL_REPEAT=" << GL_REPEAT << ")" << std::endl;
        std::cout << "MIN_FILTER: " << minFilter << " (GL_LINEAR_MIPMAP_LINEAR=" << GL_LINEAR_MIPMAP_LINEAR << ")" << std::endl;
        std::cout << "MAG_FILTER: " << magFilter << " (GL_LINEAR=" << GL_LINEAR << ")" << std::endl;
    }

    // Verificar errores de OpenGL
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error de OpenGL: " << error << std::endl;
    }
    else {
        std::cout << "No hay errores de OpenGL" << std::endl;
    }

    std::cout << "----------------------------------------" << std::endl;
}



/*------------------------ CREAR OBJETOS ------------------------*/
unsigned int VAOEjes;
unsigned int VAOCubo;
unsigned int VAOPlano;
unsigned int VAOCilindro;
unsigned int VAOSkybox;


void crearPlano() {
    unsigned int VBO, EBO;

    // Vértices de un plano simple con normales y coordenadas de textura
    float vertices[] = {
        // Posiciones          // Normales             // Coordenadas de textura
        -10.0f, 0.0f, -10.0f,  0.0f, 1.0f, 0.0f,      0.0f, 0.0f,
         10.0f, 0.0f, -10.0f,  0.0f, 1.0f, 0.0f,      30, 0.0f,
         10.0f, 0.0f,  10.0f,  0.0f, 1.0f, 0.0f,      30, 30,
        -10.0f, 0.0f,  10.0f,  0.0f, 1.0f, 0.0f,      0.0f, 30
    };

    unsigned int indices[] = {
        0, 2, 1,
        0, 3, 2
    };

    glGenVertexArrays(1, &VAOPlano);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAOPlano);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Posición - atributo 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normales - atributo 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Coordenadas de textura - atributo 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void crearEjes() {
    unsigned int VBO, EBO;
    float vertices[] = {
        //Vertices           //Colores
        0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f, // origen
        5.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f, // x
        0.0f, 5.0f, 0.0f,    0.0f, 1.0f, 0.0f, // y
        0.0f, 0.0f, 5.0f,    0.0f, 0.0f, 1.0f  // z
    };
    unsigned int indices[] = {
        0, 1,    // x
        0, 2,    // y
        0, 3     // z
    };

    glGenVertexArrays(1, &VAOEjes);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAOEjes);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Posición
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void crearCilindro() {
    unsigned int VBO, EBO;

    const unsigned int segments = 32;
    const float radius = 1.0f;
    const float height = 1.5f;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Centro de la base inferior
    vertices.push_back(0.0f);      // x
    vertices.push_back(0.0f);      // y
    vertices.push_back(0.0f);      // z
    vertices.push_back(0.0f);      // nx
    vertices.push_back(-1.0f);     // ny
    vertices.push_back(0.0f);      // nz

    // Centro de la tapa superior
    vertices.push_back(0.0f);      // x
    vertices.push_back(height);    // y
    vertices.push_back(0.0f);      // z
    vertices.push_back(0.0f);      // nx
    vertices.push_back(1.0f);      // ny
    vertices.push_back(0.0f);      // nz

    // Vértices para la circunferencia (tanto para tapas como para paredes)
    for (unsigned int i = 0; i <= segments; ++i) {
        float angle = 2.0f * glm::pi<float>() * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Vértice para la base (y=0)
        vertices.push_back(x);             // x
        vertices.push_back(0.0f);          // y
        vertices.push_back(z);             // z
        vertices.push_back(0.0f);          // nx
        vertices.push_back(-1.0f);         // ny
        vertices.push_back(0.0f);          // nz

        // Vértice para la tapa superior (y=height)
        vertices.push_back(x);             // x
        vertices.push_back(height);        // y
        vertices.push_back(z);             // z
        vertices.push_back(0.0f);          // nx
        vertices.push_back(1.0f);          // ny
        vertices.push_back(0.0f);          // nz

        // Vértice para la pared con normal hacia afuera (inferior)
        vertices.push_back(x);             // x
        vertices.push_back(0.0f);          // y
        vertices.push_back(z);             // z
        vertices.push_back(x);             // nx (normal = posición normalizada)
        vertices.push_back(0.0f);          // ny
        vertices.push_back(z);             // nz

        // Vértice para la pared con normal hacia afuera (superior)
        vertices.push_back(x);             // x
        vertices.push_back(height);        // y
        vertices.push_back(z);             // z
        vertices.push_back(x);             // nx
        vertices.push_back(0.0f);          // ny
        vertices.push_back(z);             // nz
    }

    // Índices para la base inferior
    for (unsigned int i = 0; i < segments; ++i) {
        indices.push_back(0);  // Centro de la base
        indices.push_back(2 + 4 * i);  // Vértice actual
        indices.push_back(2 + 4 * ((i + 1) % segments));  // Siguiente vértice
    }

    // Índices para la tapa superior
    for (unsigned int i = 0; i < segments; ++i) {
        indices.push_back(1);  // Centro de la tapa superior
        indices.push_back(3 + 4 * ((i + 1) % segments));  // Siguiente vértice
        indices.push_back(3 + 4 * i);  // Vértice actual
    }

    // Índices para las paredes laterales
    for (unsigned int i = 0; i < segments; ++i) {
        unsigned int base = 4 + 4 * i;  // Base del vértice actual
        unsigned int next_base = 4 + 4 * ((i + 1) % segments);  // Base del siguiente vértice

        // Primer triángulo
        indices.push_back(base - 2);  // Inferior actual
        indices.push_back(base - 1);  // Superior actual
        indices.push_back(next_base - 2);  // Inferior siguiente

        // Segundo triángulo
        indices.push_back(next_base - 2);  // Inferior siguiente
        indices.push_back(base - 1);  // Superior actual
        indices.push_back(next_base - 1);  // Superior siguiente
    }

    // Configurar VAO, VBO y EBO
    glGenVertexArrays(1, &VAOCilindro);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAOCilindro);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Posición
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void crearSkybox() {

    float skyboxVertices[] =
    {
        //   Coordinates
        -1.0f, -1.0f,  1.0f,//        7--------6
         1.0f, -1.0f,  1.0f,//       /|       /|
         1.0f, -1.0f, -1.0f,//      4--------5 |
        -1.0f, -1.0f, -1.0f,//      | |      | |
        -1.0f,  1.0f,  1.0f,//      | 3------|-2
         1.0f,  1.0f,  1.0f,//      |/       |/
         1.0f,  1.0f, -1.0f,//      0--------1
        -1.0f,  1.0f, -1.0f
    };

    unsigned int skyboxIndices[] =
    {
        // Right
        1, 2, 6,
        6, 5, 1,
        // Left
        0, 4, 7,
        7, 3, 0,
        // Top
        4, 5, 6,
        6, 7, 4,
        // Bottom
        0, 3, 2,
        2, 1, 0,
        // Back
        0, 1, 5,
        5, 4, 0,
        // Front
        3, 7, 6,
        6, 2, 3
    };

    unsigned int skyboxVBO, skyboxEBO;
    glGenVertexArrays(1, &VAOSkybox);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);
    glBindVertexArray(VAOSkybox);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    // All the faces of the cubemap (make sure they are in this exact order)
    std::string facesCubemap[6] =
    {
        "textures/skybox/right.jpg",
        "textures/skybox/left.jpg",
        "textures/skybox/top.jpg",
        "textures/skybox/bottom.jpg",
        "textures/skybox/front.jpg",
        "textures/skybox/back.jpg"
    };

    unsigned int texturaID = cargarTextura("textures/skybox/front.jpg");
    verificarEstadoTextura(texturaID, "textures/skybox/front.jpg");
    texturaID = cargarTextura("textures/skybox/left.jpg");
    verificarEstadoTextura(texturaID, "textures/skybox/left.jpg");



    // Creates the cubemap texture object

    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // These are very important to prevent seams
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // This might help with seams on some systems
    //glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // Cycles through all the textures and attaches them to the cubemap object
    for (unsigned int i = 0; i < 6; i++)
    {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            stbi_set_flip_vertically_on_load(false);
            glTexImage2D
            (
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                width,
                height,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
            stbi_image_free(data);
        }
    }

}

void crearCubo() {
    unsigned int VBO, EBO;
    // Vértices del cubo con normales correctas y coordenadas de textura
    float vertices[] = {
        // Posiciones              // Normales (apuntando hacia afuera)           // Coordenadas de textura
        // Cara frontal (Z negativo)
        -0.5f, -0.5f, -0.5f,      0.0f,  0.0f, -1.0f,       0.0f, 0.0f,  // Vértice 0
         0.5f, -0.5f, -0.5f,      0.0f,  0.0f, -1.0f,       1.0f, 0.0f,  // Vértice 1
         0.5f,  0.5f, -0.5f,      0.0f,  0.0f, -1.0f,       1.0f, 1.0f,  // Vértice 2
        -0.5f,  0.5f, -0.5f,      0.0f,  0.0f, -1.0f,       0.0f, 1.0f,  // Vértice 3
        // Cara trasera (Z positivo)
        -0.5f, -0.5f,  0.5f,      0.0f,  0.0f,  1.0f,       0.0f, 0.0f,  // Vértice 4
         0.5f, -0.5f,  0.5f,      0.0f,  0.0f,  1.0f,       1.0f, 0.0f,  // Vértice 5
         0.5f,  0.5f,  0.5f,      0.0f,  0.0f,  1.0f,       1.0f, 1.0f,  // Vértice 6
        -0.5f,  0.5f,  0.5f,      0.0f,  0.0f,  1.0f,       0.0f, 1.0f,  // Vértice 7
        // Cara izquierda (X negativo)
        -0.5f, -0.5f, -0.5f,     -1.0f,  0.0f,  0.0f,       0.0f, 0.0f,  // Vértice 8
        -0.5f, -0.5f,  0.5f,     -1.0f,  0.0f,  0.0f,       1.0f, 0.0f,  // Vértice 9
        -0.5f,  0.5f,  0.5f,     -1.0f,  0.0f,  0.0f,       1.0f, 1.0f,  // Vértice 10
        -0.5f,  0.5f, -0.5f,     -1.0f,  0.0f,  0.0f,       0.0f, 1.0f,  // Vértice 11
        // Cara derecha (X positivo)
         0.5f, -0.5f, -0.5f,      1.0f,  0.0f,  0.0f,       0.0f, 0.0f,  // Vértice 12
         0.5f, -0.5f,  0.5f,      1.0f,  0.0f,  0.0f,       1.0f, 0.0f,  // Vértice 13
         0.5f,  0.5f,  0.5f,      1.0f,  0.0f,  0.0f,       1.0f, 1.0f,  // Vértice 14
         0.5f,  0.5f, -0.5f,      1.0f,  0.0f,  0.0f,       0.0f, 1.0f,  // Vértice 15
         // Cara inferior (Y negativo)
         -0.5f, -0.5f, -0.5f,      0.0f, -1.0f,  0.0f,      0.0f, 0.0f,  // Vértice 16
          0.5f, -0.5f, -0.5f,      0.0f, -1.0f,  0.0f,      1.0f, 0.0f,  // Vértice 17
          0.5f, -0.5f,  0.5f,      0.0f, -1.0f,  0.0f,      1.0f, 1.0f,  // Vértice 18
         -0.5f, -0.5f,  0.5f,      0.0f, -1.0f,  0.0f,      0.0f, 1.0f,  // Vértice 19
         // Cara superior (Y positivo)
         -0.5f,  0.5f, -0.5f,      0.0f,  1.0f,  0.0f,      0.0f, 0.0f,  // Vértice 20
          0.5f,  0.5f, -0.5f,      0.0f,  1.0f,  0.0f,      1.0f, 0.0f,  // Vértice 21
          0.5f,  0.5f,  0.5f,      0.0f,  1.0f,  0.0f,      1.0f, 1.0f,  // Vértice 22
         -0.5f,  0.5f,  0.5f,      0.0f,  1.0f,  0.0f,      0.0f, 1.0f,  // Vértice 23
    };

    unsigned int indices[] = {
        // Cara frontal (Z negativo)
        0, 1, 2,  0, 2, 3,
        // Cara trasera (Z positivo)
        4, 6, 5,  4, 7, 6,
        // Cara izquierda (X negativo)
        8, 9, 10,  8, 10, 11,
        // Cara derecha (X positivo)
        12, 13, 14,  12, 14, 15,
        // Cara superior (Y positivo)
        20, 21, 22,  20, 22, 23,
        // Cara inferior (Y negativo)
        16, 18, 17,  16, 19, 18
    };

    // Generar VAO, VBO y EBO
    glGenVertexArrays(1, &VAOCubo);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAOCubo);

    // Vértices e índices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Atributo 0: Posición
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Atributo 1: Normales
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Atributo 2: Coordenadas de textura
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}







/*------------------------ Clase simple para objetos básicos ------------------------*/
struct AABB {
    glm::vec2 position; // centro
    glm::vec2 halfSize; // la mitad del tamaño (ancho/2, largo/2)
};


class Objeto {
public:
    glm::vec3 posicion;
    glm::vec3 escala;
    glm::vec3 color;
    glm::vec3 rotacion;
    bool usarTextura;
	bool esLuzFoco;
    unsigned int texturaID;
    unsigned int VAO;

    AABB aabb;

    Objeto(glm::vec3 pos, glm::vec3 esc, glm::vec3 col, glm::vec3 rot, unsigned int vao, const char* rutaTextura = nullptr, bool* esLuzFoco2 = nullptr) {
        posicion = pos;
        escala = esc;
        color = col;
        rotacion = rot;
        VAO = vao;
        usarTextura = false;
        esLuzFoco = false;

        if (rutaTextura != nullptr) {
            texturaID = cargarTextura(rutaTextura);
            verificarEstadoTextura(texturaID, rutaTextura);
            usarTextura = true;

            glBindTexture(GL_TEXTURE_2D, texturaID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        if (esLuzFoco2 != nullptr) esLuzFoco = true;

		aabb.position = glm::vec2(posicion.x, posicion.z);
		aabb.halfSize = glm::vec2(escala.x / 2 + 0.1f, escala.z / 2 + 0.1f);
    }

    glm::mat4 getModel() {
        glm::mat4 model;
        model = glm::translate(model, posicion);

        model = glm::rotate(model, glm::radians(rotacion.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Rot Y
        model = glm::rotate(model, glm::radians(rotacion.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Rot X
        model = glm::rotate(model, glm::radians(rotacion.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Rot Z

        model = glm::scale(model, escala);

        return model;
    }

    void actualizarAABB() {
        aabb.position = glm::vec2(posicion.x, posicion.z);
        aabb.halfSize = glm::vec2(escala.x / 2, escala.z / 2);
    }

    void dibujar(glm::mat4 view) {
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 120.0f);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(VAO);

        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        unsigned int colorLoc = glGetUniformLocation(shaderProgram, "color");
        unsigned int useTextureLoc = glGetUniformLocation(shaderProgram, "useTexture");
        unsigned int textureLoc = glGetUniformLocation(shaderProgram, "texture_diffuse");
        unsigned int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
        unsigned int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
        unsigned int lightEnabledLoc = glGetUniformLocation(shaderProgram, "lightEnabled");

        // Modificar esta parte para manejar las luces de forma diferente
        if (esLuzFoco) {
            // Siempre dibujar los focos del coche, independientemente de lightEnabled
            glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
            glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
            glUniform1i(lightEnabledLoc, 1);  // Forzar habilitado para focos
        }
        else {
            // Para otras luces, seguir la configuración global
            glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
            glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
            glUniform1i(lightEnabledLoc, lightEnabled);
        }

        if (VAO == VAOSkybox)  // Dibuja el skybox
        {
            // Activamos el shader específico para el skybox
            glUseProgram(skyboxShader);

            // Eliminamos la información de traslación de la matriz de vista para que el skybox no se mueva
            glm::mat4 skyboxView = glm::mat4(glm::mat3(view)); // Elimina la traslación

            // Pasamos las matrices al shader del skybox
            unsigned int viewLoc = glGetUniformLocation(skyboxShader, "view");
            unsigned int projectionLoc = glGetUniformLocation(skyboxShader, "projection");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(skyboxView));
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

            // Configuración para dibujar el skybox correctamente
            glDepthFunc(GL_LEQUAL); // Cambiamos la función de profundidad para que el skybox se dibuje cuando depth = 1.0

            // Vinculamos el VAO del skybox y su textura
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

            // Dibujamos el skybox
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            // Restauramos la configuración original
            glDepthFunc(GL_LESS);
        }
        else {
            // Si hay textura, activarla y configurarla
            if (usarTextura) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texturaID);
                glUniform1i(textureLoc, 0);  // Unidad de textura 0
                glUniform1i(useTextureLoc, 1);  // Indicar que se usa textura
            }
            else {
                glUniform1i(useTextureLoc, 0);  // No usar textura
            }

            glm::mat4 model = getModel();
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3fv(colorLoc, 1, glm::value_ptr(color));

            if (VAO == VAOCubo)        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            else if (VAO == VAOPlano)       glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            else if (VAO == VAOCilindro)    glDrawElements(GL_TRIANGLES, 3 * 32 + 3 * 32 + 6 * 32, GL_UNSIGNED_INT, 0);
            else if (VAO == VAOEjes)        glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);


            
        }

        glBindVertexArray(0);
        
    }
};

// Objetos de la escena
Objeto* suelo, * skyBox, * ejes;

Objeto* edificio1, * edificio2, * edificio3, * edificio4, * edificio5, * edificio6, * edificio7, * edificio8, * edificio9, * edificio10;

//Tanque
Objeto* cuerpoDelTanque, * cuerpoDelTanque2,
* cabezaDelTanque, * cabezaDelTanque2, * cabezaDelTanque3,
* canhonDelTanque, * canhonDelTanque2, * canhonDelTanque3,
* ruedaDelTanque1, * ruedaDelTanque2, * ruedaDelTanque3, * ruedaDelTanque4, * ruedaDelTanque5, * ruedaDelTanque6,
* llantaDelTanque1, * llantaDelTanque2, * llantaDelTanque3, * llantaDelTanque4, * llantaDelTanque5, * llantaDelTanque6,
* cintaDeLasRuedasIzq, * cintaDeLasRuedasDer;

//Tanque Roto
Objeto* cuerpoDelTanqueRoto, * cuerpoDelTanque2Roto,
* cabezaDelTanqueRoto, * cabezaDelTanque2Roto, * cabezaDelTanque3Roto,
* canhonDelTanqueRoto, * canhonDelTanque2Roto, * canhonDelTanque3Roto,
* ruedaDelTanque1Roto, * ruedaDelTanque2Roto, * ruedaDelTanque3Roto, * ruedaDelTanque4Roto, * ruedaDelTanque5Roto, * ruedaDelTanque6Roto,
* llantaDelTanque1Roto, * llantaDelTanque2Roto, * llantaDelTanque3Roto, * llantaDelTanque4Roto, * llantaDelTanque5Roto, * llantaDelTanque6Roto,
* cintaDeLasRuedasIzqRoto, * cintaDeLasRuedasDerRoto;

//Torreta enemiga
Objeto* baseTorreta1, * baseTorreta2, * posteTorreta, * cabezaTorreta, * canonTorreta;



/*------------------------ FUNCIONES AUXILIARES ------------------------*/
glm::vec3 obtenerPuntoCirculo(float centroX, float centroZ, float rotacion, float angulo, float radio) {
    // El ángulo final es relativo a la rotación actual del tanque
    float anguloFinal = rotacion + angulo;

    // Convertir a radianes
    float anguloRad = glm::radians(anguloFinal);

    // Calcular posición X y Z en el círculo
    float x = centroX + radio * sin(anguloRad);
    float z = centroZ + radio * cos(anguloRad);

    // Devolver la posición 3D (manteniendo la misma altura Y que el tanque)
    return glm::vec3(x, POSICION_CUERPODELTANQUE.y, z);
}

glm::vec3 obtenerPuntoEsfera(const glm::vec3& centro, float rotacionHorizontal, float anguloHorizontal, float anguloVertical, float radio) {
    // Convertir ángulos a radianes
    float azimut = glm::radians(rotacionHorizontal + anguloHorizontal);
    float elevacion = glm::radians(anguloVertical);

    // Calcular componentes esféricas
    float x = radio * sin(elevacion) * sin(azimut);
    float y = radio * cos(elevacion);
    float z = radio * sin(elevacion) * cos(azimut);

    // Aplicar al centro y devolver
    return centro + glm::vec3(x, y, z);
}

bool checkCollision(const AABB& a, const AABB& b) {
    return (abs(a.position.x - b.position.x) <= (a.halfSize.x + b.halfSize.x)) &&
        (abs(a.position.y - b.position.y) <= (a.halfSize.y + b.halfSize.y));
}

bool revisarColisiones(const AABB& tanqueAABB) {
    std::vector<Objeto*> obstaculos = {
        edificio1, edificio2, edificio3, edificio4, edificio5,
        edificio6, edificio7, edificio8, edificio9
    };

    for (auto& obstaculo : obstaculos) {
        if (checkCollision(tanqueAABB, obstaculo->aabb)) {
            return true;
        }
    }
    return false;
}

void moverTanque(bool adelante, float rotacion, float velocidad) {
	velocidad *= 1; // Ajustar la velocidad

    AABB aabbOriginal;
    aabbOriginal.position = glm::vec2(posicionTanqueX, posicionTanqueZ);
    aabbOriginal.halfSize = glm::vec2(ESCALA_COCHE.x, ESCALA_COCHE.z*0.9);

    // Calcular nueva posición temporal
    float newX = posicionTanqueX;
    float newZ = posicionTanqueZ;

    if (adelante) {
        newX += sin(glm::radians(rotacion)) * velocidad;
        newZ += cos(glm::radians(rotacion)) * velocidad;
    }
    else {
        newX -= sin(glm::radians(rotacion)) * velocidad;
        newZ -= cos(glm::radians(rotacion)) * velocidad;
    }

    // Verificar colisión en la nueva posición
    AABB aabbNuevo;
    aabbNuevo.position = glm::vec2(newX, newZ);
    aabbNuevo.halfSize = aabbOriginal.halfSize;

    if (!revisarColisiones(aabbNuevo)) {
        posicionTanqueX = newX;
        posicionTanqueZ = newZ;
        rotacionRuedas += (adelante ? 5.0f : -5.0f);
    }
}






/*------------------------ MY CAMARA ------------------------*/
glm::mat4 myCamara() {
    glm::mat4 view;

    if (camaraLibre) { //camaraLibre
        view = glm::lookAt(
            cameraPosition,
            glm::vec3(posicionTanqueX, 0.0f, posicionTanqueZ),  // Mirando al tanque
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
    }
    else if (terceraPersona) {
        view = glm::lookAt(
            obtenerPuntoCirculo(obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.4).x,
                obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.4).z,
                giroCamara3Persona,
                180,
                4.5f) + glm::vec3(0.0f, (1.5f - (inclinacionCanon - 77) / 100), 0.0f),                   // Posición en la punta del cañón
            glm::vec3(obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, giroCamara3Persona, 0, 100).x, 0.0f, obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, giroCamara3Persona, 0, 100).z),  // Mirando en la dirección del cañón
            glm::vec3(0.0f, 1.0f, 0.0f)     // Vector "arriba"
        );
    }
    else  if (primeraPersona) {
        // Modo primera persona: cámara en el cañón mirando hacia adelante
        view = glm::lookAt(
            obtenerPuntoEsfera(glm::vec3(obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.4).x,
                0.35f,
                obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.4).z),
                rotacionTanque,
                giroTorreta,
                inclinacionCanon,
                2.6) + glm::vec3(0.0f, 0.4f, 0.0f),                   // Posición en la punta del cañón
            obtenerPuntoEsfera(glm::vec3(obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.4).x,
                0.35f,
                obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.4).z),
                rotacionTanque,
                giroTorreta,
                inclinacionCanon,
                2.8) + glm::vec3(0.0f, 0.4f, 0.0f),  // Mirando en la dirección del cañón
            glm::vec3(0.0f, 1.0f, 0.0f)     // Vector "arriba"
        );



        //dibujarMiraDeSniper();
    }
    return view;

}








/*------------------------ FUNCIONES DE OPCIONES ------------------------*/
void openGlInit() {
    // Activa el Buffer de profuncidda
    glEnable(GL_DEPTH_TEST);

    // Activa el Cull Facing
    glEnable(GL_CULL_FACE);

    // Mantiene las caras frontales en el dibujado
    glCullFace(GL_FRONT);

    glFrontFace(GL_CCW);

    glClearDepth(1.0f);

    //Como el cubemap siempre tendrá una profundidad 1.0, we need that equal sign so it doesn't get discarded
    glDepthFunc(GL_LEQUAL);

    // Añade un pequeño offset para evitar z-fighting
    glPolygonOffset(1.0f, 1.0f);
}

void tamanhoPantalla(GLFWwindow* ventana, int ancho, int alto) {
    SCR_WIDTH = ancho;
    SCR_HEIGHT = alto;
}

void calculoFPS(GLFWwindow* window) {
    crntTime = glfwGetTime();
    timeDiff = crntTime - prevTime;
    counter++;

    if (timeDiff >= 1.0 / 30.0)
    {
        // Crea el titulo nuevo
        std::string soldisplayed = lightEnabled ? "ON" : "OFF";
        std::string FPS = std::to_string((1.0 / timeDiff) * counter);
        std::string ms = std::to_string((timeDiff / counter) * 1000);
        std::string newTitle = "Minijuego Tanque - LightEnabled? " + soldisplayed + " - FPS: " + FPS + " / ms: " + ms;
        glfwSetWindowTitle(window, newTitle.c_str());

        // Resets times and counter
        prevTime = crntTime;
        counter = 0;
    }
}





/*------------------------ TECLADO ------------------------*/
void teclado(GLFWwindow* ventana, int tecla, int scancode, int accion, int mods) {
    if (accion == GLFW_PRESS || accion == GLFW_REPEAT) {
        switch (tecla) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(ventana, true);
            break;

        case GLFW_KEY_E:
            verEjes = !verEjes;
            break;

        case GLFW_KEY_B:
            lightEnabled = !lightEnabled;
            std::cout << "Iluminación solar: " << (lightEnabled ? "ACTIVADA" : "DESACTIVADA") << std::endl;
            break;

        case GLFW_KEY_Q: // Cambiar entre primera y tercera persona
            primeraPersona = !primeraPersona;
            terceraPersona = !terceraPersona;
            break;

        case GLFW_KEY_2: // Cambiar entre primera y tercera persona
            camaraLibre = !camaraLibre;
            break;

        case GLFW_KEY_R:
            //RECARGAR
            break;

        case GLFW_KEY_SPACE: // Cambiar GLFW_KEY_ESPACE a GLFW_KEY_SPACE
            //DISPARAR;
            break;

            // MOVIMIENTO DEL TANQUE (con desviación suave)
        case GLFW_KEY_W:
            moverTanque(1, rotacionTanque, velocidadTanque);
            giroRuedas += 5.0f;
            rotacionRuedas = 0;
            break;

        case GLFW_KEY_S:
            moverTanque(0, rotacionTanque, velocidadTanque);
            giroRuedas -= 5.0f;
            rotacionRuedas = 0;
            break;

        case GLFW_KEY_A: // Desviación izquierda + rotación
            moverTanque(1, rotacionTanque + 22.0f, velocidadTanque * 0.8f);
            giroRuedas += 4.0f;

            rotacionRuedas = 10;

            rotacionTanque += 1.0f;
            rotacionTorreta += 1.0f;
            rotacionCanhon += 1.0f;
            rotacionCintas -= 1.0f;

            giroCamara3Persona += 1.0f;
            break;

        case GLFW_KEY_D: // Desviación derecha + rotación
            moverTanque(1, rotacionTanque - 22.0f, velocidadTanque * 0.8f);
            giroRuedas += 4.0f;

            rotacionRuedas = -10;

            rotacionTanque -= 1.0f;
            rotacionTorreta -= 1.0f;
            rotacionCanhon -= 1.0f;
            rotacionCintas += 1.0f;

            giroCamara3Persona -= 1.0f;
            break;

            // MOVIMIENTO DE LA TORRETA Y CAÑON
        case GLFW_KEY_LEFT: {
            float nuevaRotacion = rotacionTorreta + 2.0f;
            float diff = fmod(nuevaRotacion - rotacionTanque + 180.0f, 360.0f) - 180.0f;
            if (diff <= 90.0f && diff >= -90.0f) {
                rotacionTorreta = nuevaRotacion;
                rotacionCanhon = nuevaRotacion + 90;
                giroTorreta += 2.0f;
                giroCamara3Persona += 2.0f;
            }
            break;
        }
        case GLFW_KEY_RIGHT: {
            float nuevaRotacion = rotacionTorreta - 2.0f;
            float diff = fmod(nuevaRotacion - rotacionTanque + 180.0f, 360.0f) - 180.0f;
            if (diff <= 90.0f && diff >= -90.0f) {
                rotacionTorreta = nuevaRotacion;
                rotacionCanhon = nuevaRotacion + 90;
                giroTorreta -= 2.0f;
                giroCamara3Persona -= 2.0f;
            }
            break;
        }
        case GLFW_KEY_UP:
            if (inclinacionCanon > 77) {
                inclinacionCanon -= 1.0f;
            }
            break;
        case GLFW_KEY_DOWN:
            if (inclinacionCanon < 92.0f) {
                inclinacionCanon += 1.0f;
            }
            break;




            // MOVIMIENTO DE LA CÁMARA (sin cambios)
        case GLFW_KEY_I: cameraPosition.y += 0.5f; break;
        case GLFW_KEY_K: cameraPosition.y -= 0.5f; break;
        case GLFW_KEY_J: cameraPosition.x -= 0.5f; break;
        case GLFW_KEY_L: cameraPosition.x += 0.5f; break;
        case GLFW_KEY_U: cameraPosition.z -= 0.5f; break;
        case GLFW_KEY_O: cameraPosition.z += 0.5f; break;
        }
    }
}






/*------------------------ CREAR OBJETOS ------------------------*/
void crearTanque() {
    cuerpoDelTanque = new Objeto(
        POSICION_CUERPODELTANQUE,  // Posición (0.0f, 0.5f, 0.0f)
        ESCALA_COCHE,           // Escala (1.0f, 0.5f, 2.0f)
        COLOR_GRIS_OSCURO,       // Color
        glm::vec3(0.0f, 0.0f, 0.0f),                   // Rotación
        VAOCubo,
        "textures/tanque.png"
    );

    cuerpoDelTanque2 = new Objeto(
        POSICION_CUERPODELTANQUE2,  // Posición (0.0f, 0.5f, 0.0f)
        ESCALA_COCHE2,           // Escala (1.0f, 0.5f, 2.0f)
        COLOR_GRIS_OSCURO,       // Color
        glm::vec3(0.0f, 0.0f, 0.0f),                   // Rotación
        VAOCubo,
        "textures/tanque.png"
    );

    if (cuerpoDelTanque != nullptr) {

        //Crear la cabeza del tanque (torreta)
        cabezaDelTanque = new Objeto(
            POSICION_TORRETA,     // Posición relativa al cuerpo
            ESCALA_TORRETA,       // Escala
            COLOR_GRIS_CLARO,   // Color diferente para distinguir
            glm::vec3(0.0f, 0.0f, 0.0f),                // Rotación inicial
            VAOCubo,
            "textures/tanque.png"
        );


        cabezaDelTanque2 = new Objeto(
            POSICION_TORRETA,     // Posición relativa al cuerpo
            ESCALA_TORRETA2,       // Escala
            COLOR_GRIS,   // Color diferente para distinguir
            glm::vec3(0.0f, 0.0f, 0.0f),                // Rotación inicial
            VAOCubo,
            "textures/tanque2.png"             
        );


        cabezaDelTanque3 = new Objeto(
            POSICION_TORRETA,     // Posición relativa al cuerpo
            ESCALA_TORRETA3,       // Escala
            COLOR_GRIS_OSCURO,   // Color diferente para distinguir
            glm::vec3(0.0f, 0.0f, 0.0f),                // Rotación inicial
            VAOCubo,
            "textures/tanque.png"
        );


        if (cabezaDelTanque != nullptr) {

            canhonDelTanque = new Objeto(
                POSICION_CANHON,            // Posición del cañón
                glm::vec3(0.05f, 0.7f, 0.05f), // Escala (ajusta el Z para hacerlo más largo)
                COLOR_GRIS,          // Color
                glm::vec3(0.0f, rotacionTorreta, 0.0f),            // Rotación inicial
                VAOCilindro,
                "textures/tanque.png"
            );

            canhonDelTanque2 = new Objeto(
                POSICION_CANHON,            // Posición del cañón
                glm::vec3(0.03f, 1.3f, 0.03f), // Escala (ajusta el Z para hacerlo más largo)
                COLOR_GRIS,          // Color
                glm::vec3(0.0f, rotacionTorreta, 0.0f),            // Rotación inicial
                VAOCilindro,
                "textures/tanque.png"
            );

            canhonDelTanque3 = new Objeto(
                POSICION_CANHON,            // Posición del cañón
                glm::vec3(0.05f, 0.20f, 0.05f), // Escala (ajusta el Z para hacerlo más largo)
                COLOR_GRIS,          // Color
                glm::vec3(0.0f, rotacionTorreta, 0.0f),            // Rotación inicial
                VAOCilindro,
                "textures/tanque.png"
            );
        }





        // Crear las ruedas
        cintaDeLasRuedasIzq = new Objeto(
            POSICION_CINTA_IZQ,            // Posición de la cinta
            ESCALA_CINTA_IZQ,
            COLOR_NEGRO,          // Color
            glm::vec3(0.0f, rotacionTanque, 0.0f),            // Rotación inicial
            VAOCilindro                 // VAO del cilindro
        );
        if (cintaDeLasRuedasIzq != nullptr) {

            ruedaDelTanque1 = new Objeto(
                POSICION_RUEDA_IZQ_1,
                ESCALA_RUEDAS, // Cubo más pequeño
                COLOR_GRIS_CLARO,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCubo,
                "textures/arena.jpg"
            );

            ruedaDelTanque2 = new Objeto(
                POSICION_RUEDA_IZQ_2,
                ESCALA_RUEDAS,
                COLOR_GRIS_CLARO,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCubo,
                "textures/arena.jpg"
            );

            ruedaDelTanque3 = new Objeto(
                POSICION_RUEDA_IZQ_3,
                ESCALA_RUEDAS,
                COLOR_GRIS_CLARO,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCubo,
                "textures/arena.jpg"
            );

            llantaDelTanque1 = new Objeto(
                POSICION_RUEDA_IZQ_1,
                ESCALA_LLANTAS, // Cubo más pequeño
                COLOR_GRIS,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCilindro
            );

            llantaDelTanque2 = new Objeto(
                POSICION_RUEDA_IZQ_2,
                ESCALA_LLANTAS,
                COLOR_GRIS,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCilindro
            );

            llantaDelTanque3 = new Objeto(
                POSICION_RUEDA_IZQ_3,
                ESCALA_LLANTAS,
                COLOR_GRIS,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCilindro
            );


        }

        cintaDeLasRuedasDer = new Objeto(
            POSICION_CINTA_DER,            // Posición de la cinta
            ESCALA_CINTA_DER,
            COLOR_NEGRO,          // Color
            glm::vec3(0.0f, rotacionTanque, 0.0f),            // Rotación inicial
            VAOCilindro                 // VAO del cilindro
        );
        if (cintaDeLasRuedasDer != nullptr) {

            ruedaDelTanque4 = new Objeto(
                POSICION_RUEDA_DER_1,
                ESCALA_RUEDAS,
                COLOR_GRIS_CLARO,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCubo,
                "textures/arena.jpg"
            );

            ruedaDelTanque5 = new Objeto(
                POSICION_RUEDA_IZQ_2,
                ESCALA_RUEDAS,
                COLOR_GRIS_CLARO,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCubo,
                "textures/arena.jpg"
            );

            ruedaDelTanque6 = new Objeto(
                POSICION_RUEDA_IZQ_3,
                ESCALA_RUEDAS,
                COLOR_GRIS_CLARO,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCubo,
                "textures/arena.jpg"
            );

            llantaDelTanque4 = new Objeto(
                POSICION_RUEDA_DER_1,
                ESCALA_LLANTAS, // Cubo más pequeño
                COLOR_GRIS,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCilindro
            );

            llantaDelTanque5 = new Objeto(
                POSICION_RUEDA_DER_2,
                ESCALA_LLANTAS,
                COLOR_GRIS,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCilindro
            );

            llantaDelTanque6 = new Objeto(
                POSICION_RUEDA_DER_3,
                ESCALA_LLANTAS,
                COLOR_GRIS,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCilindro
            );


        }
    }

}

void crearTanqueRoto() {
    cuerpoDelTanqueRoto = new Objeto(
        glm::vec3(10, cero, 30),  // Posición (0.0f, 0.5f, 0.0f)
        1.2f * ESCALA_COCHE,           // Escala (1.0f, 0.5f, 2.0f)
        COLOR_GRIS_OSCURO,       // Color
        glm::vec3(cero, -30.0f, 30.0f),
        VAOCubo,
        "textures/tanque.png"
    );

    cuerpoDelTanque2Roto = new Objeto(
        glm::vec3(10, 0.2f, 30),  // Posición (0.0f, 0.5f, 0.0f)
        1.2f * ESCALA_COCHE2,           // Escala (1.0f, 0.5f, 2.0f)
        COLOR_GRIS_OSCURO,       // Color
        glm::vec3(cero, -30.0f, 30.0f),
        VAOCubo,
        "textures/tanque.png"
    );

    if (cuerpoDelTanqueRoto != nullptr) {

        //Crear la cabeza del tanque (torreta)
        cabezaDelTanqueRoto = new Objeto(
            glm::vec3(9, 0.1f, 30),    
            1.2f * ESCALA_TORRETA,
            COLOR_GRIS_CLARO,   
            glm::vec3(cero, cero, cero),               
            VAOCubo            
        );


        cabezaDelTanque2Roto = new Objeto(
            glm::vec3(9, 0.2f, 30.2f),     
            1.2f * ESCALA_TORRETA2,
            COLOR_GRIS,   
            glm::vec3(cero, cero, cero),               
            VAOCubo             
        );


        cabezaDelTanque3Roto = new Objeto(
            glm::vec3(9, 0.15f, 29.75f),     
            1.2f * ESCALA_TORRETA3,
            COLOR_GRIS_OSCURO, 
            glm::vec3(cero, cero, cero),               
            VAOCubo            
        );


        if (cabezaDelTanqueRoto != nullptr) {

            canhonDelTanqueRoto = new Objeto(
                glm::vec3(9, 0.15f, 29.75f),      
                1.2f * glm::vec3(0.05f, 0.7f, 0.05f),
                COLOR_GRIS,       
                glm::vec3(cero, -90, 90),            
                VAOCilindro                
            );

            canhonDelTanque2Roto = new Objeto(
                glm::vec3(9, 0.15f, 29.75f),      
                1.2f * glm::vec3(0.03f, 1.3f, 0.03f),
                COLOR_GRIS,        
                glm::vec3(cero, -90, 90),
                VAOCilindro                
            );

            canhonDelTanque3Roto = new Objeto(
                glm::vec3(9, 0.15f, 29.75f),      
                1.2f * glm::vec3(0.05f, 0.20f, 0.05f),
                COLOR_GRIS,         
                glm::vec3(cero, -90, 90),
                VAOCilindro               
            );
        }





        // Crear las ruedas
        cintaDeLasRuedasIzqRoto = new Objeto(
            glm::vec3(11, 0, 30),            // Posición de la cinta
            ESCALA_CINTA_IZQ,
            COLOR_NEGRO,          // Color
            glm::vec3(0.0f, rotacionTanque, 0.0f),            // Rotación inicial
            VAOCilindro                 // VAO del cilindro
        );
        if (cintaDeLasRuedasIzqRoto != nullptr) {

            ruedaDelTanque1Roto = new Objeto(
                glm::vec3(11, 0, 30),
                ESCALA_RUEDAS, // Cubo más pequeño
                COLOR_GRIS_CLARO,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCubo,
                "textures/arena.jpg"
            );

            ruedaDelTanque2Roto = new Objeto(
                glm::vec3(11, 0, 30),
                ESCALA_RUEDAS,
                COLOR_GRIS_CLARO,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCubo,
                "textures/arena.jpg"
            );

            ruedaDelTanque3Roto = new Objeto(
                glm::vec3(11, 0, 30),
                ESCALA_RUEDAS,
                COLOR_GRIS_CLARO,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCubo,
                "textures/arena.jpg"
            );

            llantaDelTanque1Roto = new Objeto(
                glm::vec3(11, 0, 30),
                ESCALA_LLANTAS, // Cubo más pequeño
                COLOR_GRIS,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCilindro
            );

            llantaDelTanque2Roto = new Objeto(
                glm::vec3(11, 0, 30),
                ESCALA_LLANTAS,
                COLOR_GRIS,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCilindro
            );

            llantaDelTanque3Roto = new Objeto(
                glm::vec3(11, 0, 30),
                ESCALA_LLANTAS,
                COLOR_GRIS,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCilindro
            );


        }

        cintaDeLasRuedasDerRoto = new Objeto(
            glm::vec3(11, 0, 30),          // Posición de la cinta
            ESCALA_CINTA_DER,
            COLOR_NEGRO,          // Color
            glm::vec3(0.0f, rotacionTanque, 0.0f),            // Rotación inicial
            VAOCilindro                 // VAO del cilindro
        );
        if (cintaDeLasRuedasDerRoto != nullptr) {

            ruedaDelTanque4Roto = new Objeto(
                glm::vec3(11, 0, 30),
                ESCALA_RUEDAS,
                COLOR_GRIS_CLARO,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCubo,
                "textures/arena.jpg"
            );

            ruedaDelTanque5Roto = new Objeto(
                glm::vec3(11, 0, 30),
                ESCALA_RUEDAS,
                COLOR_GRIS_CLARO,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCubo,
                "textures/arena.jpg"
            );

            ruedaDelTanque6Roto = new Objeto(
                glm::vec3(11, 0, 30),
                ESCALA_RUEDAS,
                COLOR_GRIS_CLARO,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCubo,
                "textures/arena.jpg"
            );

            llantaDelTanque4Roto = new Objeto(
                glm::vec3(11, 0, 30),
                ESCALA_LLANTAS, // Cubo más pequeño
                COLOR_GRIS,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCilindro
            );

            llantaDelTanque5Roto = new Objeto(
                glm::vec3(11, 0, 30),
                ESCALA_LLANTAS,
                COLOR_GRIS,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCilindro
            );

            llantaDelTanque6Roto = new Objeto(
                glm::vec3(11, 0, 30),
                ESCALA_LLANTAS,
                COLOR_GRIS,
                glm::vec3(0.0f, rotacionTanque, 0.0f),
                VAOCilindro
            );


        }
    }

}

void crearEdificios() {
    edificio1 = new Objeto(
        POSICION_EDIFICIO1,                             // Posición 1
        ESCALA_EDIFICIO1,                                // Escala EDIFICIO
        COLOR_BLANCO,                                   // Color
        glm::vec3(0.0f, 0.0f, 0.0f),                    // Rotación
        VAOCubo,
		"textures/edificios/edificio4.png"                        // Textura
    );

    edificio2 = new Objeto(
        POSICION_EDIFICIO2,                             // Posición 1
        ESCALA_EDIFICIO2,                                // Escala EDIFICIO
        COLOR_BLANCO,                                   // Color
        glm::vec3(0.0f, 0.0f, 0.0f),                    // Rotación
        VAOCubo,
        "textures/edificios/edificio5.png"                      // Textura
    );

    edificio3 = new Objeto(
        POSICION_EDIFICIO3,                             // Posición 1
        ESCALA_EDIFICIO3,                                // Escala EDIFICIO
        COLOR_BLANCO,                                   // Color
        glm::vec3(0.0f, 0.0f, 0.0f),                    // Rotación
        VAOCubo,
        "textures/edificios/edificio1.png"                  // Textura
    );

    edificio4 = new Objeto(
        POSICION_EDIFICIO4,                             // Posición 1
        ESCALA_EDIFICIO4,                                // Escala EDIFICIO
        COLOR_BLANCO,                                   // Color
        glm::vec3(0.0f, 0.0f, 0.0f),                    // Rotación
        VAOCubo,
        "textures/edificios/edificio3.png"                 // Textura
    );

    edificio5 = new Objeto(
        POSICION_EDIFICIO5,                             // Posición 1
        ESCALA_EDIFICIO5,                                // Escala EDIFICIO
        COLOR_BLANCO,                                   // Color
        glm::vec3(0.0f, 0.0f, 0.0f),                    // Rotación
        VAOCubo,
        "textures/edificios/edificio2.png"                 // Textura
    );

    edificio6 = new Objeto(
        POSICION_EDIFICIO6,                             // Posición 1
        ESCALA_EDIFICIO6,                                // Escala EDIFICIO
        COLOR_BLANCO,                                   // Color
        glm::vec3(0.0f, 0.0f, 0.0f),                    // Rotación
        VAOCubo,
        "textures/edificios/edificio2.png"                 // Textura
    );

    edificio7 = new Objeto(
        POSICION_EDIFICIO7,                             // Posición 1
        ESCALA_EDIFICIO7,                                // Escala EDIFICIO
        COLOR_BLANCO,                                   // Color
        glm::vec3(0.0f, 0.0f, 0.0f),                    // Rotación
        VAOCubo,
        "textures/edificios/edificio2.png"              // Textura
    );

    edificio8 = new Objeto(
        POSICION_EDIFICIO8,                             // Posición 1
        ESCALA_EDIFICIO8,                                // Escala EDIFICIO
        COLOR_BLANCO,                                   // Color
        glm::vec3(0.0f, 0.0f, 0.0f),                    // Rotación
        VAOCubo,
        "textures/edificios/edificio1.png"                // Textura
    );

    edificio9 = new Objeto(
        POSICION_EDIFICIO9,                             // Posición 1
        ESCALA_EDIFICIO9,                                // Escala EDIFICIO
        COLOR_BLANCO,                                   // Color
        glm::vec3(0.0f, 0.0f, 0.0f),                    // Rotación
        VAOCubo,
        "textures/edificios/edificio1.png"                  // Textura
    );
}

void crearTorretaEnemiga() {
    // Base en forma de cruz (dos partes)
    baseTorreta1 = new Objeto(
        POSICION_TORRETAENEMIGA_BASE,
        ESCALA_TORRETAENEMIGA_BASE,
        COLOR_GRIS_OSCURO,
        glm::vec3(0.0f, rotacionTorreta, 0.0f),
        VAOCubo
    );

    baseTorreta2 = new Objeto(
        POSICION_TORRETAENEMIGA_BASE,
        glm::vec3(0.2f, 0.1f, 1.0f), // Escala diferente para el otro brazo de la cruz
        COLOR_GRIS_OSCURO,
        glm::vec3(0.0f, rotacionTorreta, 0.0f),
        VAOCubo
    );

    // Poste central
    posteTorreta = new Objeto(
        POSICION_TORRETAENEMIGA_POSTE,
        ESCALA_TORRETAENEMIGA_POSTE,
        COLOR_GRIS,
        glm::vec3(0.0f, rotacionTorreta, 0.0f),
        VAOCilindro
    );

    // Cabeza de la torreta
    cabezaTorreta = new Objeto(
        POSICION_TORRETAENEMIGA_CABEZA,
        ESCALA_TORRETAENEMIGA_CABEZA,
        COLOR_GRIS_CLARO,
        glm::vec3(0.0f, rotacionTorreta, 0.0f),
        VAOCubo
    );

    // Cañón de la torreta
    canonTorreta = new Objeto(
        POSICION_TORRETAENEMIGA_CANON,
        ESCALA_TORRETAENEMIGA_CANON,
        COLOR_GRIS_OSCURO,
        glm::vec3(0.0f, rotacionTorreta, 0.0f),
        VAOCilindro
    );
}

void crearObjetos() {

    // Crear VAOs y VBOs
    crearEjes();
    crearCubo();
    crearPlano();
    crearCilindro();
    crearSkybox();

    skyBox = new Objeto(
        POSICION_CERO + glm::vec3(0, -10, 0),                   // Posición (se actualizará en tiempo real)
        glm::vec3(100000, 100000, 100000),                            // Escala
        COLOR_AZUL_CIELO,                                       // Color azul cielo
        glm::vec3(0.0f, 0.0f, 0.0f),                            // Rotación
        VAOSkybox                                               // Usar el VAO del domo
    );

    suelo = new Objeto(
        POSICION_CERO,
        glm::vec3(31.0f, 31.0f, 31.0f),
        COLOR_BLANCO,  // Color blanco para no afectar la textura
        glm::vec3(0.0f, 0.0f, 0.0f),
        VAOPlano,
        "textures/arena2.png"  // Ruta de la textura
    );

    ejes = new Objeto(
        glm::vec3(cero, cero, cero),
        glm::vec3(15, 15, 15),
        COLOR_AZUL,
        glm::vec3(cero, cero, cero),
        VAOEjes);


	// Crear mapa
    crearEdificios();
    crearTanqueRoto();




    // Crear el tanque
    crearTanque();



    // Crear la torreta enemiga
    crearTorretaEnemiga();
}





/*------------------------ DIBUJAR ------------------------*/
void dibujarCielo(glm::mat4 view) {
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    skyBox->dibujar(view);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);

}

void dibujarEscena(glm::mat4 view) {
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    if (verEjes) ejes->dibujar(view);
    suelo->dibujar(view);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);
}

void dibujarTanqueRoto(glm::mat4 view) {
    // CUERPO DEL  TANQUE
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    cuerpoDelTanqueRoto->dibujar(view);
    cuerpoDelTanque2Roto->dibujar(view);

    cabezaDelTanqueRoto->dibujar(view);
    cabezaDelTanque2Roto->dibujar(view);
    cabezaDelTanque3Roto->dibujar(view);

    canhonDelTanqueRoto->dibujar(view);
    canhonDelTanque2Roto->dibujar(view);
    canhonDelTanque3Roto->dibujar(view);

    cintaDeLasRuedasIzqRoto->dibujar(view);
    cintaDeLasRuedasDerRoto->dibujar(view);

    ruedaDelTanque1Roto->dibujar(view);
    ruedaDelTanque2Roto->dibujar(view);
    ruedaDelTanque3Roto->dibujar(view);
    ruedaDelTanque4Roto->dibujar(view);
    ruedaDelTanque5Roto->dibujar(view);
    ruedaDelTanque6Roto->dibujar(view);

    llantaDelTanque1Roto->dibujar(view);
    llantaDelTanque2Roto->dibujar(view);
    llantaDelTanque3Roto->dibujar(view);
    llantaDelTanque4Roto->dibujar(view);
    llantaDelTanque6Roto->dibujar(view);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);
    //*/
}

void dibujarMapa(glm::mat4 view) {
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    /*---------------- EDIFICIOS ------------------*/
    edificio1->dibujar(view);
    edificio2->dibujar(view);
    edificio3->dibujar(view);

    edificio4->dibujar(view);
    edificio5->dibujar(view);
    edificio6->dibujar(view);

    edificio7->dibujar(view);
    edificio8->dibujar(view);
    edificio9->dibujar(view);

    /*---------------- OBSTACULOS ------------------*/
    dibujarTanqueRoto(view);

    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);
}

void dibujarRuedas(glm::mat4 view) {
    /*              RUEDAS GIRADAS          */
    // Dibujar ruedas izquierdas
    ruedaDelTanque1->posicion = obtenerPuntoCirculo(
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 0, 0.55).x,
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 0, 0.55).z,
        rotacionTanque,
        90,
        0.65f);
    ruedaDelTanque1->rotacion = glm::vec3(giroRuedas, rotacionTanque + rotacionRuedas, cero);
    ruedaDelTanque1->dibujar(view);

    ruedaDelTanque2->posicion = obtenerPuntoCirculo(posicionTanqueX,
        posicionTanqueZ,
        rotacionTanque,
        90,
        0.65f);
    ruedaDelTanque2->rotacion = glm::vec3(giroRuedas, rotacionTanque + rotacionRuedas, cero);
    ruedaDelTanque2->dibujar(view);

    ruedaDelTanque3->posicion = obtenerPuntoCirculo(
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.55).x,
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.55).z,
        rotacionTanque,
        90,
        0.65f);
    ruedaDelTanque3->rotacion = glm::vec3(giroRuedas, rotacionTanque + rotacionRuedas, cero);
    ruedaDelTanque3->dibujar(view);




    // Dibujar ruedas derechas
    ruedaDelTanque4->posicion = obtenerPuntoCirculo(
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 0, 0.55).x,
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 0, 0.55).z,
        rotacionTanque,
        -90,
        0.65f);
    ruedaDelTanque4->rotacion = glm::vec3(giroRuedas, rotacionTanque + rotacionRuedas, cero);
    ruedaDelTanque4->dibujar(view);

    ruedaDelTanque5->posicion = obtenerPuntoCirculo(
        posicionTanqueX,
        posicionTanqueZ,
        rotacionTanque,
        -90,
        0.65f);
    ruedaDelTanque5->rotacion = glm::vec3(giroRuedas, rotacionTanque + rotacionRuedas, cero);
    ruedaDelTanque5->dibujar(view);

    ruedaDelTanque6->posicion = obtenerPuntoCirculo(
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.55).x,
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.55).z,
        rotacionTanque,
        -90,
        0.65f);
    ruedaDelTanque6->rotacion = glm::vec3(giroRuedas, rotacionTanque + rotacionRuedas, cero);
    ruedaDelTanque6->dibujar(view);
    //*/



    /*              RUEDAS GIRADAS          */
    // Dibujar ruedas izquierdas giradas
    ruedaDelTanque1->posicion = obtenerPuntoCirculo(
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 0, 0.55).x,
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 0, 0.55).z,
        rotacionTanque,
        90,
        0.65f);
    ruedaDelTanque1->rotacion = glm::vec3(giroRuedas + 45, rotacionTanque + rotacionRuedas, cero);
    ruedaDelTanque1->dibujar(view);

    ruedaDelTanque2->posicion = obtenerPuntoCirculo(posicionTanqueX,
        posicionTanqueZ,
        rotacionTanque,
        90,
        0.65f);
    ruedaDelTanque2->rotacion = glm::vec3(giroRuedas + 45, rotacionTanque + rotacionRuedas, cero);
    ruedaDelTanque2->dibujar(view);

    ruedaDelTanque3->posicion = obtenerPuntoCirculo(
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.55).x,
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.55).z,
        rotacionTanque,
        90,
        0.65f);
    ruedaDelTanque3->rotacion = glm::vec3(giroRuedas + 45, rotacionTanque + rotacionRuedas, cero);
    ruedaDelTanque3->dibujar(view);





    // Dibujar ruedas derechas giradas
    ruedaDelTanque4->posicion = obtenerPuntoCirculo(
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 0, 0.55).x,
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 0, 0.55).z,
        rotacionTanque,
        -90,
        0.65f);
    ruedaDelTanque4->rotacion = glm::vec3(giroRuedas + 45, rotacionTanque + rotacionRuedas, cero);
    ruedaDelTanque4->dibujar(view);

    ruedaDelTanque5->posicion = obtenerPuntoCirculo(
        posicionTanqueX,
        posicionTanqueZ,
        rotacionTanque,
        -90,
        0.65f);
    ruedaDelTanque5->rotacion = glm::vec3(giroRuedas + 45, rotacionTanque + rotacionRuedas, cero);
    ruedaDelTanque5->dibujar(view);

    ruedaDelTanque6->posicion = obtenerPuntoCirculo(
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.55).x,
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.55).z,
        rotacionTanque,
        -90,
        0.65f);
    ruedaDelTanque6->rotacion = glm::vec3(giroRuedas + 45, rotacionTanque + rotacionRuedas, cero);
    ruedaDelTanque6->dibujar(view);
    //*/


    /*              LLANTAS           */
     // Dibujar llantas izquierdas 
    llantaDelTanque1->posicion = obtenerPuntoCirculo(
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 0, 0.55).x,
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 0, 0.55).z,
        rotacionTanque,
        90,
        0.6f);
    llantaDelTanque1->rotacion = glm::vec3(-90.0f, cero, -rotacionCintas + rotacionRuedas);
    llantaDelTanque1->dibujar(view);

    llantaDelTanque2->posicion = obtenerPuntoCirculo(posicionTanqueX,
        posicionTanqueZ,
        rotacionTanque,
        90,
        0.6f);
    llantaDelTanque2->rotacion = glm::vec3(-90.0f, cero, -rotacionCintas + rotacionRuedas);
    llantaDelTanque2->dibujar(view);

    llantaDelTanque3->posicion = obtenerPuntoCirculo(
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.55).x,
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.55).z,
        rotacionTanque,
        90,
        0.6f);
    llantaDelTanque3->rotacion = glm::vec3(-90.0f, cero, -rotacionCintas + rotacionRuedas);
    llantaDelTanque3->dibujar(view);






    // Dibujar llantas derechas
    llantaDelTanque4->posicion = obtenerPuntoCirculo(
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 0, 0.55).x,
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 0, 0.55).z,
        rotacionTanque,
        -90,
        0.6f);
    llantaDelTanque4->rotacion = glm::vec3(90.0f, cero, rotacionCintas - rotacionRuedas);
    llantaDelTanque4->dibujar(view);

    llantaDelTanque5->posicion = obtenerPuntoCirculo(
        posicionTanqueX,
        posicionTanqueZ,
        rotacionTanque,
        -90,
        0.6f);
    llantaDelTanque5->rotacion = glm::vec3(90.0f, cero, rotacionCintas - rotacionRuedas);
    llantaDelTanque5->dibujar(view);

    llantaDelTanque6->posicion = obtenerPuntoCirculo(
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.55).x,
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.55).z,
        rotacionTanque,
        -90,
        0.6f);
    llantaDelTanque6->rotacion = glm::vec3(90.0f, cero, rotacionCintas - rotacionRuedas);
    llantaDelTanque6->dibujar(view);
    //*/
}

void dibujarTanque(glm::mat4 view) {
    glm::vec3 posicionTanque = glm::vec3(posicionTanqueX, 0.0f, posicionTanqueZ);

    // CUERPO DEL  TANQUE
    cuerpoDelTanque->posicion = posicionTanque + POSICION_CUERPODELTANQUE;
    cuerpoDelTanque->rotacion = glm::vec3(0.0f, rotacionTanque, 0.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    cuerpoDelTanque->dibujar(view);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);

    cuerpoDelTanque2->posicion = posicionTanque + POSICION_CUERPODELTANQUE2;
    cuerpoDelTanque2->rotacion = glm::vec3(0.0f, rotacionTanque, 0.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    cuerpoDelTanque2->dibujar(view);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);
    //*/




    //  CABEZA DEL TANQUE
    cabezaDelTanque->posicion = obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.4) + glm::vec3(0.0f, 0.4f, 0.0f);
    cabezaDelTanque->rotacion = glm::vec3(0.0f, rotacionTorreta, 0.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    cabezaDelTanque->dibujar(view);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);


    cabezaDelTanque2->posicion = obtenerPuntoCirculo(obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.4).x,
        obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.4).z,
        rotacionTorreta, 180, 0.2) + glm::vec3(0.0f, 0.52f, 0.0f);
    cabezaDelTanque2->rotacion = glm::vec3(0.0f, rotacionTorreta, 0.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    cabezaDelTanque2->dibujar(view);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);


    cabezaDelTanque3->posicion = obtenerPuntoEsfera(glm::vec3(obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.4).x, 0.4f, obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.4).z),
        rotacionTanque, giroTorreta, inclinacionCanon, 0.2) + glm::vec3(0.0f, 0.35f, 0.0f);
    cabezaDelTanque3->rotacion = glm::vec3(0.0f, rotacionTorreta, 0.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    cabezaDelTanque3->dibujar(view);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);
    //*/




    //  CAÑON
    canhonDelTanque->posicion = obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.4) + glm::vec3(0.0f, 0.4f, 0.0f);
    canhonDelTanque->rotacion = glm::vec3(0.0f, rotacionCanhon, inclinacionCanon);
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    canhonDelTanque->dibujar(view);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);

    canhonDelTanque2->posicion = obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.4) + glm::vec3(0.0f, 0.4f, 0.0f);
    canhonDelTanque2->rotacion = glm::vec3(0.0f, rotacionCanhon, inclinacionCanon);
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    canhonDelTanque2->dibujar(view);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);

    canhonDelTanque3->posicion = obtenerPuntoEsfera(glm::vec3(obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.4).x, 0.35f, obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 180, 0.4).z),
        rotacionTanque, giroTorreta, inclinacionCanon, 1.8) + glm::vec3(0.0f, 0.4f, 0.0f);
    canhonDelTanque3->rotacion = glm::vec3(0.0f, rotacionCanhon, inclinacionCanon);
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    canhonDelTanque3->dibujar(view);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);





    //CINTAS
    cintaDeLasRuedasIzq->posicion = obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, 90, radioCirucloTanque + 0.001f);
    cintaDeLasRuedasIzq->rotacion = glm::vec3(-90.0f, 0, -rotacionCintas);
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    cintaDeLasRuedasIzq->dibujar(view);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);


    cintaDeLasRuedasDer->posicion = obtenerPuntoCirculo(posicionTanqueX, posicionTanqueZ, rotacionTanque, -90, radioCirucloTanque + 0.001f);
    cintaDeLasRuedasDer->rotacion = glm::vec3(90.0f, 0, rotacionCintas); // Solo rotación Y
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    cintaDeLasRuedasDer->dibujar(view);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);

    //*/

    //  RUEDAS
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    dibujarRuedas(view);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);
    //*/
}

void dibujarTorretaEnemiga(glm::mat4 view, glm::vec3 posicionMundo) {
    // Base (cruz)
    baseTorreta1->posicion = posicionMundo + POSICION_TORRETAENEMIGA_BASE;
    baseTorreta1->rotacion = glm::vec3(0.0f, rotacionTorreta, 0.0f);
    baseTorreta1->dibujar(view);

    baseTorreta2->posicion = posicionMundo + POSICION_TORRETAENEMIGA_BASE;
    baseTorreta2->rotacion = glm::vec3(0.0f, rotacionTorreta, 0.0f);
    baseTorreta2->dibujar(view);

    // Poste
    posteTorreta->posicion = posicionMundo + POSICION_TORRETAENEMIGA_POSTE;
    posteTorreta->rotacion = glm::vec3(0.0f, rotacionTorreta, 0.0f);
    posteTorreta->dibujar(view);

    // Cabeza
    cabezaTorreta->posicion = posicionMundo + POSICION_TORRETAENEMIGA_CABEZA;
    cabezaTorreta->rotacion = glm::vec3(0.0f, rotacionTorreta, 0.0f);
    cabezaTorreta->dibujar(view);

    // Cañón
    canonTorreta->posicion = obtenerPuntoCirculo(
        posicionMundo.x + POSICION_TORRETAENEMIGA_CABEZA.x,
        posicionMundo.z + POSICION_TORRETAENEMIGA_CABEZA.z,
        rotacionTorreta,
        0,
        0.3f
    ) + glm::vec3(0.0f, POSICION_TORRETAENEMIGA_CABEZA.y, 0.0f);
    canonTorreta->rotacion = glm::vec3(0.0f, rotacionTorreta, 0.0f);
    canonTorreta->dibujar(view);
}

void display() {
    // Actualiza el uniform en el shader
    glUseProgram(shaderProgram);

    // Especificamos el color del fondo
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    // Limpiamos el buffer de color y el buffer de profundidad
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Especificamos el viewport de OpenGL
    // En este caso el viewport va desde x = 0, y = 0, a x = 800, y = 800
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 120.0f);
    glm::mat4 view = myCamara();



    //DIBUJADO DE LA ESCENA
    dibujarEscena(view);


    dibujarMapa(view);


    //Dibujado del entorno
    dibujarTanque(view);
    //dibujarTorretaEnemiga(view, projection, glm::vec3(5.0f, 0.0f, 5.0f));


    // Dibujamos el cielo lo ultimo para salvar algo de rendimiento al no dibujar los pixeles que no se ven
    dibujarCielo(view);
}







/*------------------------ MAIN ------------------------*/
int main() {
    // Inicializar GLFW
    glfwInit();
    //Decirle a OPENGL que queremos version queremos usar
    //En este caso estamos usando OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Decirle a GLFW que usamos el CORE profile
    // Eso quiere decir que solo usamos funciones modernas de OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create una GLFWwindow (ventana) de 800 x 800 pixels, con el nombre "Minijuego Tanque"
    //Comrpobar luego si da error y terminar si es necesario
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Minijuego Tanque", NULL, NULL);
    if (window == NULL) {
        std::cout << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Hacer de esta ventana la ventanad del contexto actual
    glfwMakeContextCurrent(window);


    //Teclado y pantallas
    glfwSetKeyCallback(window, teclado);
    glfwSetWindowSizeCallback(window, tamanhoPantalla);

    // Inicializar GLAD para que configure OPENGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Error al inicializar GLAD" << std::endl;
        return -1;
    }





    //codigo principal
    openGlInit();

    // Crear shader y geometrías
    shaderProgram = generarShaders(vertexShaderSource, fragmentShaderSource);
    skyboxShader = generarShaders(vertexShaderSourceSkybox, fragmentShaderSourceSkybox);
    shadowShader = generarShaders(vertexShaderSourceSun, fragmentShaderSourceSun);


    // Crear objetos de la escena
    crearObjetos();

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        //Calculo de los fps y display en el titulo de la ventana
        calculoFPS(window);

        //DISPLAY PRINCIAPAL; SE ENCARGA DE DIBUJAR LA ESCENA Y TODO LO QUE LO RODEA
        display();

        //Cambia el buffer de la ventana con el buffer frontal
        glfwSwapBuffers(window);
        // Se encarga de manekar todos los eventos de GLFW
        glfwPollEvents();
    }





    // Limpieza
    glDeleteVertexArrays(1, &VAOEjes);
    glDeleteVertexArrays(1, &VAOCubo);
    glDeleteVertexArrays(1, &VAOPlano);
    glDeleteVertexArrays(1, &VAOCilindro);
    glDeleteVertexArrays(1, &VAOSkybox);


    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}