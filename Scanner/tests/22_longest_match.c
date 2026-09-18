/* ========================================================
 * TEST 22: Operadores compuestos vs simples adyacentes
 * Objetivo: Verificar que el scanner aplica "longest match"
 *           correctamente. "++" debe ganar sobre "+" + "+".
 *           "==" debe ganar sobre "=" + "=".
 * ======================================================== */

int main() {
    int a = 0;

    a++;        /* Debe ser: a ++ y no a + + */
    a--;        /* Debe ser: a -- y no a - - */

    if (a == 0) {}  /* == no = = */
    if (a != 1) {}  /* != no ! = */
    if (a <= 2) {}  /* <= no < = */
    if (a >= 3) {}  /* >= no > = */

    int *p;
    p->a;       /* -> no - > */

    return 0;
}
