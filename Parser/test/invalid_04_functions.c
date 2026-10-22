/*
 * invalid_04_functions.c
 * Programa de prueba con múltiples errores sintácticos en definiciones de funciones,
 * firmas, parámetros, sentencias de retorno y llamadas.
 * Diseñado para probar detección de errores y recuperación de errores (error recovery).
 */

#include <stdio.h>

/* ERROR 1: Parámetros de función sin tipos de datos */
int sumar_invalido(a, b) {
    return a + b;
}

/* ERROR 2: Coma al final de la lista de parámetros formales */
void procesar_datos(int x, float y,) {
    printf("Procesando: %d, %.2f\n", x, y);
}

/* ERROR 3: Prototipo de función con doble paréntesis de cierre */
int calcular_area(int base, int altura));

/* ERROR 4: Función con cuerpo que carece de llave de apertura */
int multiplicar(int a, int b)
    return a * b;
}

/* ERROR 5: Sentencia return con operador sin operando */
int obtener_valor(void) {
    return + ;
}

/* ERROR 6: Función void que intenta retornar un tipo en lugar de una expresión */
void imprimir_mensaje(void) {
    return int;
}

/* ERROR 7: Parámetro con especificador de tipo pero sin nombre en definición con cuerpo */
int operacion_mixta(int a, double) {
    return a * 2;
}

/* ERROR 8: Llamada a función con paréntesis sin cerrar antes del punto y coma */
void ejecutar_llamadas(void) {
    imprimir_mensaje(;
}

/* ERROR 9: Declaración de función anidada con sintaxis corrupta */
int contenedor(void) {
    int sub_funcion(int x {
        return x;
    }
    return sub_funcion(5);
}

int main(void) {
    /* ERROR 10: Retorno con doble punto y coma o punto y coma flotante en bloque */
    int r = sumar_invalido(10, 20);
    return ; ;
}
