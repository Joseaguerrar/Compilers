/* ========================================================
 * TEST 28: Argumentos dificiles
 * Objetivo: Comas y parentesis dentro de strings, chars y
 *           parentesis anidados no deben separar argumentos.
 *           Los parametros solo se reemplazan como
 *           identificadores completos, nunca dentro de
 *           strings o de otros identificadores.
 * Esperado:
 *   a = ((1,2) , (3))
 *   b = ("a,b)" , 'c')
 *   c = (',' , ')')
 *   d = ("x\",(y" , "\\")
 *   e = (x_1 + xs + 7 + "x")        (solo el x suelto cambia)
 *   f = (0x1F , 1e5)                 (numeros intactos)
 *   g = ( , )                        (argumentos vacios)
 *   h = ((1 , 2) , ((3)))
 * ======================================================== */

#define PAIR(a, b) (a , b)
#define ONLYX(x) (x_1 + xs + x + "x")

int main() {
    int a = PAIR((1,2), (3));
    char *b = PAIR("a,b)", 'c');
    char c = PAIR(',', ')');
    char *d = PAIR("x\",(y", "\\");
    int e = ONLYX(7);
    int f = PAIR(0x1F, 1e5);
    int g = PAIR(,);
    int h = PAIR((1 , 2), ((3)));
    char *s = "PAIR(1, 2) no se expande";
    return a + e + f + h;
}
