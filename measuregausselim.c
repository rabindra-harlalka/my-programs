// vim: noai:ts=4:sw=4


/*
recipe:
> `gcc --std=c99 -DNOMAIN gausselim.c measuregausselim.c -o measuregausselim`
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>

extern int eliminate(int m, int n, double A[][n], bool do_partial_pivoting, bool augmented_matrix);
extern int substitute(int m, int n, double A[][n], double* x);

long get_elapsed_us(struct timeval *begin)
{
    struct timeval end;
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin->tv_sec;
    long microseconds = end.tv_usec - begin->tv_usec;
    long elapsed = seconds * 1e6 + microseconds;

    return elapsed;
}

int main()
{
    printf("n,elim,subs\n");
    for (int n = 5; n <= 1000; n+= n < 100 ? 5 : 50)
    {
        // generate random agumented matrix A of size n
        double A[n][n+1];
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j <= n; j++)
            {
                A[i][j] = rand() % 100;
            }
        }
        // run Gaussian elimination and substitution on A and measure time taken for each
        // step 1:
        struct timeval begin;
        gettimeofday(&begin, 0);

        eliminate(n, n, A, true, true);

        long time_step1 = get_elapsed_us(&begin);

        // step 2:
        double x[n];

        gettimeofday(&begin, 0);
        substitute(n, n, A, x);

        long time_step2 = get_elapsed_us(&begin);

        printf("%d,%ld,%ld\n", n, time_step1, time_step2);
    }

    return 0;
}
