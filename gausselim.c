// vim: noai:ts=4:sw=4

/*
recipe:
> `gcc --std=c99 gausselim.c -o gausselim`
> define TEST macro for running tests: `gcc --std=c99 -DTEST gausselim.c -o gausselim`
> define PRINTDEBUG macro for printing intermediate results
> define NOMAIN macro for compiling without main()
*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#ifdef TEST
#include <assert.h>
#endif

int gauss_elim(int m, int n, double A[][n], double* x, bool do_partial_pivoting, bool augmented_matrix, int precision);
int eliminate(int m, int n, double A[][n], bool do_partial_pivoting, bool augmented_matrix, int precision);
int substitute(int m, int n, double A[][n], double* x, int precision);
double round_to_digits(double value, int digits);

static int find_row_index_with_max_pivot(int m, int n, double A[][n], int i, int k);
static void row_swap(int n, double A[][n], int i, int j);
static void swap_element(int n, double A[][n], int i, int j, int c);
static bool row_is_all_zeros(int n, double A[][n], int row, int last_index);

static void debug_matrix_with_pivot_info(int m, int n, double A[][n], int pivot_index, const char* heading);
static void debug_matrix(int m, int n, double A[][n]);
static void print_matrix(int m, int n, double A[][n]);
static void debug_message(const char* message, ...);
static void print_error(const char* message, ...);

#define EPSILON 0.0001
#ifdef TEST
void test_1()
{
    double matrix_A[4][5] = {
        { 0, 2, 0, 1, 0 },
        { 2, 2, 3, 2, -2 },
        { 4, -3, 0, 1, -7 },
        { 6, 1, -6, -5, 6 }
    };

    double x[4];
    int ret = gauss_elim(4, 5, matrix_A, x, true, true, 10);

    assert(ret == 0);
    assert(fabs(x[0] - -0.5) <= EPSILON);
    assert(fabs(x[1] - 1.0) <= EPSILON);
    assert(fabs(x[2] - 0.3333) <= EPSILON);
    assert(fabs(x[3] - -2.0) <= EPSILON);

    /* without partial pivoting */
    double matrix_AA[4][5] = {
        { 0, 2, 0, 1, 0 },
        { 2, 2, 3, 2, -2 },
        { 4, -3, 0, 1, -7 },
        { 6, 1, -6, -5, 6 }
    };

    ret = gauss_elim(4, 5, matrix_AA, x, false, true, 10);

    assert(ret == 0);
    assert(fabs(x[0] - -0.5) <= EPSILON);
    assert(isnan(x[1]));
    assert(isnan(x[2]));
    assert(isnan(x[3]));
}

void test_2()
{
    double matrix_A[5][6] = {
        { 13, 4, 5, 0, 9, 101 },
        { 33, -2, -7, 8, 0, 3 },
        { 23, 32, 9, 5, 1, 7 },
        { 54, 34, 87, 2, 4, 66 },
        { -5, 6, 7, 8, 9, 10 }
    };

    double x[5];
    int ret = gauss_elim(5, 6, matrix_A, x, true, true, 10);

    assert(ret == 0);
    assert(fabs(x[0] - 1.7424) <= EPSILON);
    assert(fabs(x[1] - -0.0149) <= EPSILON);
    assert(fabs(x[2] - -0.5640) <= EPSILON);
    assert(fabs(x[3] - -7.3098) <= EPSILON);
    assert(fabs(x[4] - 9.0253) <= EPSILON);

    /* without partial pivoting */
    double matrix_AA[5][6] = {
        { 13, 4, 5, 0, 9, 101 },
        { 33, -2, -7, 8, 0, 3 },
        { 23, 32, 9, 5, 1, 7 },
        { 54, 34, 87, 2, 4, 66 },
        { -5, 6, 7, 8, 9, 10 }
    };
    ret = gauss_elim(5, 6, matrix_AA, x, false, true, 10);

    assert(ret == 0);
    assert(fabs(x[0] - 1.7424) <= EPSILON);
    assert(fabs(x[1] - -0.0149) <= EPSILON);
    assert(fabs(x[2] - -0.5640) <= EPSILON);
    assert(fabs(x[3] - -7.3098) <= EPSILON);
    assert(fabs(x[4] - 9.0253) <= EPSILON);

}

void test_3()
{
    double matrix_A[][4] = {
        { 1, 0, -5, 0 },    // father 5 times older than child
        { 1, 1, 0, 99 },    // sum of parents' age is 99
        { 0, 1, -3.5, 0 }   // mother 3.5 times older than child
    };

    double x[3];
    int ret = gauss_elim(3, 4, matrix_A, x, true, true, 10);

    assert(ret == 0);
    assert(fabs(x[0] - 58.2353) <= EPSILON);
    assert(fabs(x[1] - 40.7647) <= EPSILON);
    assert(fabs(x[2] - 11.6471) <= EPSILON);

    /* without partial pivoting */
    double matrix_AA[][4] = {
        { 1, 0, -5, 0 },    // father 5 times older than child
        { 1, 1, 0, 99 },    // sum of parents' age is 99
        { 0, 1, -3.5, 0 }   // mother 3.5 times older than child
    };
    ret = gauss_elim(3, 4, matrix_AA, x, false, true, 10);

    assert(ret == 0);
    assert(fabs(x[0] - 58.2353) <= EPSILON);
    assert(fabs(x[1] - 40.7647) <= EPSILON);
    assert(fabs(x[2] - 11.6471) <= EPSILON);

}
#endif

#ifndef NOMAIN
int main(int argc, char** argv)
{
    printf("******* Gaussian Elimination Program ******\n");

#ifdef TEST
    printf("Running tests\n");
    test_1();
    test_2();
    test_3();
    printf("Finished running tests\n");

    return 0;
#else
    /** input: parse command line arguments **/
    if (argc < 2)
    {
        printf("ERROR: Provide a matrix as input\n");
        return -1;
    }
    bool do_partial_pivot = true;
    bool augmented_matrix = true;
    int arg_i = 1;
    while (strncmp(argv[arg_i], "--", 2) == 0)
    {
        if (strcmp(argv[arg_i], "--no-pivot") == 0)
        {
            do_partial_pivot = false;
        }
        else if (strcmp(argv[arg_i], "--no-aug") == 0)
        {
            augmented_matrix = false;
        }
        else if (strlen(argv[arg_i]) != 2)
        {
            printf("WARNING: unrecognized option %s\n", argv[arg_i]);
        }
        arg_i++;
    }

    int precision = atoi(argv[arg_i]);
    int n = atoi(argv[arg_i + 1]);
    int elements_start = arg_i + 2;
    int m = (argc - elements_start) / n;
    if (n < 1 || m < 1)
    {
        printf("ERROR: The input matrix must have at least one row and column\n");
        return -1;
    }
    printf("Number of columns = %d, Number of rows = %d\n", n, m);
    double A[m][n];
    for (int i = elements_start; i < argc; i++)
    {
        int col = (i - elements_start) % n;
        int row = (i - elements_start) / n;
        A[row][col] = atof(argv[i]);
    }

    /** run **/
    double x[n - 1];
    int ret = gauss_elim(m, n, A, x, do_partial_pivot, augmented_matrix, precision);
    if (ret == 0)
    {
        printf("\nOutput\n");
        if (augmented_matrix)
        {
            for (int i = 0; i < sizeof(x)/sizeof(double); i++)
            {
                printf("x[%d] = %8.10f\n", i, x[i]);
            }
        }
        else
        {
            print_matrix(m, n, A);
        }
    }

    return ret;
#endif
}
#endif	// NOMAIN

/**
 * Given a matrix A of size mxn, performs Gaussian Elimination
 * by doing forward elimination and backward substituion, and puts the
 * result in the given array x.
 * Partial pivoting is performed to avoid pitfalls, if opted.
 */
int gauss_elim(int m, int n, double A[][n], double* x, bool do_partial_pivoting, bool augmented_matrix, int precision)
{
    /** Dump for debugging **/
    debug_matrix(m, n, A);

    /** forward elimination **/
    int ret = eliminate(m, n, A, do_partial_pivoting, augmented_matrix, precision);

    /** back substitution **/
    if (augmented_matrix) ret |= substitute(m, n, A, x, precision);

    return ret;
}

int eliminate(int m, int n, double A[][n], bool do_partial_pivoting, bool augmented_matrix, int precision)
{
    int numCoeffCols = augmented_matrix ? n - 1 : n;
    int numPivots = (m > numCoeffCols) ? numCoeffCols : (m - 1);
    for (int k = 0; k < numPivots; k++)
    {
        // 1. check if partial pivoting is needed
        double pivot = A[k][k];
        // a. divide by zero
        if (do_partial_pivoting == true && pivot == 0)
        {
            // interchange A[i] with A[r] where A[r][k] is max
            int r = find_row_index_with_max_pivot(m, n, A, k, k);
            row_swap(n, A, k, r);
            pivot = A[k][k];    // update the pivot because we have swapped

            debug_matrix_with_pivot_info(m, n, A, k, "div0");
        }

        for (int i = k + 1; i < m; i++) // for all rows below the pivot row Rp
        {
            // b. round-off error
            if (do_partial_pivoting == true && fabs(pivot) < fabs(A[i][k]))
            {
                row_swap(n, A, k, i);
                pivot = A[k][k];    // update the pivot because we have swapped

                debug_matrix_with_pivot_info(m, n, A, k, "round-off");
            }

            if (A[i][k] == 0) continue;

            // 2. eliminate
            double multiplier = round_to_digits(A[i][k] / pivot, precision);    // division operation

            A[i][k] = 0;    // set A[i][k] to zero directly
            for (int j = k + 1; j < n; j++) // rest of the columns
            {
                A[i][j] -= round_to_digits(A[k][j] * multiplier, precision);    // subtraction and multiplication operations
                A[i][j] = round_to_digits(A[i][j], precision);
            }
        }

        debug_matrix_with_pivot_info(m, n, A, k, "elim");
    }

    return 0;
}

int substitute(int m, int n, double A[][n], double* x, int precision)
{
    bool x_solved[n];
    memset(x_solved, false, n);

    for (int i = m - 1; i >= 0; i--)
    {
        double b = A[i][n - 1];
        if (row_is_all_zeros(n, A, i, n - 1))
        {
            if ((int)b == 0)
            {
                debug_message("Skipping row %d as it's all zeros\n", i);
            }
            else
            {
                print_error("ERROR: non-zero b %8.10f for zero row %d.. inconsistent system.\n", b, i);
                return -2;
            }
        }

        char unsolved_index = -1;
        // verify that the row is solved
        for (int j = 0; j < n - 1; j++)
        {
            if (A[i][j] != 0)
            {
                if (x_solved[j] == false)
                {
                    if (unsolved_index >= 0)
                    {
                        debug_message("Free variable %d found.. infinite solutions exist. Picking value 1\n", j);
                        b -= A[i][j];
                        x[j] = 1;
                        x_solved[j] = true;
                        continue;
                    }
                    unsolved_index = j;
                }
                else
                {
                    b -= round_to_digits(A[i][j] * x[j], precision);
                    b = round_to_digits(b, precision);
                }
            }
        }
        if (unsolved_index >= 0)
        {
            x[unsolved_index] = round_to_digits(b / A[i][unsolved_index], precision);
            x_solved[unsolved_index] = true;
        }
    }

    return 0;
}

// https://stackoverflow.com/a/13094362
double round_to_digits(double value, int digits)
{
    if (value == 0.0) // otherwise it will return 'nan' due to the log10() of zero
        return 0.0;

    double factor = pow(10.0, digits - ceil(log10(fabs(value))));
    return round(value * factor) / factor;
}

/**
 * Finds the max element in the given column k from rows i to m, and returns the row index
 * having the max element.
 */
static int find_row_index_with_max_pivot(int m, int n, double A[][n], int i, int k)
{
    int maxIndex = i;
    double max = A[i][k];

    for (i = i + 1; i < m; i++)
    {
        if (A[i][k] > max)
        {
            max = A[i][k];
            maxIndex = i;
        }
    }

    return maxIndex;
}

/**
 * Swaps rows i and j
 */
static void row_swap(int n, double A[][n], int i, int j)
{
    for (int col = 0; col < n; col++)
    {
        swap_element(n, A, i, j, col);
    }
}

static void swap_element(int n, double A[][n], int i, int j, int c)
{
    A[i][c] += A[j][c];
    A[j][c] = A[i][c] - A[j][c];
    A[i][c] = A[i][c] - A[j][c];
}

static bool row_is_all_zeros(int n, double A[][n], int row, int last_index)
{
    bool zeros = true;
    for (int j = 0; j < last_index; j++)
    {
        if (A[row][j] != 0)
        {
            zeros = false;
            break;
        }
    }
    return zeros;
}

static void debug_matrix_with_pivot_info(int m, int n, double A[][n], int pivot_index, const char* heading)
{
#ifdef PRINTDEBUG
    printf("k = %d\n", pivot_index);
    printf("-------%s---------\n", heading);
    print_matrix(m, n, A);
#endif
}

static void debug_matrix(int m, int n, double A[][n])
{
#ifdef PRINTDEBUG
    print_matrix(m, n, A);
#endif
}

void print_matrix(int m, int n, double A[][n])
{
    for (int i = 0; i < m; i++)
    {
        printf("[%d]: ", i);
        for (int j = 0; j < n; j++)
        {
            printf("%8.10f ", A[i][j]);
        }
        printf("\n");
    }
}

static void debug_message(const char* message, ...)
{
#ifdef PRINTDEBUG
    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);
#endif
}

static void print_error(const char* message, ...)
{
    va_list args;
    va_start(args, message);
    vfprintf(stderr, message, args);
    va_end(args);
}
