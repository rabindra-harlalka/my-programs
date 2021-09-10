#include <stdio.h>
#include <math.h>
//#include <string.h>
#ifdef PRINTDEBUG
#include <stdarg.h>
#endif

void decompose(int n, double A[][n], double L[][n], double U[][n], int precision);
void forward_substitution(int n, double L[][n], double y[n], double b[n]);
void backward_substitution(int n, double U[][n], double x[n], double y[n]);

void matmul(int n, double A[][n], double B[][n], double C[][n]);
void matmul_general(int m, int n, int p, double A[][n], double B[][p], double C[][p]);

#ifndef HIDEDUP
double round_to_digits(double value, int digits);

void print_matrix(int m, int n, double A[][n]);
#else
extern double round_to_digits(double value, int digits);

extern void print_matrix(int m, int n, double A[][n]);
#endif
static void debug_message(const char* message, ...);

void matmul_general(int m, int n, int p, double A[][n], double B[][p], double C[][p])
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < p; j++)
        {
            for (int k = 0; k < n; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void forward_substitution(int n, double L[][n], double y[n], double b[n])
{
    // Solve Ly=b for y
    // y[i] = ( b[i] - Σ[k=1 to i-1] (L[i][k] * y[k]) ) / L[i][i]
    for (int i = 0; i < n; i++)
    {
        debug_message("y%d = (b%d", i+1, i+1);
        double sum = 0;
        for (int k = 0; k < i; k++)
        {
            debug_message(" - L%d%d * y%d", i+1, k+1, k+1);
            sum += L[i][k] * y[k];
        }
        debug_message(") / L%d%d", i+1, i+1);
        y[i] = (b[i] - sum) / L[i][i];
        debug_message("\n");
    }
}

void backward_substitution(int n, double U[][n], double x[n], double y[n])
{
    // Solve Ux=y for x
    // x[i] = y[i] - Σ[k=i+1 to n] (U[i][k] * x[k])
    for (int i = n - 1; i >= 0; i--)
    {
        debug_message("x%d = y%d", i+1, i+1);
        double sum = 0;
        for (int k = i + 1; k < n; k++)
        {
            debug_message(" - U%d%d * x%d", i+1, k+1, k+1);
            sum += U[i][k] * x[k];
        }
        x[i] = y[i] - sum;
        debug_message("\n");
    }
}

void decompose(int n, double A[][n], double L[][n], double U[][n], int precision)
{
    // L lower triangular matrix
    // U unit upper triangular matrix

    // Fill diagonal of U with 1
    for (int i = 0; i < n; i++)
    {
        U[i][i] = 1;
    }

    for (int i = 0; i < n; i++)
    {
        // Calculate L
        for (int j = 0; j <= i; j++)
        {
            debug_message("L%d%d = A%d%d", i+1, j+1, i+1, j+1);
            double sum = 0;
            for (int k = 0; k < j; k++)
            {
                debug_message(" - L%d%d * U%d%d", i+1, k+1, k+1, j+1);
                if (precision == -1)
                {
                    sum += L[i][k] * U[k][j];
                }
                else
                {
                    sum += round_to_digits(L[i][k] * U[k][j], precision);
                    sum = round_to_digits(sum, precision);
                }
            }
            if (precision == -1)
            {
                L[i][j] = A[i][j] - sum;
            }
            else
            {
                L[i][j] = round_to_digits(A[i][j] - sum, precision);
            }
            debug_message("\n");
        }

        // Calculate U
        for (int j = i + 1; j < n; j++)
        {
            if (i == 0)
                debug_message("U%d%d = A%d%d", i+1, j+1, i+1, j+1);
            else
                debug_message("U%d%d = (A%d%d", i+1, j+1, i+1, j+1);
            double sum = 0;
            for (int k = 0; k < i; k++)
            {
                debug_message(" - L%d%d * U%d%d", i+1, k+1, k+1, j+1);
                if (precision == -1)
                {
                    sum += L[i][k] * U[k][j];
                }
                else
                {
                    sum += round_to_digits(L[i][k] * U[k][j], precision);
                    sum = round_to_digits(sum, precision);
                }
            }
            if (i == 0)
                debug_message(" / L%d%d", i+1, i+1);
            else
                debug_message(") / L%d%d", i+1, i+1);
            if (precision == -1)
            {
                U[i][j] = (A[i][j] - sum) / L[i][i];
            }
            else
            {
                U[i][j] = round_to_digits(
                    round_to_digits((A[i][j] - sum), precision) / L[i][i], precision);
            }

            debug_message("\n");
        }
    }
}

void matmul(int n, double A[][n], double B[][n], double C[][n])
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            for (int k = 0; k < n; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

#ifndef NOMAIN
int main()
{
    int n = 4;
    double A[][4] = {
    {0.4218,    0.6557,    0.6787,    0.6555},
    {0.9157,    0.0357,    0.7577,    0.1712},
    {0.7922,    0.8491,    0.7431,    0.7060},
    {0.9595,    0.9340,    0.3922,    0.0318},
    };

    double L[4][4] = {0};
    // memset(L, 0, sizeof(double) * 16);
    double U[4][4] = {0};
    // memset(U, 0, sizeof(double) * 16);

    printf("Matrix L\n");
    print_matrix(n, n, L);
    printf("Matrix U\n");
    print_matrix(n, n, U);

    decompose(4, A, L, U, -1/*5*/);

    printf("Matrix L\n");
    print_matrix(n, n, L);
    printf("Matrix U\n");
    print_matrix(n, n, U);

    double C[4][4] = {0};
    matmul(n, L, U, C);
    printf("Matrix A\n");
    print_matrix(n, n, A);
    printf("Matrix C\n");
    print_matrix(n, n, C);

    double x[4] = {0};
    double y[4] = {0};
    double b[4] = { 0.7094, 0.7547, 0.2760, 0.6797 };

    forward_substitution(n, L, y, b);
    backward_substitution(n, U, x, y);

    printf("Vector x\n");
    for (int j = 0; j < n; j++)
    {
        printf("%1.4f\n", x[j]);
    }

    double b_check[4][1] = {0};
    double x_mat[4][1];
    for (int i = 0; i < n; i++)
    {
        x_mat[i][0] = x[i];
    }
    matmul_general(4, 4, 1, A, x_mat, b_check);

    printf("Check vector b (Ax = b)\n");
    for (int j = 0; j < n; j++)
    {
        printf("%1.4f\n", b_check[j][0]);
    }
}
#endif	// NOMAIN

#ifndef HIDEDUP
// https://stackoverflow.com/a/13094362
double round_to_digits(double value, int digits)
{
    if (value == 0.0) // otherwise it will return 'nan' due to the log10() of zero
        return 0.0;

    double factor = pow(10.0, digits - ceil(log10(fabs(value))));
    return round(value * factor) / factor;
}

void print_matrix(int m, int n, double A[][n])
{
    for (int i = 0; i < m; i++)
    {
        printf("[%d]: ", i);
        for (int j = 0; j < n; j++)
        {
            printf("%1.4f ", A[i][j]);
        }
        printf("\n");
    }
}
#endif

static void debug_message(const char* message, ...)
{
#ifdef PRINTDEBUG
    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);
#endif
}