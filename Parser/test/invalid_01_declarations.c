/*
 * invalid_01_declarations.c
 * Programa de prueba con múltiples errores sintácticos en declaraciones.
 * Diseñado para probar detección de errores y recuperación de errores (error recovery).
 */

#include <stdio.h>

/* ERROR 1: Asignación global sin tipo de dato especificado */
x = 100;

/* ERROR 2: Tipos incompatibles duplicados en una misma declaración */
int double valor_invalido = 3.14;

/* ERROR 3: Definición de struct sin punto y coma después de los campos */
struct Persona {
    char nombre[50]
    int edad;
};

/* ERROR 4: Definición de struct sin punto y coma al final de la llave de cierre */
struct Coordenada {
    float x;
    float y;
}

/* ERROR 5: Declaración de arreglo con corchete sin cerrar */
int lista_numeros[10 = {1, 2, 3};

/* ERROR 6: Coma colgante sin identificador en lista de variables */
int variable_a, , variable_b = 50;

/* ERROR 7: Typedef incompleto sin nuevo nombre */
typedef unsigned long;

/* ERROR 8: Declaración de puntero con operador de dirección en vez de indirección */
int &puntero_invalido = 0;

int main(void) {
    /* ERROR 9: Inicialización con corchetes en lugar de llaves */
    int arr[3] = [1, 2, 3];

    /* ERROR 10: Declaración local con operador de asignación faltante */
    int total 42;

    printf("Fin de prueba de declaraciones invalidas\n");
    return 0;
}
