/* ========================================================
 * TEST 23: Define con string que contiene nombre de macro
 * Objetivo: Verificar que si defines MSG como "MAX es 100"
 *           y MAX como 999, el preprocesador NO expande
 *           MAX dentro de la string "MAX es 100".
 * ======================================================== */

#define MAX 999
#define MSG "MAX es 100"

int main() {
    char *s = MSG;
    int x = MAX;
    return 0;
}
