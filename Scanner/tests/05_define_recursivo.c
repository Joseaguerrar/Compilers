/* ========================================================
 * TEST 05: Expansión recursiva de macros
 * Objetivo: Verificar que las macros se expanden
 *           correctamente cuando una referencia a otra.
 *           A -> 10, B -> A + 5, C -> B * 2
 * ======================================================== */

#define A 10
#define B A + 5
#define C B * 2

int main() {
    int x = A;
    int y = B;
    int z = C;
    return 0;
}
