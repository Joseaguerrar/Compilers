/*
 * invalid_02_expressions.c
 * Programa de prueba con múltiples errores sintácticos en expresiones aritméticas y lógicas.
 * Diseñado para probar detección de errores y recuperación de errores (error recovery).
 */

#include <stdio.h>

int calcular(int a, int b) {
    return a + b;
}

int main(void) {
    int x = 10;
    int y = 20;
    int z = 0;

    /* ERROR 1: Operador binario sin operando intermedio */
    z = x + * y;

    /* ERROR 2: Paréntesis no balanceado en expresión aritmética */
    z = (x + y * (x - 2);

    /* ERROR 3: Asignación a un literal (rvalue como destino de asignación) */
    100 = x;

    /* ERROR 4: Operador de incremento aplicado a un número literal */
    z = 42++;

    /* ERROR 5: Operador ternario incompleto (falta rama falsa ':') */
    z = (x > y) ? x ;

    /* ERROR 6: Operador de flecha/acceso a miembro sin identificador a la derecha */
    struct { int campo; } obj;
    z = obj.;

    /* ERROR 7: Doble coma en argumentos de llamada a función */
    z = calcular(x, , y);

    /* ERROR 8: Corchete de indexación sin expresión interna ni cierre adecuado */
    int tabla[5] = {1, 2, 3, 4, 5};
    z = tabla[ + 2;

    /* ERROR 9: Operador lógico con operando faltante al final */
    if (x > 0 && ) {
        z = 1;
    }

    /* ERROR 10: Asignación compuesta con operador dividido por espacio */
    z + = 5;

    printf("Fin de prueba de expresiones invalidas\n");
    return 0;
}
