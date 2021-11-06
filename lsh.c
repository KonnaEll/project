#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <limits.h>


float normal_distribution_number()     // random number that follows the normal distribution
{
    float vect1 = (rand() / ((float)RAND_MAX + 1));
    float vect2 = (rand() / ((float)RAND_MAX + 1));
    float number = sqrt(-2*log(vect1)) * cos(2*3.14*vect2);  // mean μ = 0, variance σ^2 = 1

    return number;
}

float vectors_dot_product(int** p, float* v, int index, int dimension)   // dot product of two vectors
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

struct Hash_Node
{
    int item;
    int ID;
    struct Hash_Node* next;
};


int main(int argc, char* argv[])
{
    while(1)
    {
	// parameters check
    char* input_file = malloc(sizeof(char*) + 1);
    char* query_file = malloc(sizeof(char*) + 1);
    int k;
    int L;
    char* output_file = malloc(sizeof(char*) + 1);
    int N;
    int R;
	if(argc == 11)
	{
        for(int i=1; i<=9; i=i+2)
        {
            if(strcmp(argv[i], "-i") == 0)
                input_file = argv[i+1];
            else if(strcmp(argv[i], "-q") == 0)
                query_file = argv[i+1];
            else if(strcmp(argv[i], "-o") == 0)
                output_file = argv[i+1];
            else if(strcmp(argv[i], "-N") == 0)
                N = atoi(argv[i+1]);
            else if(strcmp(argv[i], "-R") == 0)
                R = atoi(argv[i+1]);
        }
        k = 4;
        L = 5;
        N = 1;
        R = 10000;
	}
    else if(argc == 15)
    {
        for(int i=1; i<=13; i=i+2)
        {
            if(strcmp(argv[i], "-i") == 0)
                input_file = argv[i+1];
            else if(strcmp(argv[i], "-q") == 0)
                query_file = argv[i+1];
            else if(strcmp(argv[i], "-k") == 0)
                k = atoi(argv[i+1]);
            else if(strcmp(argv[i], "-L") == 0)
                L = atoi(argv[i+1]);
            else if(strcmp(argv[i], "-o") == 0)
                output_file = argv[i+1];
            else if(strcmp(argv[i], "-N") == 0)
                N = atoi(argv[i+1]);
            else if(strcmp(argv[i], "-R") == 0)
                R = atoi(argv[i+1]);
        }
    }
    else
    {
        printf("Give dataset path\n");
        scanf("%s", input_file);
        printf("%s\n", input_file);
        k = 4;
        L = 5;
        N = 1;
        R = 10000;
    }


    FILE *input_file_ptr;
    input_file_ptr = fopen(input_file, "r");    // open input file
    if(input_file_ptr == NULL)
    {
        perror("Error\n");
        exit(1);
    }
    char c;
    int input_items_counter = 0;
    for (c = getc(input_file_ptr); c != EOF; c = getc(input_file_ptr))  // count the items of the file input(axis x)
    {
        if (c == '\n')
            input_items_counter = input_items_counter + 1;
    }
    rewind(input_file_ptr);
    int dimension = 0;
    while((c = fgetc(input_file_ptr)) != '\n')  // calculate the dimension of all the vectors. One vector is enough.
    {
        if(c == ' ')
            dimension++;
    }
    rewind(input_file_ptr);

    // Work for input file
    int** p = malloc(sizeof(int*) * input_items_counter);    // array of the items of dataset
    for(int i=0; i<input_items_counter; i++)
        p[i] = malloc(sizeof(int) * (dimension + 1));
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
    free(x);

    float** h_p_result = malloc(sizeof(float*) * input_items_counter); // array with the results of the h function
    for(int i=0; i<input_items_counter; i++)
        h_p_result[i] = malloc(sizeof(float) * k);
    for(int i=0; i<input_items_counter; i++)
    {
        srand(time(0));
        for(int j=0; j<k; j++)
        {
            h_p_result[i][j] = h_function(p, i, dimension);   // h_function
        }
    }

    // Hash table for input file
    int hash_index;
    int TableSize = input_items_counter / 8;
    int M = (int)pow(2, 32) - 5;
    struct Hash_Node* hash_tables[L][TableSize];
    for(int n=0; n<L; n++)
    {
        for(int i=0; i<(TableSize); i++)
        {
            hash_tables[n][i] = NULL;
        }
    }
    int** r = malloc(sizeof(int*) * L);
    for(int i=0; i<L; i++)
        r[i] = malloc(sizeof(int) * k);
    int ID;
    for(int n=0; n<L; n++)
    {
        for(int i=0; i<k; i++)
        {
            r[n][i] = rand() % 10;
        }
        for(int i=0; i<input_items_counter; i++)
        {
            hash_index = 0;
            for(int j=0; j<k; j++)
            {
                hash_index = hash_index + (int)h_p_result[i][j] * r[n][j];
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
    printf("rty\n");
    if(argc != 11 && argc != 15)
    {
        printf("Give query file\n");
        scanf("%s", query_file);
        printf("Give output file\n");
        scanf("%s", output_file);
    }
printf("th\n");

    FILE *query_file_ptr;
    query_file_ptr = fopen(query_file, "r");    // open query file
    if(query_file_ptr == NULL)
    {
        perror("Error\n");
        exit(1);
    }
printf("th2\n");

    int query_items_counter = 0;
    for (c = getc(query_file_ptr); c != EOF; c = getc(query_file_ptr))  // count the items of the file query(axis x)
    {
        if (c == '\n')
            query_items_counter = query_items_counter + 1;
    }
    rewind(query_file_ptr);
printf("th3\n");

    int** q = malloc(sizeof(int*) * query_items_counter);    // array of the items of dataset
    for(int i=0; i<query_items_counter; i++)
        q[i] = malloc(sizeof(int) * (dimension + 1));
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
    free(y);
printf("th5\n");

    float** h_q_result = malloc(sizeof(float*) * query_items_counter); // array with the results of the h function
    for(int i=0; i<query_items_counter; i++)
        h_q_result[i] = malloc(sizeof(float) * k);
    for(int i=0; i<query_items_counter; i++)
    {
        srand(time(0));
        for(int j=0; j<k; j++)
        {
            h_q_result[i][j] = h_function(q, i, dimension);   // h_function
        }
    }
printf("th6\n");

    FILE *output_file_ptr;
    output_file_ptr = fopen(output_file, "w");    // open output file
    if(output_file_ptr == NULL)
    {
        perror("Error\n");
        exit(1);
    }
printf("th7\n");

    // find the neighbors of each query
    for(int g=0; g<L; g++)
    {
        fprintf(output_file_ptr, "Hash Table no%d\n", g);
        struct Hash_Node* temp;
        for(int m=0; m<query_items_counter; m++)    // for every query show the results
        {
            hash_index = 0;
            for(int j=0; j<k; j++)
            {
                hash_index = hash_index + (int)h_q_result[m][j] * r[g][j]; // find the bucket of the query
            }
            hash_index = hash_index % M;    // mod M
            if(hash_index < 0)
            {
                hash_index = hash_index * (-1);
            }
            int k_ID = hash_index;  // ID for comparison
            hash_index = hash_index % TableSize;    // mod TableSize

            int min_dist = INT_MAX;
            int nearest_neighbor = -1;
            temp = hash_tables[g][hash_index];
            int counter = 0;
            while(hash_tables[g][hash_index] != NULL)   // count the list
            {
                counter++;
                hash_tables[g][hash_index] = hash_tables[g][hash_index]->next;
            }

            hash_tables[g][hash_index] = temp;
            int k_min_dist[counter];    // array of the distances
            int k_nearest_neighbor[counter];    // array of the items
            i = 0, j = 0;
            int radius[counter];    // array for vectors within radius
            struct timeval start, stop;
            gettimeofday(&start, 0);
            while(hash_tables[g][hash_index] != NULL)
            {
                if(k_ID == hash_tables[g][hash_index]->ID)  // compare the IDs
                {
                    // calculate distance
                    int dist = 0;
                    for(int d=1; d<=dimension; d++)
                    {
                        dist = dist + pow((q[0][d] - p[hash_tables[g][hash_index]->item - 1][d]), 2);
                    }
                    dist = sqrt(dist);
                    if(dist < min_dist) // minimun LSH distance
                    {
                        min_dist = dist;
                        nearest_neighbor = hash_tables[g][hash_index]->item;
                    }

                    // k nearest neighbors
                    k_min_dist[i] = dist;
                    k_nearest_neighbor[i] = hash_tables[g][hash_index]->item;
                    i++;

                    // vectors in range r
                    if(dist < R)
                    {
                        radius[j] = hash_tables[g][hash_index]->item;
                        j++;
                    }
                }
                hash_tables[g][hash_index] = hash_tables[g][hash_index]->next;        
            }
            gettimeofday(&stop, 0);
            long sec = stop.tv_sec - start.tv_sec;
            long mic_sec = stop.tv_usec - start.tv_usec;
            double lsh_time = sec + mic_sec*1e-6;

            // array of distances and items and sort them in ascending order
            int s;
            counter = i;
            gettimeofday(&start, 0);
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
            gettimeofday(&stop, 0);
            sec = stop.tv_sec - start.tv_sec;
            mic_sec = stop.tv_usec - start.tv_usec;
            double k_lsh_time = lsh_time + sec + mic_sec*1e-6;

            // calculate true distance and time
            int true_min_dist = INT_MAX;
            gettimeofday(&start, 0);
            for(int i=0; i<input_items_counter; i++)
            {
                int dist = 0;
                for(int d=1; d<=dimension; d++)
                {
                    dist = dist + pow((q[m][d] - p[i][d]), 2);
                }
                dist = sqrt(dist);
                if(dist < true_min_dist && dist >= 0) // minimun LSH distance
                {
                    true_min_dist = dist;
                }
            }
            gettimeofday(&stop, 0);
            sec = stop.tv_sec - start.tv_sec;
            mic_sec = stop.tv_usec - start.tv_usec;
            double true_time = sec + mic_sec*1e-6;


            // true distance of k neighbors
            int k_true_min_dist[input_items_counter];
            gettimeofday(&start, 0);
            for(int i=0; i<input_items_counter; i++)
            {
                int dist = 0;
                for(int d=1; d<=dimension; d++)
                {
                    dist = dist + pow((q[m][d] - p[i][d]), 2);
                }
                dist = sqrt(dist);

                // k nearest neighbors
                k_true_min_dist[i] = dist;

            }
            gettimeofday(&stop, 0);
            sec = stop.tv_sec - start.tv_sec;
            mic_sec = stop.tv_usec - start.tv_usec;
            double temp_true_k_time = sec + mic_sec*1e-6;


            gettimeofday(&start, 0);
            for(int i=0; i<counter - 1; i++)
            {
                for(int j=0; j<counter-i-1; j++)
                {
                    if(k_true_min_dist[j] > k_true_min_dist[j + 1])
                    {
                        s = k_true_min_dist[j];
                        k_true_min_dist[j] = k_true_min_dist[j + 1];
                        k_true_min_dist[j + 1] = s;
                    }
                }
            }
            gettimeofday(&stop, 0);
            sec = stop.tv_sec - start.tv_sec;
            mic_sec = stop.tv_usec - start.tv_usec;
            double true_k_time = temp_true_k_time + sec + mic_sec*1e-6;

            if(min_dist != INT_MAX)
            {
                // print query
                fprintf(output_file_ptr, "Query: %d\n", q[m][0]);

                // print nearest neighbor
                fprintf(output_file_ptr, "Nearest neighbor-1: %d\n", nearest_neighbor);

                // print LSH distance
                fprintf(output_file_ptr, "distanceLSH: %f\n", (double)min_dist);

                // print true distance
                fprintf(output_file_ptr, "distanceTrue: %f\n", (double)true_min_dist);

                // print the find time of the distance
                fprintf(output_file_ptr, "tLSH: %.7f\n", lsh_time);

                // print the real find time of the distance
                fprintf(output_file_ptr, "tTrue: %.7f\n", true_time);


                // print k nearest neighbors and distances
                for(int i=0; i<counter; i++)
                {
                    fprintf(output_file_ptr, "Nearest neighbor-%d: %d\n", i+1, k_nearest_neighbor[i]);
                    fprintf(output_file_ptr, "distanceLSH: %f\n", (double)k_min_dist[i]);
                    fprintf(output_file_ptr, "distanceTrue: %f\n", (double)k_true_min_dist[i]);
                    if(i == (N - 1))
                        break;
                }

                // print the find time of the distances from k neighbors
                fprintf(output_file_ptr, "tLSH: %.7f\n", k_lsh_time);
                
                // print the real find time of the distance
                fprintf(output_file_ptr, "tTrue: %.7f\n", true_k_time);


                // print the vectors within radius R
                counter = j;
                fprintf(output_file_ptr, "R-near neighbors:\n");
                for(int i=0; i<counter; i++)
                {
                    fprintf(output_file_ptr, "%d\n", radius[i]);
                }
            }
            else
            {
                fprintf(output_file_ptr, "Query: %d\n", q[m][0]);
                fprintf(output_file_ptr, "There is not a near vector in this bucket!\n");
            }
            fprintf(output_file_ptr, "\n");
        }
    }


    fclose(query_file_ptr);
    fclose(input_file_ptr);
    fclose(output_file_ptr);

    if(argc != 11 && argc != 15)
    {
        free(input_file);
        free(query_file);
        free(output_file);
    }

    free(p);
    for(int i=0; i<input_items_counter; i++)
    {
        free(p[i]);
    }

    // free(h_p_result);
    // for(int i=0; i<input_items_counter; i++)
    // {
    //     free(h_p_result[i]);
    // }

    free(q);
    for(int i=0; i<query_items_counter; i++)
    {
        free(q[i]);
    }

    // free(h_q_result);
    // for(int i=0; i<query_items_counter; i++)
    // {
    //     free(h_q_result[i]);
    // }

    // free(r);
    // for(int i=0; i<L; i++)
    // {
    //     free(r[i]);
    // }



    printf("~If you want to terminate the program press 'STOP'\n");
    printf("~If you want to continue the program press 'AGAIN'\n");
    char* response = malloc(sizeof(char*) + 1);
    scanf("%s", response);
    if(strcmp(response, "STOP") == 0)
        break;
    else if(strcmp(response, "AGAIN") == 0)
    {
        rewind(input_file_ptr);
        rewind(query_file_ptr);
        argc = 0;
        continue;
    }
    else
    {
        printf("Wrong input. Can't continue!\n");
        break;
    }



    }

    return 0;
}
