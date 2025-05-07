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



float cero = 0.0f;

//      CAMARA
glm::vec3 cameraPosition = glm::vec3(1.0f, 100.0f, 0.0f);





//     POSICIONES

glm::vec3 POSICION_SOL = glm::vec3(30, 100, -150);  // Posición alta para simular sol

glm::vec3 POSICION_CERO = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 POSICION_CUERPODELTANQUE = glm::vec3(0.0f, 0.35f, 0.0f);
glm::vec3 POSICION_CUERPODELTANQUE2 = glm::vec3(0.0f, 0.55f, 0.0f);

glm::vec3 POSICION_TORRETA = POSICION_CUERPODELTANQUE + glm::vec3(0.0f, 0.4f, -0.3f);
glm::vec3 POSICION_CANHON = POSICION_TORRETA + glm::vec3(0.0f, 0.0f, 0.0f);   //+0.4

glm::vec3 POSICION_CINTA_IZQ = POSICION_CUERPODELTANQUE + glm::vec3(-0.96f, -0.1f, 0.0f);
glm::vec3 POSICION_CINTA_DER = POSICION_CUERPODELTANQUE + glm::vec3(0.96f, -0.1f, 0.0f);

glm::vec3 POSICION_RUEDA_IZQ_1 = glm::vec3(-0.5f, 0.3f, -0.6f);
glm::vec3 POSICION_RUEDA_IZQ_2 = glm::vec3(-0.5f, 0.3f, 0.0f);
glm::vec3 POSICION_RUEDA_IZQ_3 = glm::vec3(-0.5f, 0.3f, 0.6f);
glm::vec3 POSICION_RUEDA_DER_1 = glm::vec3(0.5f, 0.3f, -0.6f);
glm::vec3 POSICION_RUEDA_DER_2 = glm::vec3(0.5f, 0.3f, 0.0f);
glm::vec3 POSICION_RUEDA_DER_3 = glm::vec3(0.5f, 0.3f, 0.6f);

glm::vec3 POSICION_TORRETAENEMIGA_BASE = glm::vec3(0.0f, 0.1f, 0.0f);
glm::vec3 POSICION_TORRETAENEMIGA_POSTE = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 POSICION_TORRETAENEMIGA_CABEZA = glm::vec3(0.0f, 2.0f, 0.0f);
glm::vec3 POSICION_TORRETAENEMIGA_CANON = glm::vec3(0.0f, 2.0f, 0.3f);


glm::vec3 POSICION_EDIFICIO1 = glm::vec3(20,	0,	25); 

glm::vec3 POSICION_EDIFICIO2 = glm::vec3(-20,	0,	25);  


glm::vec3 POSICION_EDIFICIO3 = glm::vec3(15,	0,	0);
glm::vec3 POSICION_EDIFICIO4 = glm::vec3(0,		0,	-10);
glm::vec3 POSICION_EDIFICIO5 = glm::vec3(-15,	0,	0);

glm::vec3 POSICION_EDIFICIO6 = glm::vec3(30, 0, 40);
glm::vec3 POSICION_EDIFICIO7 = glm::vec3(-30, 0, 40);
glm::vec3 POSICION_EDIFICIO8 = glm::vec3(40, 0, 40);
glm::vec3 POSICION_EDIFICIO9 = glm::vec3(-40, 0, 40);


//     ESCALA
glm::vec3 ESCALA_EDIFICIO1 = glm::vec3(10, 50, 40);

glm::vec3 ESCALA_EDIFICIO2 = glm::vec3(10, 50, 40);

glm::vec3 ESCALA_EDIFICIO3 = glm::vec3(20, 50, 10);
glm::vec3 ESCALA_EDIFICIO4 = glm::vec3(50, 50, 10);
glm::vec3 ESCALA_EDIFICIO5 = glm::vec3(20, 50, 10);

glm::vec3 ESCALA_EDIFICIO6 = glm::vec3(10, 50, 10);
glm::vec3 ESCALA_EDIFICIO7 = glm::vec3(10, 50, 10);

glm::vec3 ESCALA_EDIFICIO8 = glm::vec3(10, 50, 10);
glm::vec3 ESCALA_EDIFICIO9 = glm::vec3(10, 50, 10);


glm::vec3 ESCALA_COCHE = glm::vec3(0.9f, 0.4f, 1.8f);
glm::vec3 ESCALA_COCHE2 = glm::vec3(1.1f, 0.25f, 2.0f);

glm::vec3 ESCALA_TORRETA = glm::vec3(0.8f, 0.15f, 0.8f);
glm::vec3 ESCALA_TORRETA2 = glm::vec3(0.8f, 0.1f, 0.6f);
glm::vec3 ESCALA_TORRETA3 = glm::vec3(0.2f, 0.2f, 0.6f);

glm::vec3 ESCALA_CINTA_IZQ = glm::vec3(0.9f, 0.1f, 0.28f);
glm::vec3 ESCALA_CINTA_DER = glm::vec3(0.9f, 0.1f, 0.28f);
glm::vec3 ESCALA_RUEDAS = glm::vec3(0.03f, 0.15f, 0.15f);
glm::vec3 ESCALA_LLANTAS = glm::vec3(0.15f, 0.02f, 0.15f);

glm::vec3 ESCALA_TORRETAENEMIGA_BASE = glm::vec3(1.0f, 0.1f, 0.2f);
glm::vec3 ESCALA_TORRETAENEMIGA_POSTE = glm::vec3(0.1f, 1.0f, 0.1f);
glm::vec3 ESCALA_TORRETAENEMIGA_CABEZA = glm::vec3(0.5f, 0.3f, 0.5f);
glm::vec3 ESCALA_TORRETAENEMIGA_CANON = glm::vec3(0.05f, 0.05f, 0.8f);






//      COLORES
glm::vec3 COLOR_SOL = glm::vec3(1.0f, 0.95f, 0.9f);         // Color cálido del sol
glm::vec3 COLOR_SOL2 = glm::vec3(1.0f, 0.8f, 0.1f);

glm::vec3 COLOR_NEGRO = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 COLOR_BLANCO = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 COLOR_GRIS_OSCURO = glm::vec3(0.3f, 0.3f, 0.3f);
glm::vec3 COLOR_GRIS = glm::vec3(0.5f, 0.5f, 0.5f);
glm::vec3 COLOR_GRIS_CLARO = glm::vec3(0.7f, 0.7f, 0.7f);

glm::vec3 COLOR_ROJO = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 COLOR_VERDE = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 COLOR_AZUL = glm::vec3(0.0f, 0.0f, 1.0f);

glm::vec3 COLOR_AZUL_CIELO = glm::vec3(0.5f, 0.7f, 1.0f);
glm::vec3 COLOR_AMARILLO = glm::vec3(1.0f, 1.0f, 0.0f);
glm::vec3 COLOR_CYAN = glm::vec3(0.0f, 1.0f, 1.0f);
glm::vec3 COLOR_MAGENTA = glm::vec3(1.0f, 0.0f, 1.0f);

glm::vec3 COLOR_VERDE_HIERBA_OSCURA = glm::vec3(0.2f, 0.5f, 0.1f);
glm::vec3 COLOR_NARANJA = glm::vec3(1.0f, 0.5f, 0.0f);
glm::vec3 COLOR_VIOLETA = glm::vec3(0.5f, 0.0f, 0.5f); 

#pragma once
