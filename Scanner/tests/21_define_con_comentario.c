/* ========================================================
 * TEST 21: Define con comentario en la misma línea
 * Objetivo: Verificar que un comentario al final de un
 *           #define no se incluye en el valor de la macro.
 * ======================================================== */

#define VALOR 42 /* este comentario no es parte del valor */
#define OTRO 10 // tampoco este

int main() {
    int a = VALOR;
    int b = OTRO;
    return 0;
}
