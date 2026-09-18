/* ========================================================
 * TEST 04: Macros duplicadas con #define
 * Objetivo: El preprocesador debe reportar un error cuando
 *           se define la misma macro dos veces.
 * ======================================================== */

#define MAX 100
#define MIN 0
#define MAX 200

int main() {
    int a = MAX;
    int b = MIN;
    return 0;
}
