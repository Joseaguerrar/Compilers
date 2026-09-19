/* ========================================================
 * TEST 27: Macros con argumentos anidadas
 * Objetivo: Llamadas dentro de argumentos, macros cuyo
 *           cuerpo llama a otras macros, macros objeto como
 *           argumento y macros pasadas como "funcion".
 * Esperado:
 *   a = ((((1) > (2) ? (1) : (2))) > (3) ? (((1) > (2) ? (1) : (2))) : (3))
 *   b = ((((5) * (5))) * (((5) * (5))))   (N = 5 dentro del arg)
 *   c = ((2) * (2))                   (APPLY(SQ, 2) -> SQ(2) al reescanear)
 *   d = ((10) + (((1) * (1))))        (cuerpo de CALL usa SQ)
 *   e = 5 + 5                         (objeto dentro de funcion)
 *   f = ((1) > (2) ? (1) : (2))  (FN -> MAX y luego se une
 *       con el '(' que sigue, como en C estandar)
 *   g = SQ anidado 6 veces, sin error
 * ======================================================== */

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define SQ(x) ((x) * (x))
#define N 5
#define APPLY(f, v) f(v)
#define CALL(v) ((v) + (SQ(1)))
#define TWICE(x) x + x
#define FN MAX

int main() {
    int a = MAX(MAX(1, 2), 3);
    int b = SQ(SQ(N));
    int c = APPLY(SQ, 2);
    int d = CALL(10);
    int e = TWICE(N);
    int f = FN(1, 2);
    int g = SQ(SQ(SQ(SQ(SQ(SQ(1))))));
    return a + b + c + d + e + f + g;
}
