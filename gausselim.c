// vim: noai:ts=4:sw=4

/*
recipe:
> `gcc --std=c99 gausselim.c -o gausselim`
> define TEST macro for running tests: `gcc --std=c99 -DTEST gausselim.c -o gausselim`
> define PRINTDEBUG macro for printing intermediate results
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#ifdef TEST
#include <assert.h>
#include <math.h>
#endif

int gauss_elim(int m, int n, double A[][n], double* x, bool do_partial_pivoting);

static int find_row_index_with_max_pivot(int m, int n, double A[][n], int i, int p);
static void row_swap(int n, double A[][n], int i, int j);
static void swap_element(int n, double A[][n], int i, int j, int c);

static void debug_matrix(int m, int n, double A[][n], int pivotIndex, const char* heading);
static void print_matrix(int m, int n, double A[][n]);

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
	int ret = gauss_elim(4, 5, matrix_A, x, true);

	assert(ret == 0);
	assert(fabs(x[0] - -0.5) <= EPSILON);
	assert(fabs(x[1] - 1.0) <= EPSILON);
	assert(fabs(x[2] - 0.3333) <= EPSILON);
	assert(fabs(x[3] - -2.0) <= EPSILON);
}
#endif

int main(int argc, char** argv)
{
	printf("******* Gaussian Elimination Program ******\n");

#ifdef TEST
	printf("Running tests\n");
	test_1();
	printf("Finished running tests\n");
#else
	/** input: parse command line arguments **/
	if (argc < 2)
	{
		printf("ERROR: Provide an augmented matrix as input\n");
		return -1;
	}
	bool do_partial_pivot = true;
	int arg_i = 1;
	while (strncmp(argv[arg_i], "--", 2) == 0)
	{
		if (strcmp(argv[1], "--nopivot") == 0)
		{
			do_partial_pivot = false;
		}
		arg_i++;
	}

	int n = atoi(argv[arg_i]);
	int elements_start = arg_i + 1;
	int m = (argc - elements_start) / n;
	if (n < 1 || m < 1)
	{
		printf("ERROR: The input matrix must have at least one row and column\n");
		return -1;
	}
	if (m < n - 1)
	{
		printf("ERROR: The input augmented matrix must have at least as many columns"
				" as the co-efficient matrix\n");
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
	print_matrix(m, n, A);

	/** run **/
	double x[n - 1];
	int ret = gauss_elim(m, n, A, x, do_partial_pivot);
	if (ret == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			printf("x[%d] = %8.4f\n", i, x[i]);
		}
	}
#endif

	return 0;
}

/**
 * Given augmented matrix A of size mxn, performs Gaussian Elimination
 * by doing forward elimination and backward substituion, and puts the
 * result in the given array x.
 * Partial pivoting is performed to avoid pitfalls, if opted.
 */
int gauss_elim(int m, int n, double A[][n], double* x, bool do_partial_pivoting)
{
	/** Dump for debugging **/
	print_matrix(m, n, A);

	bool x_solved[n];
	memset(x_solved, false, n);

	/** forward elimination **/
	for (int p = 0; p < m - 1; p++)
	{
		// 1. check if partial pivoting is needed
		double pivot = A[p][p];
		// a. divide by zero
		if (do_partial_pivoting == true && pivot == 0)
		{
			// interchange A[i] with A[r] where A[r][p] is max
			int r = find_row_index_with_max_pivot(m, n, A, p, p);
			row_swap(n, A, p, r);
			pivot = A[p][p];	// update the pivot because we have swapped

			debug_matrix(m, n, A, p, "div0");
		}

		for (int k = p + 1; k < m; k++)	// for all rows below the pivot row Rp
		{
			// b. round-off error
			if (do_partial_pivoting == true && abs(pivot) < abs(A[k][p]))
			{
				row_swap(n, A, p, k);	
				pivot = A[p][p];	// update the pivot because we have swapped

				debug_matrix(m, n, A, p, "round-off");
			}

			if (A[k][p] == 0) continue;

			// 2. eliminate
			double multiplier = A[k][p] / pivot;			// division operation

			A[k][p] = 0;	// set A[i][p] to zero directly
			for (int j = p + 1; j < n; j++)	// rest of the columns
			{
				A[k][j] -= A[p][j] * multiplier;	// subtraction and multiplication operations
			}
		}

		debug_matrix(m, n, A, p, "elim");
	}

	/** back substitution **/
	for (int k = m - 1; k >= 0; k--)
	{
		char unsolved_index = -1;
		// verify that the row is solved
		double b = A[k][n - 1];
		for (int j = 0; j < n - 1; j++)
		{
			if (A[k][j] != 0)
			{
				if (x_solved[j] == false)
				{
					if (unsolved_index >= 0) return -1;	// ERROR: solution doesn't exist, or not enough equations given!
					unsolved_index = j;
				}
				else
				{
					b -= A[k][j] * x[j];
				}
			}
		}
		if (unsolved_index >= 0)
		{
			x[unsolved_index] = b / A[k][unsolved_index];
			x_solved[unsolved_index] = true;
		}
	}

	return 0;
}

/**
 * Finds the max element in the given column p from rows i to m, and returns the row index
 * having the max element.
 */
static int find_row_index_with_max_pivot(int m, int n, double A[][n], int i, int p)
{
	int maxIndex = i;
	double max = A[i][p];

	for (i = i + 1; i < m; i++)
	{
		if (A[i][p] > max)
		{
			max = A[i][p];
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

static void debug_matrix(int m, int n, double A[][n], int pivotIndex, const char* heading)
{
#ifdef PRINTDEBUG
	printf("p = %d\n", pivotIndex);
	printf("-------%s---------\n", heading);
	print_matrix(m, n, A);
#endif
}

void print_matrix(int m, int n, double A[][n])
{
#ifdef PRINTDEBUG
	for (int i = 0; i < m; i++)
	{
		printf("[%d]: ", i);
		for (int j = 0; j < n; j++)
		{
			printf("%8.4f ", A[i][j]);
		}
		printf("\n");
	}
#endif
}
