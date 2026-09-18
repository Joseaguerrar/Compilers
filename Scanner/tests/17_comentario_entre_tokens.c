/* ========================================================
 * TEST 17: Comentario entre tokens (no debe unirlos)
 * Objetivo: Verificar que un comentario entre dos tokens
 *           actúa como separador. "int" y "x" no deben
 *           fusionarse en "intx".
 * ======================================================== */

int/*comentario*/x = 5;
int   /*   otro   */   y = 10;
int/* a */main/* b */(/* c */void/* d */)/* e */{/* f */return/* g */0/* h */;/* i */}
