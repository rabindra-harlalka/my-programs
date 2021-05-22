// vim: noai:ts=4:sw=4

/*
recipe:
> `gcc --std=c99 gausselim.c -o gausselim`
> define TEST macro for running tests: `gcc --std=c99 -DTEST gausselim.c -o gausselim`
> define PRINTDEBUG macro for printing intermediate results
> define NOMAIN macro for compiling without main()
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
int eliminate(int m, int n, double A[][n], bool do_partial_pivoting);
int substitute(int m, int n, double A[][n], double* x);

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

	/* without partial pivoting */
	double matrix_AA[4][5] = {
		{ 0, 2, 0, 1, 0 },
		{ 2, 2, 3, 2, -2 },
		{ 4, -3, 0, 1, -7 },
		{ 6, 1, -6, -5, 6 }
	};

	ret = gauss_elim(4, 5, matrix_AA, x, false);

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
	int ret = gauss_elim(5, 6, matrix_A, x, true);

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
	ret = gauss_elim(5, 6, matrix_AA, x, false);

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
		{ 1, 0, -5, 0 },	// father 5 times older than child
		{ 1, 1, 0, 99 },	// sum of parents' age is 99
		{ 0, 1, -3.5, 0 }	// mother 3.5 times older than child
	};

	double x[3];
	int ret = gauss_elim(3, 4, matrix_A, x, true);

	assert(ret == 0);
	assert(fabs(x[0] - 58.2353) <= EPSILON);
	assert(fabs(x[1] - 40.7647) <= EPSILON);
	assert(fabs(x[2] - 11.6471) <= EPSILON);

	/* without partial pivoting */
	double matrix_AA[][4] = {
		{ 1, 0, -5, 0 },	// father 5 times older than child
		{ 1, 1, 0, 99 },	// sum of parents' age is 99
		{ 0, 1, -3.5, 0 }	// mother 3.5 times older than child
	};
	ret = gauss_elim(3, 4, matrix_AA, x, false);

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
		if (strcmp(argv[arg_i], "--no-pivot") == 0)
		{
			do_partial_pivot = false;
		}
		else
		{
			printf("WARNING: unrecognized option %s\n", argv[arg_i]);
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
		// elimination will work, but backward substiution won't
		printf("ERROR: The input augmented matrix must have at least as many rows as"
				" columns in the co-efficient matrix\n");
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
		for (int i = 0; i < sizeof(x)/sizeof(double); i++)
		{
			printf("x[%d] = %8.4f\n", i, x[i]);
		}
	}
#endif

	return 0;
}
#endif	// NOMAIN

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

	/** forward elimination **/
	int ret = eliminate(m, n, A, do_partial_pivoting);

	/** back substitution **/
	ret &= substitute(m, n, A, x);

	return ret;
}

int eliminate(int m, int n, double A[][n], bool do_partial_pivoting)
{
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

	return 0;
}

int substitute(int m, int n, double A[][n], double* x)
{
	bool x_solved[n];
	memset(x_solved, false, n);

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
					// ERROR: solution doesn't exist, or not enough equations given!
					if (unsolved_index >= 0) return -1;
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
