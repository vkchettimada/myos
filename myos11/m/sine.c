/* sine.c */
#include <math.h>

double sin(double x)
{
	double sum = x;
	double num = x;
	double den = 1.0;
	int i;

	for(i=1; i<MATH_TAYLOR_ITERATIONS; i++) {
		double term;
		
		num *= x*x;
		den *= 2*i*(2*i+1);
		term = num/den;
		if (i%2)
		{
			sum -= term;
		}
		else
		{
			sum += term;
		}
	}
	return(sum);
}

