#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

extern int eliminate(int m, int n, double A[][n], bool do_partial_pivoting, bool augmented_matrix, int precision);
extern int substitute(int m, int n, double A[][n], double* x, int precision);
extern int gauss_elim(int m, int n, double A[][n], double* x, bool do_partial_pivoting, bool augmented_matrix, int precision);
extern double round_to_digits(double value, int digits);

// https://stackoverflow.com/a/13409133
float randf(float min, float max)
{
    float x = (float)rand()/(float)(RAND_MAX/max) + min;
    return x;
}

int main()
{
    // generate random agumented matrix A of size n
    srand(time(NULL));

    int n = 2;
    double M[n][n+1];
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j <= n; j++)
        {
            M[i][j] = randf(-10, 10);
        }
    }

    for (int d = 3; d <= 6; d++)
    {
        double A[n][n+1];
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j <= n; j++)
            {
                A[i][j] = round_to_digits(M[i][j], d);
            }
        }
        // copy for without pivoting
        double AA[n][n+1];
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j <= n; j++)
            {
                AA[i][j] = A[i][j];
            }
        }

        // run Gaussian elimination and substitution on A and measure time taken for each
        double x[n];
        int ret = gauss_elim(n, n + 1, A, x, true, true, d);

        if (ret == 0)
        {
            printf("\nOutput with pivoting\n");
            for (int i = 0; i < sizeof(x)/sizeof(double); i++)
            {
                printf("x[%d] = %8.6f\n", i, x[i]);
            }
        }

        double xx[n];
        ret = gauss_elim(n, n + 1, AA, xx, false, true, d);
        if (ret == 0)
        {
            printf("\nOutput without pivoting\n");
            for (int i = 0; i < sizeof(xx)/sizeof(double); i++)
            {
                printf("xx[%d] = %8.6f\n", i, xx[i]);
            }
        }
    }

    return 0;
}
