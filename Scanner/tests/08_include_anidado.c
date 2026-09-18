/* ========================================================
 * TEST 08: #include anidado
 * Objetivo: Verificar que archivos incluidos que a su vez
 *           incluyen otros archivos se procesan bien.
 *           Este archivo incluye 08_header_a.h
 * ======================================================== */

#include "08_header_a.h"

int main() {
    int x = VALOR_A;
    int y = VALOR_B;
    return 0;
}
