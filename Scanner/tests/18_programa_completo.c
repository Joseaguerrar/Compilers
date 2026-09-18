/* ========================================================
 * TEST 18: Programa C completo y realista
 * Objetivo: Un programa C completo con structs, punteros,
 *           arrays, ciclos, funciones, switch, etc. para
 *           verificar el scanner de punta a punta.
 * ======================================================== */

#define TAMANO 10
#define PI 3.14159

typedef struct {
    int id;
    char nombre[50];
    double salario;
} Empleado;

static int contador = 0;

void inicializar(Empleado *emp, int id, double sal) {
    emp->id = id;
    emp->salario = sal;
    contador++;
}

int buscar(Empleado empleados[], int n, int id) {
    for (int i = 0; i < n; i++) {
        if (empleados[i].id == id) {
            return i;
        }
    }
    return -1;
}

double calcular(int tipo, double valor) {
    double resultado;

    switch (tipo) {
        case 0:
            resultado = valor * PI;
            break;
        case 1:
            resultado = valor + 1.0;
            break;
        default:
            resultado = 0.0;
            break;
    }

    return resultado;
}

int main(void) {
    Empleado lista[TAMANO];
    int i;

    for (i = 0; i < TAMANO; i++) {
        inicializar(&lista[i], i + 1, 1000.0 * (i + 1));
    }

    int pos = buscar(lista, TAMANO, 5);

    if (pos >= 0) {
        double val = calcular(0, lista[pos].salario);
        unsigned long total = (unsigned long)val;
    } else {
        do {
            i--;
        } while (i > 0);
    }

    return 0;
}
