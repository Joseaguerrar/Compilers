/* ========================================================
 * TEST 12: Todos los operadores de C
 * Objetivo: Verificar que cada operador se reconoce con su
 *           código correcto, incluyendo operadores compuestos
 *           vs simples (ej. = vs ==, + vs ++).
 * ======================================================== */

int main() {
    int a, b, c;

    /* Aritméticos */
    c = a + b;
    c = a - b;
    c = a * b;
    c = a / b;
    c = a % b;

    /* Incremento y Decremento */
    a++;
    b--;

    /* Asignación y Comparación */
    a = b;
    if (a == b) {}
    if (a != b) {}

    /* Relacionales */
    if (a < b) {}
    if (a > b) {}
    if (a <= b) {}
    if (a >= b) {}

    /* Lógicos */
    if (a && b) {}
    if (a || b) {}
    if (!a) {}

    /* Acceso a miembros */
    /* ptr->campo; */

    return 0;
}
