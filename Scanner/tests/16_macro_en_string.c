/* ========================================================
 * TEST 16: Macro dentro de un string literal
 * Objetivo: Verificar que el preprocesador NO expande
 *           macros que están dentro de comillas.
 *           "MAX" debe quedar como "MAX", no como "100".
 * ======================================================== */

#define MAX 100
#define NAME "hello"

int main() {
    char *a = "MAX no debe ser reemplazado";
    char *b = "NAME tampoco";
    char c = 'M';
    int d = MAX;

    return 0;
}
