/* ========================================================
 * TEST 30: Macros autorreferentes y mutuamente recursivas
 * Objetivo: El preprocesador no debe entrar en un ciclo
 *           infinito ni dar Segmentation Fault.
 *           En C estandar: f(1) -> f(1), A(1) -> A(1),
 *           foo -> foo(foo) (la macro no se reexpande).
 * ======================================================== */

#define f(x) f(x)
#define A(x) B(x)
#define B(x) A(x)
#define foo foo(foo)
#define LOOP(x) LOOP(LOOP(x))

int main() {
    int a = f(1);
    int b = A(1);
    int c = foo;
    int d = LOOP(2);
    int e = 99;
    return a + b + c + d + e;
}
