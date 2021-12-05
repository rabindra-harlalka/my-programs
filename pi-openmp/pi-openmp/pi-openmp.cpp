// pi-openmp.cpp : Defines the entry point for the application.
//

#include "pi-openmp.h"

#include <omp.h>
//#include <stdlib.h>

using namespace std;

int main() {
    constexpr int MAX_THREADS = 8;
	constexpr long steps = 1'000'000'000;

    const double step = 1.0 / static_cast<double>(steps);

	for (auto j = 1; j <= MAX_THREADS; j++) {
		std::cout << "Running on " << j << " threads" << std::endl;
		// This is the beginning of a single PI computation
		omp_set_num_threads(j);
		double sum = 0.0;
		double start = omp_get_wtime();

		double x;
        #pragma omp parallel for reduction(+:sum) private(x)
		for (auto i = 0; i < steps; i++) {
			/*double */x = (i + 0.5) * step;
			sum += 4.0 / (1.0 + x * x);
		}
		// Out of the parallel region, finalize computation
		double pi = step * sum;
		double delta = omp_get_wtime() - start;
		printf("PI = %.16g computed in %.4g seconds\n", pi, delta);
	}

	return 0;
}
