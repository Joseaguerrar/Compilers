/*
 * invalid_03_control_flow.c
 * Programa de prueba con múltiples errores sintácticos en estructuras de control
 * (if/else, for, while, do-while, switch/case).
 * Diseñado para probar detección de errores y recuperación de errores (error recovery).
 */

#include <stdio.h>

int main(void) {
    int contador = 0;
    int limite = 10;

    /* ERROR 1: Sentencia if sin paréntesis en la condición */
    if contador < limite {
        contador++;
    }

    /* ERROR 2: Sentencia else sin bloque o sentencia if correspondiente previa */
    else {
        contador = 0;
    }

    /* ERROR 3: Bucle for con sólo dos secciones (falta un punto y coma separador) */
    for (int i = 0; i < limite) {
        contador += i;
    }

    /* ERROR 4: Bucle for con demasiados puntos y coma en el encabezado */
    for (int j = 0; j < 5; j++; ;) {
        contador--;
    }

    /* ERROR 5: Bucle while con condición vacía dentro de los paréntesis */
    while () {
        contador++;
        if (contador > 50) break;
    }

    /* ERROR 6: Bucle do-while sin condición while al final */
    do {
        contador += 2;
    } (contador < 20);

    /* ERROR 7: Switch con expresión de caso sin dos puntos ':' */
    int opcion = 2;
    switch (opcion) {
        case 1
            printf("Opcion 1\n");
            break;
        case 2:
            printf("Opcion 2\n");
            break;
        /* ERROR 8: default sin dos puntos */
        default
            printf("Opcion desconocida\n");
            break;
    }

    /* ERROR 9: Llave de cierre faltante en bloque if */
    if (contador > 0) {
        printf("Contador positivo: %d\n", contador);
    /* Falta '}' */

    /* ERROR 10: Sentencia switch sin llaves de apertura y cierre */
    switch (opcion)
        case 1: printf("Uno\n"); break;

    return 0;
}
