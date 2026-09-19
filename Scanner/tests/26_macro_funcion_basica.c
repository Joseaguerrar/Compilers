/* ========================================================
 * TEST 26: Macros con argumentos - casos basicos
 * Objetivo: Definiciones y llamadas con distintos espacios,
 *           cero parametros, continuaciones \ y llamadas
 *           partidas en varias lineas.
 * Esperado:
 *   a = (1 < 2);   b = (1 >= 2);   c = 42 + 42 + 42;
 *   d = ((3) * (3));   e = ((4) + (5) + (6));
 *   f = (7 - 8);   g = ((1) + (2)) (en varias lineas)
 *   h = SQ;   i = (SQ);  (SQ sin parentesis NO se expande)
 *   j = (z)   (DEFINE con espacio antes de '(' es objeto)
 * ======================================================== */

#define MAX(x,y) (x < y)
#define MIN(  x ,   y  )   (x >= y)
#define F() 42
#define SQ(n) ((n) * (n))
#define SUM3(a, \
             b, \
             c) ((a) + \
                 (b) + (c))
#define SUB(a,b) (a - b) /* comentario */
#define ADD(a,b) ((a) + (b)) // comentario
#define OBJ (z)

int main() {
    int a = MAX(1, 2);
    int b = MIN(1,2);
    int c = F() + F ( ) + F(
    );
    int d = SQ(3);
    int e = SUM3(4, 5, 6);
    int f = SUB(7, 8);
    int g = ADD(
        1,
        2
    );
    int h = SQ;
    int i = (SQ);
    int j = OBJ;
    int SQ2 = 1;
    return a + b + c + d + e + f + g + j + SQ2;
}
