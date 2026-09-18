/* ========================================================
 * TEST 09: Caracteres ilegales / errores léxicos
 * Objetivo: El scanner debe reportar TOK_ERROR para
 *           caracteres que no pertenecen al lenguaje C.
 * ======================================================== */

int main() {
    int x = 10;
    int y = @;
    int z = $;
    int w = ~;
    int v = `;
    char c = #;
    return 0;
}
