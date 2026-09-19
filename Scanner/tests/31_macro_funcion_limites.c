/* ========================================================
 * TEST 31: Limites del preprocesador
 * Objetivo: 32 parametros (MAX_ARGS) funcionan, 33 dan
 *           error; una expansion exponencial que supera
 *           MAX_VALUE y una anidacion mayor a MAX_DEPTH
 *           deben reportar error sin Segmentation Fault.
 *           La linea "int ok = 1;" debe llegar intacta.
 * ======================================================== */

#define P32(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,b0,b1,b2,b3,b4,b5,b6,b7,b8,b9,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,d0,d1) a0+d1
#define P33(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,b0,b1,b2,b3,b4,b5,b6,b7,b8,b9,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,d0,d1,d2) a0
#define D(x) x x x
#define I(x) x

int main() {
    int a = P32(1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2);
    int b = I(1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    int c = D(D(D(D(D(D(D(D(D(abcdefgh)))))))));
    int d = I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(I(1))))))))))))))))))))))))))))))))))))))));
    int ok = 1;
    return a + b + c + d + ok;
}
