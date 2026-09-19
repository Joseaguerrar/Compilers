/* ========================================================
 * TEST 29: Comentarios dentro de la llamada
 * Objetivo: Un comentario se comporta como espacio. Las
 *           comas y parentesis dentro de comentarios NO
 *           cuentan, y un comentario // no debe terminar
 *           dentro de la expansion.
 * Esperado:
 *   a = (1 < 2)
 *   b = (3 < 4)
 *   c = (5 < 6)
 *   d = (7 < 8)
 * ======================================================== */

#define MAX(x,y) (x < y)

int main() {
    int a = MAX(1 /* , ( */, 2);
    int b = MAX /* entre nombre y parentesis */ (3, 4);
    int c = MAX(5, // coma, parentesis ) aqui
                6);
    int d = MAX(7,
                /* comentario
                   multilinea ) , */ 8);
    return a + b + c + d;
}
