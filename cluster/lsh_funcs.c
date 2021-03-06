#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "lsh_funcs.h"

static float normal_distribution_number()     // random number that follows the normal distribution
{
    float vect1 = (rand() / ((float)RAND_MAX + 1));
    float vect2 = (rand() / ((float)RAND_MAX + 1));
    float number = sqrt(-2*log(vect1)) * cos(2*3.14*vect2);  // mean μ = 0, variance σ^2 = 1

    return number;
}

static float vectors_dot_product(int** p, float* v, int index, int dimension)   // dot product of two vectors
{
    float product = 0.0;
    for(int i=0; i<dimension; i++)
    {
        product = product + (double)p[index][i + 1] * v[i];   // calculation of dot product
    }
    return product;
}

float h_function(int** p, int index, int dimension)     // calculation of h function
{
    float* v = malloc(sizeof(float) * dimension);
    for(int i=0; i<dimension; i++)
    {
        v[i] = normal_distribution_number();    // find random v vector that follows the normal distribution
    }

    float dot_result = vectors_dot_product(p, v, index, dimension);   // calculate the dot product of the to vectors
    int w = 6;  // stable
    float t = (float)(rand() % 4);    // calculate t uniformly in range [0, w)

    float h_result = floor((dot_result + t) / w);  // result of h function

    return h_result;
}

