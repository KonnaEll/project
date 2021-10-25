#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

double normal_distribution_number()     // random number that follows the normal distribution
{
    double vect1 = (rand() / ((double)RAND_MAX + 1));
    double vect2 = (rand() / ((double)RAND_MAX + 1));
    double number = sqrt(-2*log(vect1)) * cos(2*3.14*vect2);  // mean μ =0, variance σ^2 = 1

    return number;
}

double vectors_dot_product(double p[], double q[], int n)   // dot product of two vectors
{
    double product = 0.0;
    for(int i=0; i<=n; i++)
    {
        product = product + p[i] * q[i];
    }
    return product;
}

int h_function(int n)
{
    double v[n];
    for(int i=0; i<n; i++)
    {
        v[i] = normal_distribution_number();
    }

    return 0;
}


int main(int argc, char* argv[])
{
    srand(time(0));
    printf("%f\n", normal_distribution_number());

    FILE *query_file_ptr;
    query_file_ptr = fopen("query", "r");    // open query file
    if(query_file_ptr == NULL)
    {
        perror("Error\n");
        exit(1);
    }
    FILE *input_file_ptr;
    input_file_ptr = fopen("input", "r");    // open input file
    if(input_file_ptr == NULL)
    {
        perror("Error\n");
        exit(1);
    }


    char c;
    int query_items_counter = 0;
    for (c = getc(query_file_ptr); c != EOF; c = getc(query_file_ptr))  // count the items of the file query(axis x)
    {
        if (c == '\n')
            query_items_counter = query_items_counter + 1;
    }
    printf("%d\n", query_items_counter);
    int input_items_counter = 0;
    for (c = getc(input_file_ptr); c != EOF; c = getc(input_file_ptr))  // count the items of the file input(axis x)
    {
        if (c == '\n')
            input_items_counter = input_items_counter + 1;
    }
    printf("%d\n", input_items_counter);
    
    rewind(query_file_ptr);    
    rewind(input_file_ptr);

    int dimension = 0;
    while((c = fgetc(query_file_ptr)) != '\n')
    {
        if(c == ' ')
            dimension++;
    }
    printf("%d\n", dimension);

    rewind(query_file_ptr);



    h_function(input_dimension);






    fclose(query_file_ptr);
    fclose(input_file_ptr);
    return 0;
}
