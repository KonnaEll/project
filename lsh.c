#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

int dimension;

float normal_distribution_number()     // random number that follows the normal distribution
{
    float vect1 = (rand() / ((float)RAND_MAX + 1));
    float vect2 = (rand() / ((float)RAND_MAX + 1));
    float number = sqrt(-2*log(vect1)) * cos(2*3.14*vect2);  // mean μ =0, variance σ^2 = 1

    return number;
}

float vectors_dot_product(int p[0][dimension + 1], float v[], int index)   // dot product of two vectors
{
    float product = 0.0;
    for(int i=0; i<dimension; i++)
    {
        product = product + (double)p[index][i + 1] * v[i];   // calculation of dot product
    }
    return product;
}

float h_function(int p[0][dimension + 1], int index)     // calculation of h function
{
    float v[dimension];
    for(int i=0; i<dimension; i++)
    {
        v[i] = normal_distribution_number();    // find random v vector that follows the normal distribution
    }

    float dot_result = vectors_dot_product(p, v, index);   // calculate the dot product of the to vectors
    int w = 6;  // stable
    float t = (float)(rand() % 4);    // calculate t uniformly in range [0, w)

    float h_result = floor((dot_result + t) / w);  // result of h function

    return h_result;
}

struct Hash_Node
{
    int item;
    int ID;
    struct Hash_Node* next;
};


int main(int argc, char* argv[])
{
    FILE *query_file_ptr;
    query_file_ptr = fopen("query_small_id", "r");    // open query file
    if(query_file_ptr == NULL)
    {
        perror("Error\n");
        exit(1);
    }
    FILE *input_file_ptr;
    input_file_ptr = fopen("input_small_id", "r");    // open input file
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
    rewind(query_file_ptr);

    // Work for input file
    int p[input_items_counter][dimension + 1];    // array of the items of dataset
    int i = 0, j = 0;
    char* x = malloc(sizeof(char*) + 1);
    while(fscanf(input_file_ptr, "%s", x) != EOF)     // fill the array with the dataset
    {
        p[i][j] = atoi(x);
        j++;
        if(j == dimension)
        {
            i++;
            j = 0;
        }
    }
    // for(int i=0; i<input_items_counter; i++)
    //     for(int j=0; j<dimension; j++)
    //         printf("%d\n", p[i][j]);  // allocate memory for the array

    int k = 4;
    float h_p_result[input_items_counter][k]; // array with the results of the h function
    for(int i=0; i<input_items_counter; i++)
    {
        srand(time(0));
        for(int j=0; j<k; j++)
        {
            h_p_result[i][j] = h_function(p, i);   // h_function
        }
    }

    // Same for query file
    int q[query_items_counter][dimension + 1];    // array of the items of dataset
    i = 0, j = 0;
    char* y = malloc(sizeof(char*) + 1);
    while(fscanf(query_file_ptr, "%s", y) != EOF)     // fill the array with the dataset
    {
        q[i][j] = atoi(y);
        j++;
        if(j == dimension)
        {
            i++;
            j = 0;
        }
    }
    // for(int i=0; i<query_items_counter; i++)
    //     for(int j=0; j<dimension; j++)
    //         printf("%d\n", q[i][j]);  // allocate memory for the array

    float h_q_result[query_items_counter][k]; // array with the results of the h function
    for(int i=0; i<query_items_counter; i++)
    {
        srand(time(0));
        for(int j=0; j<k; j++)
        {
            h_q_result[i][j] = h_function(q, i);   // h_function
        }
    }


    // Hash table for input file
    int hash_index;
    int TableSize = input_items_counter / 8;
    int M = (int)pow(2, 32) - 5;
    int L = 5;
    struct Hash_Node* hash_tables[L][input_items_counter + query_items_counter];
    for(int n=0; n<L; n++)
    {
        for(int i=0; i<(input_items_counter + query_items_counter); i++)
        {
            hash_tables[n][i] = NULL;
        }
    }
    int r[k];
    int ID;
    for(int n=0; n<L; n++)
    {
        for(int i=0; i<k; i++)
        {
            r[i] = rand() % 10;
        }
        for(int i=0; i<input_items_counter; i++)
        {
            hash_index = 0;
            for(int j=0; j<k; j++)
            {
                hash_index = hash_index + (int)h_p_result[i][j] * r[j];
            }
            hash_index = hash_index % M;    // mod M
            if(hash_index < 0)
            {
                hash_index = hash_index * (-1);     // only positive number
            }
            ID = hash_index;
            hash_index = hash_index % TableSize;    // mod TableSize

            struct Hash_Node* data_item = (struct Hash_Node*)malloc(sizeof(struct Hash_Node));
            data_item->item = i + 1;
            data_item->ID = ID;
            if(hash_tables[n][hash_index] == NULL)
            {
                hash_tables[n][hash_index] = data_item;
                hash_tables[n][hash_index]->ID = ID;
            }
            else
            {
                struct Hash_Node* temp = hash_tables[n][hash_index];
                while(hash_tables[n][hash_index]->next != NULL)
                {
                    hash_tables[n][hash_index] = hash_tables[n][hash_index]->next;
                }
                hash_tables[n][hash_index]->next = data_item;
                hash_tables[n][hash_index]->next->ID = ID;
                hash_tables[n][hash_index] = temp;
            }
        }
    }

    // for(int i=0; i<L; i++)   // an thelw na vriskw to dianysma se kathe hash prepei na kanw disdiastato to r
    // {

    struct Hash_Node* temp;
    for(int m=0; m<query_items_counter; m++)    // for every query show the results
    {
        hash_index = 0;
        for(int j=0; j<k; j++)
        {
            hash_index = hash_index + (int)h_q_result[m][j] * r[j]; // find the bucket of the query
        }
        hash_index = hash_index % M;    // mod M
        if(hash_index < 0)
        {
            hash_index = hash_index * (-1);
        }
        int k_ID = hash_index;  // ID for comparison
        hash_index = hash_index % TableSize;    // mod TableSize

        int min_dist = 10000;
        int nearest_neighbor = -1;
        temp = hash_tables[4][hash_index];
        int counter = 0;
        while(hash_tables[4][hash_index] != NULL)   // count the list
        {
            counter++;
            hash_tables[4][hash_index] = hash_tables[4][hash_index]->next;
        }

        int N = 2;
        int R = 10000;
        int k_min_dist[counter];    // array of the distances
        int k_nearest_neighbor[counter];    // array of the items
        i = 0, j = 0;
        hash_tables[4][hash_index] = temp;
        int radius[counter];    // array for vectors within radius
        while(hash_tables[4][hash_index] != NULL)
        {
            if(k_ID == hash_tables[4][hash_index]->ID)  // compare the IDs
            {
                // calculate distance
                int dist = 0;
                for(int d=1; d<=dimension; d++)
                {
                    dist = dist + pow((q[0][d] - p[hash_tables[4][hash_index]->item - 1][d]), 2);
                }
                dist = sqrt(dist);
                if(dist < min_dist) // minimun LSH distance
                {
                    min_dist = dist;
                    nearest_neighbor = hash_tables[4][hash_index]->item;
                }

                // k nearest neighbors
                k_min_dist[i] = dist;
                k_nearest_neighbor[i] = hash_tables[4][hash_index]->item;
                i++;

                // vectors in range r
                if(dist < R)
                {
                    radius[j] = hash_tables[4][hash_index]->item;
                    j++;
                }
            }
            hash_tables[4][hash_index] = hash_tables[4][hash_index]->next;        
        }
        hash_tables[4][hash_index] = temp;
        
        // }
        
        // array of distances and items and sort them in ascending order
        int s;
        counter = i;
        for(int i=0; i<counter - 1; i++)
        {
            for(int j=0; j<counter-i-1; j++)
            {
                if(k_min_dist[j] > k_min_dist[j + 1])
                {
                    s = k_min_dist[j];
                    k_min_dist[j] = k_min_dist[j + 1];
                    k_min_dist[j + 1] = s;
                    s = k_nearest_neighbor[j];
                    k_nearest_neighbor[j] = k_nearest_neighbor[j + 1];
                    k_nearest_neighbor[j + 1] = s;
                }
            }
        }


        if(min_dist != 10000)
        {
            // print query
            printf("\nQuery: %d\n", q[m][0]);

            // print nearest neighbor
            printf("Nearest neighbor-1: %d\n", nearest_neighbor);

            // print LSH distance
            printf("distanceLSH: %d\n", min_dist);

            // print true distance
            int true_min_dist = 10000;
            for(int i=0; i<input_items_counter; i++)
            {
                int dist = 0;
                for(int d=1; d<=dimension; d++)
                {
                    dist = dist + pow((q[m][d] - p[i][d]), 2);
                }
                dist = sqrt(dist);
                printf("dist %d\n", dist);
                if(dist < true_min_dist && dist >= 0) // minimun LSH distance
                {
                    true_min_dist = dist;
                }
            }
            printf("distanceTrue: %d\n", true_min_dist);
            printf("%f\n", (double)true_min_dist/min_dist);

            // print k nearest neighbors and distances
            for(int i=0; i<counter; i++)
            {
                printf("Nearest neighbor-%d: %d\n", i+1, k_nearest_neighbor[i]);
                printf("distanceLSH: %d\n", k_min_dist[i]);
                
                // int true_true_min_dist = 10000;
                // nearest_neighbor = -1;
                // for(int i=0; i<input_items_counter; i++)
                // {
                //     int dist = 0;
                //     for(int d=1; d<=dimension; d++)
                //     {
                //         dist = dist + pow((q[m][d] - p[i][d]), 2);
                //     }
                //     dist = sqrt(dist);
                //     if(dist < true_min_dist) // minimun LSH distance
                //     {
                //         true_min_dist = dist;
                //         nearest_neighbor = p[i][0];
                //     }
                // }
                printf("distanceTrue: %d\n", true_min_dist);
                printf("%f\n", (double)true_min_dist/k_min_dist[i]);
                if(i == (N - 1))
                    break;
            }

            // print the vectors within radius R
            counter = j;
            printf("%d-near neighbors:\n", R);
            for(int i=0; i<counter; i++)
            {
                printf("%d\n", radius[i]);
            }
        }
        else
        {
            printf("\nQuery: %d\n", q[m][0]);
            printf("There is not a near vector in this bucket!\n");
        }



    }










    fclose(query_file_ptr);
    fclose(input_file_ptr);
    return 0;
}
