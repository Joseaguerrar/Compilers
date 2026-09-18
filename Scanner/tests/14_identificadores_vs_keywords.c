/* ========================================================
 * TEST 14: Identificadores que parecen keywords
 * Objetivo: Verificar que nombres que contienen keywords
 *           como prefijo/sufijo se reconocen como
 *           IDENTIFIER y no como KEYWORD.
 *           Ej: "integer" no es "int", "format" no es "for".
 * ======================================================== */

int main() {
    int integer = 1;
    int formatting = 2;
    int doubles = 3;
    int shorter = 4;
    int longer = 5;
    int voided = 6;
    int returning = 7;
    int whileTrue = 8;
    int ifTrue = 9;
    int automate = 10;
    int breakpoint = 11;
    int _underscore = 12;
    int __double = 13;
    int a123 = 14;
    int _123abc = 15;

    return 0;
}
