#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <limits.h>
#include "lsh_funcs.h"
#include "cube_funcs.h"


int main(int argc, char* argv[])
{
    // parameters check
    char* input_file = malloc(sizeof(char*) + 1);
    char* query_file = malloc(sizeof(char*) + 1);
    int k;
    int M;
    int probes;
    char* output_file = malloc(sizeof(char*) + 1);
    int N;
    int R;

    if(argc == 17)
    {
        for(int i=1; i<=15; i=i+2)
        {
            if(strcmp(argv[i], "-i") == 0)
                input_file = argv[i+1];
            else if(strcmp(argv[i], "-q") == 0)
                query_file = argv[i+1];
            else if(strcmp(argv[i], "-k") == 0)
                k = atoi(argv[i+1]);
            else if(strcmp(argv[i], "-M") == 0)
                M = atoi(argv[i+1]);
            else if(strcmp(argv[i], "-probes") == 0)
                probes = atoi(argv[i+1]);
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
        k = 3;
        M = 10;
        probes = 2;
        N = 2;
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

    
    // create 2 tables with 2 dimension  and we put inside 0,1//
    int** f = (int**)malloc(input_items_counter * sizeof(int*));
    for(int i=0; i<input_items_counter; i++)
    {
        f[i] = (int*)malloc(sizeof(int) * k);
    }
    for (int i=0;i<input_items_counter; i++){
        for (int j=0;j<k;j++){
            f[i][j]=rand() % 2;
        }
    }

    // Work for input file
    int** p = malloc(sizeof(int*) * input_items_counter * sizeof(int*));    // array of the items of dataset
    for(int i=0; i<input_items_counter; i++)
        p[i] = malloc(sizeof(int) * (dimension + 1));
    
    int i=0, j=0;
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


    float** h_p_result = malloc(sizeof(float*) * input_items_counter * sizeof(int*)); // array with the results of the h function
    for(int i=0; i<input_items_counter; i++)
        h_p_result[i] = malloc(sizeof(float) * k);
    for(int i=0; i<input_items_counter; i++)
    {
        srand(time(0));
        for(int j=0; j<k; j++)
        {
            h_p_result[i][j] = h_function(p, i, dimension);   // h_function
            for(int a=0;a<i;a++){
                for(int b=0;b<j;b++){
                    if(h_p_result[i][j] == h_p_result[a][b]){   //if we find the same result we put the number 0 in f table//
                        f[i][j]=0;
                    }
                }
            }
        }
    }
    int * a=h_p(f,k,input_items_counter);

    // Hash table for input file
    int TableSize = pow(2,k);
    int hash_index;
    struct Hash_Node* hash_tables[input_items_counter];
    for(int i=0; i<(input_items_counter); i++)
    {
        hash_tables[i] = NULL;
    }

    int ID;

    // we put them in hash table
    for(int i=0; i<input_items_counter; i++)
    {
        hash_index =a[i];
        ID=hash_index;
        hash_index= con(a[i],k);
        struct Hash_Node* data_item = (struct Hash_Node*)malloc(sizeof(struct Hash_Node));
        data_item->item = i + 1;
        data_item->ID = ID;
        if(hash_tables[hash_index] == NULL)
        {
            hash_tables[hash_index] = data_item;
            hash_tables[hash_index]->ID = ID;
        }
        else
        {
            struct Hash_Node* temp = hash_tables[hash_index];
            while(hash_tables[hash_index]->next != NULL)
            {
                hash_tables[hash_index] = hash_tables[hash_index]->next;
            }
            hash_tables[hash_index]->next = data_item;
            hash_tables[hash_index]->next->ID = ID;
            hash_tables[hash_index] = temp;
        }
    }

    if(argc != 17)
    {
        printf("Give query file\n");
        scanf("%s", query_file);
        printf("Give output file\n");
        scanf("%s", output_file);
    }

    FILE *query_file_ptr;
    query_file_ptr = fopen(query_file, "r");    // open query file
    if(query_file_ptr == NULL)
    {
        perror("Error\n");
        exit(1);
    }

    int query_items_counter = 0;
    for (c = getc(query_file_ptr); c != EOF; c = getc(query_file_ptr))  // count the items of the file query(axis x)
    {
        if (c == '\n')
            query_items_counter = query_items_counter + 1;
    }
    rewind(query_file_ptr);

    int** f1 = (int**)malloc(query_items_counter * sizeof(int*));
    for(int i=0; i<query_items_counter; i++)
    {
        f1[i] = (int*)malloc(sizeof(int) * k);
    }
    for (i=0;i<query_items_counter; i++){
        for (j=0;j<k;j++){
            f1[i][j]=rand() % 2;
        }
    }

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


    float** h_q_result = malloc(sizeof(float*) * query_items_counter); // array with the results of the h function
    for(int i=0; i<query_items_counter; i++)
        h_q_result[i] = malloc(sizeof(float) * k);
    for(int i=0; i<query_items_counter; i++)
    {
        srand(time(0));
        for(int j=0; j<k; j++)
        {
            h_q_result[i][j] = h_function(q, i, dimension);   // h_function
            for(int a=0;a<i;a++){
                for(int b=0;b<j;b++){
                    if(h_q_result[i][j] == h_q_result[a][b]){   //if we find the same result we put the number 0 in f1 table//
                        f1[i][j]=0;
                    }
                }
            }
        }
    }


    FILE *output_file_ptr;
    output_file_ptr = fopen(output_file, "w");    // open output file
    if(output_file_ptr == NULL)
    {
        perror("Error\n");
        exit(1);
    }

    int * a1=h_p(f1,k,query_items_counter);

        struct Hash_Node* temp;
        for(int m=0; m<query_items_counter; m++)    // for every query show the results
        {
            hash_index= a1[m];
            int k_ID=hash_index;
            hash_index= con(a1[m],k);
            int min_dist = INT_MAX;
            int nearest_neighbor = -1;
            temp = hash_tables[hash_index];
            int counter = 0;
            while(hash_tables[hash_index] != NULL)   // count the list
            {
                counter++;
                hash_tables[hash_index] = hash_tables[hash_index]->next;
            }
            int *k_min_dist=malloc(sizeof(int)*counter);
            int * k_nearest_neighbor=malloc(sizeof(int)*counter);
            i = 0, j = 0;
            int cp;
            hash_tables[hash_index] = temp;
            int *radius=malloc(sizeof(int)*counter);
            struct timeval start, stop;
            gettimeofday(&start, 0);
            for(int t=0;t<TableSize;t++){
                while(hash_tables[hash_index] != NULL)
                {
                    cp=0;
                    int hdi=hammingDistance(hash_index,t);
                        if((k_ID == hash_tables[hash_index]->ID || hdi<=probes ) && cp<=M)  // compare the IDs or haming distance <=probes and counter of points<=M
                    {
                    int dist = 0;
                    for(int d=1; d<=dimension; d++)
                    {
                        dist = dist + pow((q[0][d] - p[hash_tables[hash_index]->item - 1][d]), 2);
                    }
                    dist = sqrt(dist);
                    if(dist < min_dist) // minimun LSH distance
                    {
                        min_dist = dist;
                        nearest_neighbor = hash_tables[hash_index]->item;
                    }
                    // k nearest neighbors
                    k_min_dist[i] = dist;
                    k_nearest_neighbor[i] = hash_tables[hash_index]->item;
                    i++;
                    // vectors in range r
                    if(dist < R)
                    {
                        radius[j] = hash_tables[hash_index]->item;
                        j++;
                    }
                }
                hash_tables[hash_index] = hash_tables[hash_index]->next;        
            }
            
        }
        hash_tables[hash_index] = temp;
        gettimeofday(&stop, 0);
        long sec = stop.tv_sec - start.tv_sec;
        long mic_sec = stop.tv_usec - start.tv_usec;
        double hypercube_time = sec + mic_sec*1e-6;

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
        double k_hypercube_time = hypercube_time + sec + mic_sec*1e-6;

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
            if(dist < true_min_dist && dist >= 0) // minimun Hypercube distance
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
        for(int i=0; i<input_items_counter - 1; i++)
        {
            for(int j=0; j<input_items_counter-i-1; j++)
            {
                if(k_true_min_dist[j] > k_true_min_dist[j + 1])
                {
                    s = k_true_min_dist[j];
                    k_true_min_dist[j] = k_true_min_dist[j + 1];    // true distance for k neighbors
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

            // print Hypercube distance
            fprintf(output_file_ptr, "distanceHypercube: %f\n", (double)min_dist);

            // print true distance
            fprintf(output_file_ptr, "distanceTrue: %f\n", (double)true_min_dist);

            // print the find time of the distance
            fprintf(output_file_ptr, "tHypercube: %.7f\n", hypercube_time);

            // print the real find time of the distance
            fprintf(output_file_ptr, "tTrue: %.7f\n", true_time);


            // print k nearest neighbors and distances
            for(int i=0; i<counter; i++)
            {
                fprintf(output_file_ptr, "Nearest neighbor-%d: %d\n", i+1, k_nearest_neighbor[i]);
                fprintf(output_file_ptr, "distanceHypercube: %f\n", (double)k_min_dist[i]);
                fprintf(output_file_ptr, "distanceTrue: %f\n", (double)k_true_min_dist[i]);
                if(i == (N - 1))
                    break;
            }

            // print the find time of the distances from k neighbors
            fprintf(output_file_ptr, "tHypercube: %.7f\n", k_hypercube_time);
            
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

        free(k_min_dist);
        free(k_nearest_neighbor);    //free memory
        free(radius);
    }


    fclose(query_file_ptr);
    fclose(input_file_ptr);

    //free memory
    for(int i=0; i<input_items_counter; i++)
    {
        free(f[i]);
        free(p[i]);
        free(h_p_result[i]);
    }
    free(f);
    free(p);
    free(h_p_result);

    for(int i=0; i<query_items_counter; i++)
    {
        free(f1[i]);
        free(q[i]);
        free(h_q_result[i]);
    }
    free(f1);
    free(q);
    free(h_q_result);

    free(a);
    free(a1);

    return 0;
}