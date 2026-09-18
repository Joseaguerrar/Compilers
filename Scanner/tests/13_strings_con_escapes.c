/* ========================================================
 * TEST 13: Strings con secuencias de escape
 * Objetivo: Verificar que el scanner reconoce strings
 *           completos con escapes internos sin romperse.
 * ======================================================== */

int main() {
    char *a = "hello world";
    char *b = "tabs\there";
    char *c = "newline\nhere";
    char *d = "backslash\\here";
    char *e = "quote\"inside";
    char *f = "";
    char *g = "null\0char";
    char *h = "mixed\t\n\\\"escapes";
    char *i = "very long string with many many many words to test buffer handling";

    return 0;
}
