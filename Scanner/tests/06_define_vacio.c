/* ========================================================
 * TEST 06: #define vacío (sin valor)
 * Objetivo: Verificar que macros sin valor no causan
 *           errores y se reemplazan por nada.
 * ======================================================== */

#define GUARD
#define EMPTY

int main() {
    int GUARD = 1;
    int EMPTY = 2;
    return 0;
}
