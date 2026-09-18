/* ========================================================
 * TEST 20: Comentario con contenido engañoso
 * Objetivo: Verificar que el preprocesador ignora
 *           directivas y código dentro de comentarios.
 *           Nada de esto debe ser procesado.
 * ======================================================== */

/* #include "no_existe.h" */
/* #define TRAP 999 */

// #include "tampoco.h"
// #define OTRA_TRAP 888

int main() {
    /* int x = TRAP; */
    // int y = OTRA_TRAP;
    int z = 0;
    return z;
}
