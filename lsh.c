#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

int dimension;

double normal_distribution_number()     // random number that follows the normal distribution
{
    // srand(time(0));
    double vect1 = (rand() / ((double)RAND_MAX + 1));
    double vect2 = (rand() / ((double)RAND_MAX + 1));
    double number = sqrt(-2*log(vect1)) * cos(2*3.14*vect2);  // mean μ =0, variance σ^2 = 1

    return number;
}

double vectors_dot_product(char* p[0][dimension + 1], double v[], int index)   // dot product of two vectors
{
    double product = 0.0;
    for(int i=0; i<dimension; i++)
    {
        product = product + atof(p[index][i + 1]) * v[i];   // calculation of dot product
    }
    return product;
}

double h_function(char* p[0][dimension + 1], int index)     // calculation of h function
{
    double v[dimension];
    for(int i=0; i<dimension; i++)
    {
        v[i] = normal_distribution_number();    // find random v vector that follows the normal distribution
    }

    double dot_result = vectors_dot_product(p, v, index);   // calculate the dot product of the to vectors
    int w = 4;  // stable
    double t = (double)(rand() % 4);    // calculate t uniformly in range [0, w)

    double h_result = floor((dot_result + t) / w);  // result of h function

    return h_result;
}


int main(int argc, char* argv[])
{
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
    int input_items_counter = 0;
    for (c = getc(input_file_ptr); c != EOF; c = getc(input_file_ptr))  // count the items of the file input(axis x)
    {
        if (c == '\n')
            input_items_counter = input_items_counter + 1;
    }
    
    rewind(query_file_ptr);    
    rewind(input_file_ptr);

    dimension = 0;
    while((c = fgetc(query_file_ptr)) != '\n')  // calculate the dimension of all the vectors. One vector is enough.
    {
        if(c == ' ')
            dimension++;
    }
    printf("Dimension: %d\n", dimension);
    rewind(query_file_ptr);


    // Work for input file
    char* p[input_items_counter][dimension + 1];    // array of the items of dataset
    for(int i=0; i<input_items_counter; i++)
        for(int j=0; j<=dimension; j++)
            p[i][j] = malloc(sizeof(char*) + 1);  // allocate memory for the array

    int i = 0, j = 0;
    while(fscanf(input_file_ptr, "%s", p[i][j]) != EOF)     // fill the array with the dataset
    {
        j++;
        int c = fgetc(input_file_ptr);
        if(c == '\n')
        {
            i++;
            j = 0;
        }
    }

    int k = 4;
    double h_p_result[input_items_counter][k]; // array with the results of the h function
    for(int i=0; i<input_items_counter; i++)
    {
        srand(time(0));
        printf("Line %d\n", i);
        for(int j=0; j<k; j++)
        {
            h_p_result[i][j] = h_function(p, i);   // h_function
            printf("%f\n", h_p_result[i][j]);
        }
    }

    // Hash table for input file
    int g_for_p[input_items_counter];
    // int TableSize = input_items_counter / 4;
    int TableSize = 4;
    int M = (int)pow(2, 32) - 5;
    for(int i=0; i<input_items_counter; i++)
    {
        printf("Line %d\n", i);
        g_for_p[i] = 0;
        for(int j=0; j<k; j++)
        {
            int r = (rand() % 20);
            g_for_p[i] = g_for_p[i] + (int)h_p_result[i][j] * r;
        }
        g_for_p[i] = ((g_for_p[i] % M) + M) % M;    // mod M
        g_for_p[i] = ((g_for_p[i] % TableSize) + TableSize) % TableSize;    // mod TableSize
        printf("%d\n", g_for_p[i]);
    }



    // Same for query file
    char* q[query_items_counter][dimension + 1];    // array of the items of dataset
    for(int i=0; i<query_items_counter; i++)
        for(int j=0; j<=dimension; j++)
            q[i][j] = malloc(sizeof(char*) + 1);  // allocate memory for the array

    i = 0;
    j = 0;
    while(fscanf(query_file_ptr, "%s", q[i][j]) != EOF)     // fill the array with the dataset
    {
        j++;
        int c = fgetc(query_file_ptr);
        if(c == '\n')
        {
            i++;
            j = 0;
        }
    }

    double h_q_result[query_items_counter][k]; // array with the results of the h function
    printf("\nQuery\n");
    for(int i=0; i<query_items_counter; i++)
    {
        srand(time(0));
        printf("Line %d\n", i);
        for(int j=0; j<k; j++)
        {
            h_q_result[i][j] = h_function(q, i);   // h_function
            // printf("%f\n", h_q_result[i][j]);
        }
    }

    // Hash table for query file
    int g_for_q[query_items_counter];
    // TableSize = query_items_counter / 4;
    TableSize = 3;
    M = (int)pow(2, 32) - 5;
    for(int i=0; i<query_items_counter; i++)
    {
        printf("Line %d\n", i);
        g_for_q[i] = 0;
        for(int j=0; j<k; j++)
        {
            int r = (rand() % 20);
            g_for_q[i] = g_for_q[i] + (int)h_q_result[i][j] * r;
        }
        g_for_q[i] = ((g_for_q[i] % M) + M) % M;    // mod M
        g_for_q[i] = ((g_for_q[i] % TableSize) + TableSize) % TableSize;    // mod TableSize
        printf("%d\n", g_for_q[i]);
    }




    fclose(query_file_ptr);
    fclose(input_file_ptr);
    return 0;
}
