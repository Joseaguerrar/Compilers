/* ========================================================
 * TEST 32: Errores en macros con argumentos
 * Objetivo: Cada caso debe reportar un error y el
 *           preprocesador debe continuar con el resto
 *           del archivo. La llamada sin cerrar va al
 *           final porque consume hasta EOF.
 * Errores esperados:
 *   - parametro invalido (1x), parametro duplicado,
 *     coma final en parametros, lista sin cerrar,
 *     macro redefinida, pocos argumentos, demasiados
 *     argumentos, argumentos a macro sin parametros,
 *     llamada sin cerrar.
 * ======================================================== */

#define BAD1(1x) x
#define BAD2(a, a) a
#define BAD3(a,) a
#define BAD4(a, b a + b
#define TWO(a, b) (a + b)
#define TWO(a) a
#define NONE() 0

int main() {
    int a = TWO(1);
    int b = TWO(1, 2, 3);
    int c = NONE(1);
    int d = TWO(1, 2);
    int e = TWO(1,
