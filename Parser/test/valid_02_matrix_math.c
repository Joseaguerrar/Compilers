/*
 * valid_02_matrix_math.c
 * Programa de prueba sintácticamente válido para C99/C11.
 * Biblioteca de álgebra lineal: Operaciones matriciales, eliminación gaussiana,
 * descomposición LU, cálculo de determinantes, vectores 3D y sistemas Ax = b.
 * Líneas: > 500
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define EPSILON 1e-9

/* ========================================================================= */
/* 1. Vector Tridimensional y N-dimensional                                 */
/* ========================================================================= */

typedef struct {
    double x;
    double y;
    double z;
} Vec3;

typedef struct {
    double *data;
    size_t dim;
} NVector;

Vec3 vec3_create(double x, double y, double z) {
    Vec3 v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

Vec3 vec3_add(Vec3 a, Vec3 b) {
    return vec3_create(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3 vec3_sub(Vec3 a, Vec3 b) {
    return vec3_create(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3 vec3_scale(Vec3 v, double scalar) {
    return vec3_create(v.x * scalar, v.y * scalar, v.z * scalar);
}

double vec3_dot(Vec3 a, Vec3 b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

Vec3 vec3_cross(Vec3 a, Vec3 b) {
    Vec3 result;
    result.x = (a.y * b.z) - (a.z * b.y);
    result.y = (a.z * b.x) - (a.x * b.z);
    result.z = (a.x * b.y) - (a.y * b.x);
    return result;
}

double vec3_length(Vec3 v) {
    return sqrt(vec3_dot(v, v));
}

Vec3 vec3_normalize(Vec3 v) {
    double len = vec3_length(v);
    if (len < EPSILON) {
        return vec3_create(0.0, 0.0, 0.0);
    }
    return vec3_scale(v, 1.0 / len);
}

NVector *nvector_create(size_t dim) {
    NVector *vec = (NVector *)malloc(sizeof(NVector));
    if (vec == NULL) {
        return NULL;
    }
    vec->dim = dim;
    vec->data = (double *)calloc(dim, sizeof(double));
    if (vec->data == NULL) {
        free(vec);
        return NULL;
    }
    return vec;
}

void nvector_destroy(NVector *vec) {
    if (vec != NULL) {
        free(vec->data);
        free(vec);
    }
}

double nvector_dot(const NVector *a, const NVector *b) {
    if (a == NULL || b == NULL || a->dim != b->dim) {
        return 0.0;
    }
    double sum = 0.0;
    for (size_t i = 0; i < a->dim; i++) {
        sum += a->data[i] * b->data[i];
    }
    return sum;
}

/* ========================================================================= */
/* 2. Estructura de Matriz y Operaciones Básicas                            */
/* ========================================================================= */

typedef struct {
    size_t rows;
    size_t cols;
    double **data;
} Matrix;

Matrix *matrix_create(size_t rows, size_t cols) {
    if (rows == 0 || cols == 0) {
        return NULL;
    }
    Matrix *mat = (Matrix *)malloc(sizeof(Matrix));
    if (mat == NULL) {
        return NULL;
    }
    mat->rows = rows;
    mat->cols = cols;
    mat->data = (double **)malloc(rows * sizeof(double *));
    if (mat->data == NULL) {
        free(mat);
        return NULL;
    }
    for (size_t i = 0; i < rows; i++) {
        mat->data[i] = (double *)calloc(cols, sizeof(double));
        if (mat->data[i] == NULL) {
            for (size_t j = 0; j < i; j++) {
                free(mat->data[j]);
            }
            free(mat->data);
            free(mat);
            return NULL;
        }
    }
    return mat;
}

void matrix_destroy(Matrix *mat) {
    if (mat == NULL) {
        return;
    }
    for (size_t i = 0; i < mat->rows; i++) {
        free(mat->data[i]);
    }
    free(mat->data);
    free(mat);
}

Matrix *matrix_identity(size_t n) {
    Matrix *mat = matrix_create(n, n);
    if (mat == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < n; i++) {
        mat->data[i][i] = 1.0;
    }
    return mat;
}

Matrix *matrix_copy(const Matrix *src) {
    if (src == NULL) {
        return NULL;
    }
    Matrix *dst = matrix_create(src->rows, src->cols);
    if (dst == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < src->rows; i++) {
        for (size_t j = 0; j < src->cols; j++) {
            dst->data[i][j] = src->data[i][j];
        }
    }
    return dst;
}

void matrix_print(const Matrix *mat, const char *label) {
    if (label != NULL) {
        printf("--- %s (%zux%zu) ---\n", label, mat->rows, mat->cols);
    }
    if (mat == NULL) {
        printf("(null)\n");
        return;
    }
    for (size_t i = 0; i < mat->rows; i++) {
        printf("[ ");
        for (size_t j = 0; j < mat->cols; j++) {
            printf("%8.3f ", mat->data[i][j]);
        }
        printf("]\n");
    }
}

/* ========================================================================= */
/* 3. Aritmética de Matrices                                                */
/* ========================================================================= */

Matrix *matrix_add(const Matrix *a, const Matrix *b) {
    if (a == NULL || b == NULL || a->rows != b->rows || a->cols != b->cols) {
        return NULL;
    }
    Matrix *res = matrix_create(a->rows, a->cols);
    if (res == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < a->rows; i++) {
        for (size_t j = 0; j < a->cols; j++) {
            res->data[i][j] = a->data[i][j] + b->data[i][j];
        }
    }
    return res;
}

Matrix *matrix_sub(const Matrix *a, const Matrix *b) {
    if (a == NULL || b == NULL || a->rows != b->rows || a->cols != b->cols) {
        return NULL;
    }
    Matrix *res = matrix_create(a->rows, a->cols);
    if (res == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < a->rows; i++) {
        for (size_t j = 0; j < a->cols; j++) {
            res->data[i][j] = a->data[i][j] - b->data[i][j];
        }
    }
    return res;
}

Matrix *matrix_scalar_mul(const Matrix *a, double scalar) {
    if (a == NULL) {
        return NULL;
    }
    Matrix *res = matrix_create(a->rows, a->cols);
    if (res == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < a->rows; i++) {
        for (size_t j = 0; j < a->cols; j++) {
            res->data[i][j] = a->data[i][j] * scalar;
        }
    }
    return res;
}

Matrix *matrix_mul(const Matrix *a, const Matrix *b) {
    if (a == NULL || b == NULL || a->cols != b->rows) {
        return NULL;
    }
    Matrix *res = matrix_create(a->rows, b->cols);
    if (res == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < a->rows; i++) {
        for (size_t j = 0; j < b->cols; j++) {
            double sum = 0.0;
            for (size_t k = 0; k < a->cols; k++) {
                sum += a->data[i][k] * b->data[k][j];
            }
            res->data[i][j] = sum;
        }
    }
    return res;
}

Matrix *matrix_transpose(const Matrix *mat) {
    if (mat == NULL) {
        return NULL;
    }
    Matrix *res = matrix_create(mat->cols, mat->rows);
    if (res == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < mat->rows; i++) {
        for (size_t j = 0; j < mat->cols; j++) {
            res->data[j][i] = mat->data[i][j];
        }
    }
    return res;
}

/* ========================================================================= */
/* 4. Eliminación Gaussiana y Solución de Sistemas Ax = b                    */
/* ========================================================================= */

static void matrix_swap_rows(Matrix *mat, size_t r1, size_t r2) {
    if (r1 == r2 || mat == NULL) {
        return;
    }
    double *temp = mat->data[r1];
    mat->data[r1] = mat->data[r2];
    mat->data[r2] = temp;
}

bool matrix_gaussian_elimination(Matrix *aug) {
    if (aug == NULL || aug->cols <= aug->rows) {
        return false;
    }
    size_t n = aug->rows;

    for (size_t i = 0; i < n; i++) {
        size_t max_row = i;
        double max_val = fabs(aug->data[i][i]);

        for (size_t k = i + 1; k < n; k++) {
            double current_val = fabs(aug->data[k][i]);
            if (current_val > max_val) {
                max_val = current_val;
                max_row = k;
            }
        }

        if (max_val < EPSILON) {
            return false; /* Matriz singular o casi singular */
        }

        matrix_swap_rows(aug, i, max_row);

        for (size_t k = i + 1; k < n; k++) {
            double factor = aug->data[k][i] / aug->data[i][i];
            for (size_t j = i; j < aug->cols; j++) {
                aug->data[k][j] -= factor * aug->data[i][j];
            }
        }
    }
    return true;
}

double *matrix_solve_system(const Matrix *a, const double *b) {
    if (a == NULL || b == NULL || a->rows != a->cols) {
        return NULL;
    }
    size_t n = a->rows;
    Matrix *aug = matrix_create(n, n + 1);
    if (aug == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            aug->data[i][j] = a->data[i][j];
        }
        aug->data[i][n] = b[i];
    }

    if (!matrix_gaussian_elimination(aug)) {
        matrix_destroy(aug);
        return NULL;
    }

    double *x = (double *)calloc(n, sizeof(double));
    if (x == NULL) {
        matrix_destroy(aug);
        return NULL;
    }

    for (int i = (int)n - 1; i >= 0; i--) {
        double sum = aug->data[i][n];
        for (size_t j = (size_t)i + 1; j < n; j++) {
            sum -= aug->data[i][j] * x[j];
        }
        x[i] = sum / aug->data[i][(size_t)i];
    }

    matrix_destroy(aug);
    return x;
}

/* ========================================================================= */
/* 5. Descomposición LU y Determinante                                      */
/* ========================================================================= */

bool matrix_lu_decompose(const Matrix *mat, Matrix **out_l, Matrix **out_u) {
    if (mat == NULL || mat->rows != mat->cols || out_l == NULL || out_u == NULL) {
        return false;
    }
    size_t n = mat->rows;
    Matrix *l = matrix_identity(n);
    Matrix *u = matrix_create(n, n);
    if (l == NULL || u == NULL) {
        matrix_destroy(l);
        matrix_destroy(u);
        return false;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t k = i; k < n; k++) {
            double sum = 0.0;
            for (size_t j = 0; j < i; j++) {
                sum += (l->data[i][j] * u->data[j][k]);
            }
            u->data[i][k] = mat->data[i][k] - sum;
        }

        for (size_t k = i + 1; k < n; k++) {
            double sum = 0.0;
            for (size_t j = 0; j < i; j++) {
                sum += (l->data[k][j] * u->data[j][i]);
            }
            if (fabs(u->data[i][i]) < EPSILON) {
                matrix_destroy(l);
                matrix_destroy(u);
                return false;
            }
            l->data[k][i] = (mat->data[k][i] - sum) / u->data[i][i];
        }
    }

    *out_l = l;
    *out_u = u;
    return true;
}

double matrix_determinant(const Matrix *mat) {
    if (mat == NULL || mat->rows != mat->cols) {
        return 0.0;
    }
    size_t n = mat->rows;
    if (n == 1) {
        return mat->data[0][0];
    }
    if (n == 2) {
        return (mat->data[0][0] * mat->data[1][1]) - (mat->data[0][1] * mat->data[1][0]);
    }

    Matrix *l = NULL;
    Matrix *u = NULL;
    if (!matrix_lu_decompose(mat, &l, &u)) {
        return 0.0;
    }

    double det = 1.0;
    for (size_t i = 0; i < n; i++) {
        det *= u->data[i][i];
    }

    matrix_destroy(l);
    matrix_destroy(u);
    return det;
}

/* ========================================================================= */
/* 6. Inversa de una Matriz                                                 */
/* ========================================================================= */

Matrix *matrix_inverse(const Matrix *mat) {
    if (mat == NULL || mat->rows != mat->cols) {
        return NULL;
    }
    size_t n = mat->rows;
    Matrix *aug = matrix_create(n, 2 * n);
    if (aug == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            aug->data[i][j] = mat->data[i][j];
        }
        aug->data[i][n + i] = 1.0;
    }

    /* Eliminación Gauss-Jordan */
    for (size_t i = 0; i < n; i++) {
        size_t pivot = i;
        double max_val = fabs(aug->data[i][i]);
        for (size_t k = i + 1; k < n; k++) {
            double v = fabs(aug->data[k][i]);
            if (v > max_val) {
                max_val = v;
                pivot = k;
            }
        }
        if (max_val < EPSILON) {
            matrix_destroy(aug);
            return NULL;
        }

        matrix_swap_rows(aug, i, pivot);

        double div = aug->data[i][i];
        for (size_t j = 0; j < 2 * n; j++) {
            aug->data[i][j] /= div;
        }

        for (size_t k = 0; k < n; k++) {
            if (k != i) {
                double factor = aug->data[k][i];
                for (size_t j = 0; j < 2 * n; j++) {
                    aug->data[k][j] -= factor * aug->data[i][j];
                }
            }
        }
    }

    Matrix *inv = matrix_create(n, n);
    if (inv == NULL) {
        matrix_destroy(aug);
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            inv->data[i][j] = aug->data[i][n + j];
        }
    }

    matrix_destroy(aug);
    return inv;
}

/* ========================================================================= */
/* 7. Programa Principal de Demostración y Pruebas                          */
/* ========================================================================= */

int main(void) {
    printf("=== Probando Vectores 3D ===\n");
    Vec3 u = vec3_create(1.0, 2.0, 3.0);
    Vec3 v = vec3_create(4.0, 5.0, 6.0);
    Vec3 cross = vec3_cross(u, v);
    printf("u x v = [%.2f, %.2f, %.2f]\n", cross.x, cross.y, cross.z);
    printf("u . v = %.2f\n", vec3_dot(u, v));

    printf("\n=== Probando Multiplicacion de Matrices ===\n");
    Matrix *a = matrix_create(2, 3);
    Matrix *b = matrix_create(3, 2);

    a->data[0][0] = 1.0; a->data[0][1] = 2.0; a->data[0][2] = 3.0;
    a->data[1][0] = 4.0; a->data[1][1] = 5.0; a->data[1][2] = 6.0;

    b->data[0][0] = 7.0;  b->data[0][1] = 8.0;
    b->data[1][0] = 9.0;  b->data[1][1] = 1.0;
    b->data[2][0] = 2.0;  b->data[2][1] = 3.0;

    Matrix *c = matrix_mul(a, b);
    matrix_print(a, "Matriz A");
    matrix_print(b, "Matriz B");
    matrix_print(c, "Producto C = A * B");

    printf("\n=== Probando Determinante e Inversa ===\n");
    Matrix *sq = matrix_create(3, 3);
    sq->data[0][0] = 2.0; sq->data[0][1] = -1.0; sq->data[0][2] = 0.0;
    sq->data[1][0] = -1.0; sq->data[1][1] = 2.0; sq->data[1][2] = -1.0;
    sq->data[2][0] = 0.0; sq->data[2][1] = -1.0; sq->data[2][2] = 2.0;

    double det = matrix_determinant(sq);
    printf("Determinante de sq = %.4f\n", det);

    Matrix *inv = matrix_inverse(sq);
    matrix_print(inv, "Inversa de sq");

    Matrix *ident = matrix_mul(sq, inv);
    matrix_print(ident, "sq * inv (debe ser Identidad)");

    printf("\n=== Probando Solucion de Sistema Ax = b ===\n");
    double rhs[] = {1.0, 0.0, 1.0};
    double *solution = matrix_solve_system(sq, rhs);
    if (solution != NULL) {
        printf("Solucion x:\n");
        for (size_t i = 0; i < 3; i++) {
            printf("x[%zu] = %.4f\n", i, solution[i]);
        }
        free(solution);
    }

    matrix_destroy(a);
    matrix_destroy(b);
    matrix_destroy(c);
    matrix_destroy(sq);
    matrix_destroy(inv);
    matrix_destroy(ident);

    printf("\n=== Todas las pruebas matriciales completadas con exito ===\n");
    return 0;
}
