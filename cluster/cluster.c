#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <limits.h>


int min_distance(int** p, int j, int** centroid_index, int number_of_clusters, int dimension)     // minimun distance of vector from a centroid
{
    int min = INT_MAX;
    int dist;
    for(int i=0; i<number_of_clusters; i++)
    {
        dist = 0;
        for(int d=1; d<=dimension; d++)
        {
            dist = dist + pow((centroid_index[i][d] - p[j][d]), 2);
        }
        dist = sqrt(dist);
        if(dist < min)
        {
            min = dist;
        }
    }
    return min;
}

int max_distance(int* D, int input_items_counter, int** p)
{
    int max = INT_MIN;
    int max_ind;
    for(int i=0; i<input_items_counter; i++)
    {
        if(D[i] > max)
        {
            max = D[i];
            max_ind = p[i][0];
        }
    }
    return max_ind;
}

int min_distance_index(int** p, int j, int** centroid_index, int number_of_clusters, int dimension)
{
    int min = INT_MAX;
    int dist;
    int min_index;
    for(int i=0; i<number_of_clusters; i++)
    {
        dist = 0;
        for(int d=1; d<=dimension; d++)
        {
            dist = dist + pow((centroid_index[i][d] - p[j][d]), 2);
        }
        dist = sqrt(dist);
        if(dist < min)
        {
            min = dist;
            min_index = i;
        }
    }
    return min_index;
}

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
	// parameters check
    char* input_file = malloc(sizeof(char*) + 1);
    char* configuration_file = malloc(sizeof(char*) + 1);
    char* output_file = malloc(sizeof(char*) + 1);
    char* method = malloc(sizeof(char*) + 1);
    int complete = 0;
	if(argc == 9)
	{
        for(int i=1; i<=7; i=i+2)
        {
            if(strcmp(argv[i], "-i") == 0)
                input_file = argv[i+1];
            else if(strcmp(argv[i], "-c") == 0)
                configuration_file = argv[i+1];
            else if(strcmp(argv[i], "-o") == 0)
                output_file = argv[i+1];
            else if(strcmp(argv[i], "-m") == 0)
                method = argv[i+1];
        }
	}
    else if(argc == 10)
    {
        for(int i=1; i<=9; i=i+2)
        {
            if(strcmp(argv[i], "-i") == 0)
                input_file = argv[i+1];
            else if(strcmp(argv[i], "-c") == 0)
                configuration_file = argv[i+1];
            else if(strcmp(argv[i], "-o") == 0)
                output_file = argv[i+1];
            else if(strcmp(argv[i], "-m") == 0)
                method = argv[i+1];
            else if(strcmp(argv[i], "-complete") == 0)
                complete = 1;
        }
    }
    else
    {
        printf("Wrong input. Try again!\n");
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
    int** p = (int**)malloc(input_items_counter * sizeof(int*));
    for (int i = 0; i < input_items_counter; i++)
    {
        p[i] = (int*)malloc((dimension+1) * sizeof(int));
    }
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

    FILE *output_file_ptr;
    output_file_ptr = fopen(output_file, "w");    // open output file
    if(output_file_ptr == NULL)
    {
        perror("Error\n");
        exit(1);
    }

    FILE *conf_file_ptr;
    conf_file_ptr = fopen(configuration_file, "r");    // open input file
    if(conf_file_ptr == NULL)
    {
        perror("Error\n");
        exit(1);
    }

    int number_of_clusters;
    int number_of_vector_hash_tables;
    int number_of_vector_hash_functions;
    int max_number_M_hypercube;
    int number_of_hypercube_dimensions;
    int number_of_probes;
    i = 0;
    char* numb = malloc(sizeof(char*) + 1);
    while(fscanf(conf_file_ptr, "%s", numb) != EOF)     // fill the array with the dataset
    {
        if(i == 3)
            number_of_clusters = atoi(numb);
        else if(i == 6)
            number_of_vector_hash_tables = atoi(numb);
        else if(i == 9)
            number_of_vector_hash_functions = atoi(numb);
        else if(i == 12)
            max_number_M_hypercube = atoi(numb);
        else if(i == 15)
            number_of_hypercube_dimensions = atoi(numb);
        else if(i == 18)
            number_of_probes = atoi(numb);
        i++;
    }

    // Initialize the centroids with K-means++ method
    srand(time(0));
    int** centroid_index = malloc(sizeof(int*) * number_of_clusters);
    for(int i=0; i<number_of_clusters; i++)
    {
        centroid_index[i] = malloc(sizeof(int) * (dimension + 1));
    }
    int item_id = rand() % input_items_counter;    
    centroid_index[0][0] = item_id;
    for(int d=1; d<=dimension; d++)
    {
        centroid_index[0][d] = p[item_id - 1][d];
    }
    int* D = malloc(input_items_counter * sizeof(int));
    int index;
	for(int i=1; i<number_of_clusters; i++)
    {
		for(int j=0; j<input_items_counter; j++)
        {
			D[j] = min_distance(p, j, centroid_index, i, dimension);
		}

        index = max_distance(D, input_items_counter, p);
        centroid_index[i][0] = index;

        for(int d=1; d<=dimension; d++)
        {
            centroid_index[i][d] = p[index - 1][d];
        }
	}

    for(int i=0; i<number_of_clusters;i++)
    {
        for(int j=0; j<dimension; j++)
            printf("%d ", centroid_index[i][j]);
        printf("\n\n\n");
    }


    if(strcmp(method, "Classic") == 0)
    {
        // Lloyd's algorithm
        int* cluster_per_item = malloc(sizeof(int) * input_items_counter);
        int** previous_centroid_index = malloc(sizeof(int*) * number_of_clusters);
        for(int i=0; i<number_of_clusters; i++)
        {
            previous_centroid_index[i] = malloc(sizeof(int) * (dimension + 1));
        }
        for(int i=0; i<number_of_clusters; i++)
        {
            for(int z=1; z<dimension; z++)
            {
                previous_centroid_index[i][z] == -1;
            }
        }
        int count;
        while(1)
        {
            for(int j=0; j<input_items_counter; j++)
            {
                cluster_per_item[j] = min_distance_index(p, j, centroid_index, number_of_clusters, dimension);
            }

            // Fix the new centroids
            int sum;
            for(int i=0; i<number_of_clusters; i++)
            {
                count = 0;
                centroid_index[i][0] = i;
                for(int z=1; z<dimension; z++)
                {
                    sum = 0;
                    for(int j=0; j<input_items_counter; j++)
                    {
                        if(cluster_per_item[j] == i)
                        {
                            sum = sum + p[j][z];
                            count++;
                        }
                    }
                    centroid_index[i][z] = sum / count;
                }
            }

            count = 0;
            for(int i=0; i<number_of_clusters; i++)
            {
                for(int z=1; z<dimension; z++)
                {
                    if(previous_centroid_index[i][z] != centroid_index[i][z])
                    {
                        count++;
                    }
                    previous_centroid_index[i][z] = centroid_index[i][z];
                }
            }
            if(count == 0)
            {
                break;
            }
        }
        fprintf(output_file_ptr, "Algorithm: Lloyd's\n");
        for(int m=0; m<number_of_clusters; m++)
        {
            fprintf(output_file_ptr, "CLUSTER-%d ", m+1);
            fprintf(output_file_ptr, "(centroid: ");
            for(int i=0; i<dimension; i++)
                fprintf(output_file_ptr, "%d ", centroid_index[m][i]);
            fprintf(output_file_ptr, ")\n\n");
        }
    }
    else if(strcmp(method, "LSH") == 0)     // LSH method
    {
        fprintf(output_file_ptr, "Algorithm: LSH\n");
        float** h_p_result = malloc(sizeof(float*) * input_items_counter); // array with the results of the h function
        for(int i=0; i<input_items_counter; i++)
            h_p_result[i] = malloc(sizeof(float) * number_of_vector_hash_functions);
        for(int i=0; i<input_items_counter; i++)
        {
            srand(time(0));
            for(int j=0; j<number_of_vector_hash_functions; j++)
            {
                h_p_result[i][j] = h_function(p, i, dimension);   // h_function
            }
        }

        // Hash table for input file
        int hash_index;
        int TableSize = input_items_counter / 6;
        int M = (int)pow(2, 32) - 5;
        struct Hash_Node* hash_tables[number_of_vector_hash_tables][TableSize];
        for(int n=0; n<number_of_vector_hash_tables; n++)
        {
            for(int i=0; i<(TableSize); i++)
            {
                hash_tables[n][i] = NULL;
            }
        }
        int** initial_centroid = malloc(sizeof(int*) * number_of_clusters);
        for(int i=0; i<number_of_clusters; i++)
        {
            initial_centroid[i] = malloc(sizeof(int) * (dimension + 1));
        }
        for(int i=0; i<number_of_clusters; i++)
        {
            for(int z=1; z<dimension; z++)
            {
                initial_centroid[i][z] = centroid_index[i][z];
            }
        }
        int r[number_of_vector_hash_tables][number_of_vector_hash_functions];
        int ID;
        for(int g=0; g<number_of_vector_hash_tables; g++)
        {
            // me ta sxolia vriskei diaforetika gt pairnei to teleutaio tou 3ou cluster sto kainourgio hash
            // xwris sxolia einai ola ta idia


            // for(int i=0; i<number_of_clusters; i++)
            // {
            //     for(int z=1; z<dimension; z++)
            //     {
            //         centroid_index[i][z] = initial_centroid[i][z];
            //     }
            // }
            

            struct timeval start, stop;
            gettimeofday(&start, 0);
            for(int i=0; i<number_of_vector_hash_functions; i++)
            {
                r[g][i] = rand() % 10;
            }
            for(int i=0; i<input_items_counter; i++)
            {
                hash_index = 0;
                for(int j=0; j<number_of_vector_hash_functions; j++)
                {
                    hash_index = hash_index + (int)h_p_result[i][j] * r[g][j];
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
                if(hash_tables[g][hash_index] == NULL)
                {
                    hash_tables[g][hash_index] = data_item;
                    hash_tables[g][hash_index]->ID = ID;
                }
                else
                {
                    struct Hash_Node* temp = hash_tables[g][hash_index];
                    while(hash_tables[g][hash_index]->next != NULL)
                    {
                        hash_tables[g][hash_index] = hash_tables[g][hash_index]->next;
                    }
                    hash_tables[g][hash_index]->next = data_item;
                    hash_tables[g][hash_index]->next->ID = ID;
                    hash_tables[g][hash_index] = temp;
                }
            }

            int** previous_centroid_index = malloc(sizeof(int*) * number_of_clusters);
            for(int i=0; i<number_of_clusters; i++)
            {
                previous_centroid_index[i] = malloc(sizeof(int) * (dimension + 1));
            }
            for(int i=0; i<number_of_clusters; i++)
            {
                for(int z=1; z<dimension; z++)
                {
                    previous_centroid_index[i][z] == -1;
                }
            }

            
            int** radius = malloc(sizeof(int*) * number_of_clusters);    // array for vectors within radius
            for(int i=0; i<number_of_clusters; i++)
                radius[i] = malloc(sizeof(int) * input_items_counter);
            for(int i=0; i<number_of_clusters; i++)
                for(int j=0; j<input_items_counter; j++)
                    radius[i][j] = -1;

            int j[number_of_clusters];
            int j_temp[number_of_clusters];
            for(int i=0; i<number_of_clusters; i++)
            {
                j[i] = 0;
                j_temp[i] = -1;
            }
            while(1)
            {
                float** h_q_result = malloc(sizeof(float*) * number_of_clusters); // array with the results of the h function
                for(int i=0; i<number_of_clusters; i++)
                    h_q_result[i] = malloc(sizeof(float) * number_of_vector_hash_functions);
                for(int i=0; i<number_of_clusters; i++)
                {
                    srand(time(0));
                    for(int j=0; j<number_of_vector_hash_functions; j++)
                    {
                        h_q_result[i][j] = h_function(centroid_index, i, dimension);   // h_function
                    }
                }

                int min = INT_MAX;
                int dist;
                for(int i=0; i<number_of_clusters; i++)     // find first radius (minimum distance among all centroids)
                {
                    for(int j=0; j<number_of_clusters; j++)
                    {
                        if(i != j)
                        {
                            dist = 0;
                            for(int d=1; d<=dimension; d++)
                            {
                                dist = dist + pow((centroid_index[i][d] - centroid_index[j][d]), 2);
                            }
                            dist = sqrt(dist);
                            if(dist < min)
                            {
                                min = dist;
                            }
                        }
                    }
                }
                int R = min / 2;

                int q_hash_index[number_of_clusters];
                int k_ID[number_of_clusters];

                struct Hash_Node* temp[number_of_vector_hash_tables][TableSize];
                for(int m=0; m<number_of_clusters; m++)    // for every query show the results
                {
                    q_hash_index[m] = 0;
                    for(int j=0; j<number_of_vector_hash_functions; j++)
                    {
                        q_hash_index[m] = q_hash_index[m] + (int)h_q_result[m][j] * r[g][j]; // find the bucket of the query
                    }
                    q_hash_index[m] = q_hash_index[m] % M;    // mod M
                    if(q_hash_index[m] < 0)
                    {
                        q_hash_index[m] = q_hash_index[m] * (-1);
                    }
                    k_ID[m] = q_hash_index[m];  // ID for comparison
                    q_hash_index[m] = q_hash_index[m] % TableSize;    // mod TableSize
                }
                int count[number_of_clusters];
                for(int m=0; m<number_of_clusters; m++)
                    count[m] = 0;
                int cntr = 0;
                while(1)
                {
                    for(int m=0; m<number_of_clusters; m++)    // for every query show the results
                    {
                        temp[g][m] = hash_tables[g][q_hash_index[m]];
                        while(hash_tables[g][q_hash_index[m]] != NULL)
                        {
                            if(k_ID[m] == hash_tables[g][q_hash_index[m]]->ID)  // compare the IDs
                            {
                                // calculate distance
                                int dist = 0;
                                for(int d=1; d<=dimension; d++)
                                {
                                    dist = dist + pow((centroid_index[m][d] - p[hash_tables[g][q_hash_index[m]]->item - 1][d]), 2);
                                }
                                dist = sqrt(dist);

                                // vectors in range r
                                if(dist < R)
                                {
                                    int j_dist = j[m];
                                    int flag = 0;
                                    for(int i=0; i<=j_dist; i++)
                                    {
                                        if(hash_tables[g][q_hash_index[m]]->item == radius[m][i])
                                        {
                                            flag = 1;
                                        }
                                    }
                                    if(flag == 0)
                                    {
                                        radius[m][j[m]] = hash_tables[g][q_hash_index[m]]->item;
                                        j[m]++;
                                        for(int i=0; i<number_of_clusters; i++)
                                        {
                                            if(i != m)
                                            {
                                                for(int k=0; k<j[i]; k++)
                                                {
                                                    if(hash_tables[g][q_hash_index[m]]->item == radius[i][k])
                                                    {
                                                        int sec_dist = 0;
                                                        for(int d=1; d<=dimension; d++)
                                                        {
                                                            sec_dist = sec_dist + pow((centroid_index[i][d] - p[hash_tables[g][q_hash_index[m]]->item - 1][d]), 2);
                                                        }
                                                        sec_dist = sqrt(sec_dist);
                                                        if(dist < sec_dist)
                                                        {
                                                            radius[i][k] = -1;
                                                        }
                                                        else
                                                        {
                                                            radius[m][j[m] - 1] = -1;
                                                            j[m]--;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            hash_tables[g][q_hash_index[m]] = hash_tables[g][q_hash_index[m]]->next;
                        }
                        hash_tables[g][q_hash_index[m]] = temp[g][m];
                        if(j_temp[m] == j[m])
                        {
                            count[m]++;
                        }
                        j_temp[m] = j[m];
                    }
                    for(int i=0; i<number_of_clusters; i++)
                    {
                        if(count[i] >= 3)
                        {
                            cntr++;
                        }
                    }
                    if(cntr > 0)
                        break;
                    R = R * 2;
                }
                for(int i=0; i<number_of_clusters; i++)
                {
                    for(int j=0; j<input_items_counter; j++)
                    {
                        if(radius[i][j] == -1)
                            continue;
                    }
                }

                // Fix the new centroids
                int sum;
                for(int i=0; i<number_of_clusters; i++)
                {
                    centroid_index[i][0] = i;
                    for(int z=1; z<dimension; z++)
                    {
                        sum = 0;
                        for(int k=0; k<j[i]; k++)
                        {
                            sum = sum + p[radius[i][k] - 1][z];
                        }
                        if(j[i] > 0)
                            centroid_index[i][z] = sum / j[i];
                    }
                }

                int counter = 0;
                for(int i=0; i<number_of_clusters; i++)
                {
                    for(int z=1; z<dimension; z++)
                    {
                        if(previous_centroid_index[i][z] != centroid_index[i][z])
                        {
                            counter++;
                        }
                        previous_centroid_index[i][z] = centroid_index[i][z];
                    }
                }

                if(counter == 0)
                {
                    printf("THE END\n");
                    break;
                }
            }

            int count = 0;
            int min = -1;
            for(int k=0; k<input_items_counter; k++)
            {
                for(int m=0; m<number_of_clusters; m++)
                {
                    for(int i=0; i<j[m]; i++)
                    {
                        if(p[k][0] == radius[m][i])
                            count++;
                    }
                }
                if(count == 0)
                {
                    min = min_distance_index(p, k, centroid_index, number_of_clusters, dimension);
                    radius[min][j[min] + 1] = p[k][0];
                    j[min]++;
                }
            }

            // clustering time
            gettimeofday(&stop, 0);
            long sec = stop.tv_sec - start.tv_sec;
            long mic_sec = stop.tv_usec - start.tv_usec;
            double clustering_time = sec + mic_sec*1e-6;

            if(complete == 0)
            {
                for(int m=0; m<number_of_clusters; m++)
                {
                    fprintf(output_file_ptr, "CLUSTER-%d ", m+1);
                    fprintf(output_file_ptr, "(centroid: ");
                    for(int i=0; i<dimension; i++)
                        fprintf(output_file_ptr, "%d ", centroid_index[m][i]);
                    fprintf(output_file_ptr, ")\n\n");
                }
                fprintf(output_file_ptr, "clustering time: %f\n\n", clustering_time);
            }
            else
            {
                for(int m=0; m<number_of_clusters; m++)
                {
                    fprintf(output_file_ptr, "CLUSTER-%d ", m+1);
                    fprintf(output_file_ptr, "(centroid: ");
                    for(int i=0; i<dimension; i++)
                        fprintf(output_file_ptr, "%d ", centroid_index[m][i]);
                    for(int i=0; i<j[m]; i++)
                        fprintf(output_file_ptr, ", %d", radius[m][i]);
                    fprintf(output_file_ptr, ")\n\n");
                }
            }
        }
    }










    return 0;
}