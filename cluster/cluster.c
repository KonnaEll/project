#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <limits.h>
#include "lsh_funcs.h"
#include "cube_funcs.h"
#include "cluster_funcs.h"


int main(int argc, char* argv[])
{
	// parameters check
    char* input_file = malloc(sizeof(char*) + 1);
    char* configuration_file = malloc(sizeof(char*) + 1);
    char* output_file = malloc(sizeof(char*) + 1);
    char* method = malloc(sizeof(char*) + 1);
    int complete = 0;
	if(argc == 9)   // without complete
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
    else if(argc == 10) // with complete
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
    conf_file_ptr = fopen(configuration_file, "r");    // open configuration file
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
    while(fscanf(conf_file_ptr, "%s", numb) != EOF)     // save the contents of the file in variables
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
    // array with number of clusters lines and dimension columns
    int** centroid_index = malloc(sizeof(int*) * number_of_clusters);
    for(int i=0; i<number_of_clusters; i++)
    {
        centroid_index[i] = malloc(sizeof(int) * (dimension + 1));
    }
    int item_id = rand() % input_items_counter; // first random centroid
    centroid_index[0][0] = item_id;
    for(int d=1; d<=dimension; d++)
    {
        centroid_index[0][d] = p[item_id - 1][d];   // put the dimensions
    }
    int* D = malloc(input_items_counter * sizeof(int));
    int index;
	for(int i=1; i<number_of_clusters; i++)
    {
		for(int j=0; j<input_items_counter; j++)
        {
			D[j] = min_distance(p, j, centroid_index, i, dimension);    // min distance from the centroids
		}

        index = max_distance(D, input_items_counter, p);    // max distance of the min distances above
        centroid_index[i][0] = index;

        for(int d=1; d<=dimension; d++)
        {
            centroid_index[i][d] = p[index - 1][d];
        }
	}
    free(D);

    if(strcmp(method, "Classic") == 0)
    {
        // Lloyd's algorithm
        int* cluster_per_item = malloc(sizeof(int) * input_items_counter);
        int** previous_centroid_index = malloc(sizeof(int*) * number_of_clusters);
        for(int i=0; i<number_of_clusters; i++)
        {
            previous_centroid_index[i] = malloc(sizeof(int) * (dimension + 1));     // save the centroids
        }
        for(int i=0; i<number_of_clusters; i++)
        {
            for(int z=1; z<dimension; z++)
            {
                previous_centroid_index[i][z] = -1;
            }
        }
        int count;
        struct timeval start, stop;
        gettimeofday(&start, 0);
        while(1)
        {
            for(int j=0; j<input_items_counter; j++)    // each item has its centroid
            {
                cluster_per_item[j] = min_distance_index(p, j, centroid_index, number_of_clusters, dimension);
            }

            // Fix the new centroids
            int sum;
            for(int i=0; i<number_of_clusters; i++)
            {
                centroid_index[i][0] = i;
                for(int z=1; z<dimension; z++)
                {
                    count = 0;
                    sum = 0;
                    for(int j=0; j<input_items_counter; j++)
                    {
                        if(cluster_per_item[j] == i)    // find the right centroid
                        {
                            sum = sum + p[j][z];
                            count++;
                        }
                    }
                    centroid_index[i][z] = sum / count;     // average
                }
            }

            count = 0;
            for(int i=0; i<number_of_clusters; i++)     // if two centroids are the same for 2 rounds
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
            if(count == 0)  // then break
            {
                break;
            }
        }
        gettimeofday(&stop, 0);
        long sec = stop.tv_sec - start.tv_sec;
        long mic_sec = stop.tv_usec - start.tv_usec;
        double clustering_time = sec + mic_sec*1e-6;

        int j[number_of_clusters];
        for(int i=0; i<number_of_clusters; i++)
            j[i] = 0;

        int** radius = malloc(sizeof(int*) * number_of_clusters);    // array for vectors within radius
        for(int i=0; i<number_of_clusters; i++)
            radius[i] = malloc(sizeof(int) * input_items_counter);
        for(int i=0; i<number_of_clusters; i++)
            for(int j=0; j<input_items_counter; j++)
                radius[i][j] = -1;
        
        for(int i=0; i<number_of_clusters; i++) // fill with the items
        {
            for(int k=0; k<input_items_counter; k++)
            {
                if(cluster_per_item[k] == i)
                {
                    radius[i][j[i]] = k + 1;
                    j[i]++;
                }
            }
        }

        if(complete == 0)
        {
            fprintf(output_file_ptr, "Algorithm: Lloyd's\n");   // prints
            for(int m=0; m<number_of_clusters; m++)
            {
                fprintf(output_file_ptr, "CLUSTER-%d ", m+1);
                fprintf(output_file_ptr, "(centroid: ");
                for(int i=0; i<dimension; i++)
                    fprintf(output_file_ptr, "%d ", centroid_index[m][i]);
                fprintf(output_file_ptr, ")\n\n");
            }
            fprintf(output_file_ptr, "clustering time: %f\n\n", clustering_time);

            // Silhouette
            int dist;
            int min;
            int min_ind[number_of_clusters];
            for(int i=0; i<number_of_clusters; i++)     // which is the closest centroid to each centroid
            {
                min = INT_MAX;
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
                            min_ind[i] = j;
                        }
                    }
                }
            }

            int clust, a_i, b;
            int sum = 0;
            int s[input_items_counter];
            for(int i=0; i<input_items_counter; i++)
            {
                clust = cluster_per_item[i];
                for(int k=0; k<j[clust]; k++)
                {
                    dist = 0;
                    for(int d=1; d<=dimension; d++)
                    {
                        dist = dist + pow((p[i][d] - p[radius[clust][k] - 1][d]), 2);   // distance for every item
                    }
                    dist = sqrt(dist);
                    sum = sum + dist;
                }
                a_i = sum / j[clust];   // average
                sum = 0;
                clust = min_ind[clust];     // closest centroid
                for(int k=0; k<j[clust]; k++)
                {
                    dist = 0;
                    for(int d=1; d<=dimension; d++)
                    {
                        dist = dist + pow((p[i][d] - p[radius[clust][k] - 1][d]), 2);
                    }
                    dist = sqrt(dist);
                    sum = sum + dist;
                }
                b = sum / j[clust]; // average
                sum = 0;

                if(a_i > b)
                    s[i] = (b / a_i) -1;
                else if(b > a_i)
                    s[i] = 1 - (a_i / b);
                else
                    s[i] = 0;
            }

            float average_per_cluster[number_of_clusters];  // average for each cluster
            for(int i=0; i<number_of_clusters; i++)
                average_per_cluster[i] = 0;
            for(int i=0; i<input_items_counter; i++)
            {
                for(int m=0; m<number_of_clusters; m++)
                {
                    if(cluster_per_item[i] == m)
                    {
                        average_per_cluster[m] = average_per_cluster[m] + s[i];
                        continue;
                    }
                }
            }

            fprintf(output_file_ptr, "Silhouette: [");
            for(int m=0; m<number_of_clusters; m++)
            {
                fprintf(output_file_ptr, "%f, ", average_per_cluster[m] / j[m]);
            }

            float total_average_sil = 0;
            for(int i=0; i<input_items_counter; i++)
                total_average_sil = total_average_sil + s[i];
            total_average_sil = total_average_sil / input_items_counter;
            fprintf(output_file_ptr, "%f]", total_average_sil);
        }
        else
        {
            for(int m=0; m<number_of_clusters; m++)
            {
                fprintf(output_file_ptr, "CLUSTER-%d ", m+1);
                fprintf(output_file_ptr, "{centroid: ");
                for(int i=0; i<dimension; i++)
                    fprintf(output_file_ptr, "%d ", centroid_index[m][i]);
                for(int i=0; i<j[m]; i++)
                    fprintf(output_file_ptr, ", %d", radius[m][i]);
                fprintf(output_file_ptr, "}\n\n");
            }
        }

        // free memory
        free(cluster_per_item);
        for(int i=0; i<number_of_clusters; i++)
        {
            free(previous_centroid_index[i]);
            free(radius[i]);
        }
        free(previous_centroid_index);
        free(radius);

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
        int TableSize = input_items_counter / 8;
        int M = (int)pow(2, 32) - 5;
        struct Hash_Node* hash_tables[number_of_vector_hash_tables][TableSize];
        for(int n=0; n<number_of_vector_hash_tables; n++)
        {
            for(int i=0; i<(TableSize); i++)
            {
                hash_tables[n][i] = NULL;
            }
        }

        // same way as lsh
        float** h_q_result = malloc(sizeof(float*) * number_of_clusters); // array with the results of the h function
        for(int i=0; i<number_of_clusters; i++)
            h_q_result[i] = malloc(sizeof(float) * number_of_vector_hash_functions);
        
        int** radius = malloc(sizeof(int*) * number_of_clusters);    // array for vectors within radius
        for(int i=0; i<number_of_clusters; i++)
            radius[i] = malloc(sizeof(int) * input_items_counter);
        
        int** previous_centroid_index = malloc(sizeof(int*) * number_of_clusters);  // save the centroids
        for(int i=0; i<number_of_clusters; i++)
            previous_centroid_index[i] = malloc(sizeof(int) * (dimension + 1));
        int r[number_of_vector_hash_tables][number_of_vector_hash_functions];
        int ID;
        for(int g=0; g<number_of_vector_hash_tables; g++)
        {
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

            for(int i=0; i<number_of_clusters; i++)
            {
                for(int z=1; z<dimension; z++)
                {
                    previous_centroid_index[i][z] = -1;
                }
            }

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
                for(int m=0; m<number_of_clusters; m++)    // for every cluster find the right bucket
                {
                    q_hash_index[m] = 0;
                    for(int j=0; j<number_of_vector_hash_functions; j++)
                    {
                        q_hash_index[m] = q_hash_index[m] + (int)h_q_result[m][j] * r[g][j]; // g function
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
                    for(int m=0; m<number_of_clusters; m++)
                    {
                        temp[g][m] = hash_tables[g][q_hash_index[m]];
                        while(hash_tables[g][q_hash_index[m]] != NULL)  // in the list
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
                                        if(hash_tables[g][q_hash_index[m]]->item == radius[m][i])   // not same items in the list
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
                                                    // item in another cluster
                                                    if(hash_tables[g][q_hash_index[m]]->item == radius[i][k])
                                                    {
                                                        int sec_dist = 0;
                                                        for(int d=1; d<=dimension; d++) // calculate distance of the other
                                                        {
                                                            sec_dist = sec_dist + pow((centroid_index[i][d] - p[hash_tables[g][q_hash_index[m]]->item - 1][d]), 2);
                                                        }
                                                        sec_dist = sqrt(sec_dist);
                                                        if(dist < sec_dist) // closer cluster
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
                        if(j_temp[m] == j[m])   // if no item gets in any cluster
                        {
                            count[m]++;
                        }
                        j_temp[m] = j[m];
                    }
                    for(int i=0; i<number_of_clusters; i++)
                    {
                        if(count[i] >= 3)   // for 3 times
                        {
                            cntr++;
                        }
                    }
                    if(cntr > 0)    // then break
                        break;
                    R = R * 2;  // or double the radius and continue
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
                            sum = sum + p[radius[i][k] - 1][z]; // average
                        }
                        if(j[i] > 0)
                            centroid_index[i][z] = sum / j[i];
                    }
                }

                int counter = 0;
                for(int i=0; i<number_of_clusters; i++)     // centroids are the same for two rounds
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

                if(counter == 0)    // then stop
                {
                    break;
                }
            }

            int count = 0;
            int min = -1;
            int centroids[input_items_counter]; // cluster of each item
            for(int k=0; k<input_items_counter; k++)
            {
                for(int m=0; m<number_of_clusters; m++)
                {
                    for(int i=0; i<j[m]; i++)
                    {
                        if(p[k][0] == radius[m][i])
                        {
                            centroids[k] = m;
                            count++;
                        }
                    }
                }
                if(count == 0)
                {
                    // whoever is left behind then find the closer cluster and put it in there
                    min = min_distance_index(p, k, centroid_index, number_of_clusters, dimension);
                    radius[min][j[min]] = p[k][0];
                    j[min]++;
                    centroids[k] = min;
                }
                count = 0;
            }

            // Clustering time
            gettimeofday(&stop, 0);
            long sec = stop.tv_sec - start.tv_sec;
            long mic_sec = stop.tv_usec - start.tv_usec;
            double clustering_time = sec + mic_sec*1e-6;

            if(complete != 0)
            {
                for(int m=0; m<number_of_clusters; m++)
                {
                    fprintf(output_file_ptr, "CLUSTER-%d ", m+1);
                    fprintf(output_file_ptr, "{centroid: ");
                    for(int i=0; i<dimension; i++)
                        fprintf(output_file_ptr, "%d ", centroid_index[m][i]);
                    for(int i=0; i<j[m]; i++)
                        fprintf(output_file_ptr, ", %d", radius[m][i]);
                    fprintf(output_file_ptr, "}\n\n");
                }
            }
            else
            {
                for(int m=0; m<number_of_clusters; m++)
                {
                    fprintf(output_file_ptr, "CLUSTER-%d ", m+1);
                    fprintf(output_file_ptr, "{size: %d, ", j[m]);
                    fprintf(output_file_ptr, "centroid: ");
                    for(int i=0; i<dimension; i++)
                        fprintf(output_file_ptr, "%d ", centroid_index[m][i]);
                    fprintf(output_file_ptr, "}\n\n");
                }
                fprintf(output_file_ptr, "clustering time: %f\n\n", clustering_time);
        
                // Silhouette
                int dist;
                int min_ind[number_of_clusters];
                for(int i=0; i<number_of_clusters; i++)     // which is the closest centroid to each centroid
                {
                    min = INT_MAX;
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
                                min_ind[i] = j;
                            }
                        }
                    }
                }

                int clust, a_i, b;
                int sum = 0;
                int s[input_items_counter];
                for(int i=0; i<input_items_counter; i++)
                {
                    clust = centroids[i];
                    for(int k=0; k<j[clust]; k++)
                    {
                        dist = 0;
                        for(int d=1; d<=dimension; d++)
                        {
                            dist = dist + pow((p[i][d] - p[radius[clust][k] - 1][d]), 2);   // distance for every item
                        }
                        dist = sqrt(dist);
                        sum = sum + dist;
                    }
                    a_i = sum / j[clust];   // average
                    sum = 0;

                    clust = min_ind[clust];     // closest centroid
                    for(int k=0; k<j[clust]; k++)
                    {
                        dist = 0;
                        for(int d=1; d<=dimension; d++)
                        {
                            dist = dist + pow((p[i][d] - p[radius[clust][k] - 1][d]), 2);
                        }
                        dist = sqrt(dist);
                        sum = sum + dist;
                    }
                    b = sum / j[clust]; // average
                    sum = 0;

                    if(a_i > b)
                        s[i] = (b / a_i) -1;
                    else if(b > a_i)
                        s[i] = 1 - (a_i / b);
                    else
                        s[i] = 0;
                }

                float average_per_cluster[number_of_clusters];  // average for each cluster
                for(int i=0; i<number_of_clusters; i++)
                    average_per_cluster[i] = 0;
                for(int i=0; i<input_items_counter; i++)
                {
                    for(int m=0; m<number_of_clusters; m++)
                    {
                        if(centroids[i] == m)
                        {
                            average_per_cluster[m] = average_per_cluster[m] + s[i];
                            continue;
                        }
                    }
                }

                fprintf(output_file_ptr, "Silhouette: [");
                for(int m=0; m<number_of_clusters; m++)
                {
                    fprintf(output_file_ptr, "%f, ", average_per_cluster[m] / j[m]);
                }

                float total_average_sil = 0;
                for(int i=0; i<input_items_counter; i++)
                    total_average_sil = total_average_sil + s[i];
                total_average_sil = total_average_sil / input_items_counter;
                fprintf(output_file_ptr, "%f]\n", total_average_sil);
            }
        }

        // free memory
        for(int i=0; i<input_items_counter; i++)
        {
            free(h_p_result[i]);
        }
        free(h_p_result);
        for(int i=0; i<number_of_clusters; i++)
        {
            free(previous_centroid_index[i]);
            free(radius[i]);
            free(h_q_result[i]);
        }
        free(previous_centroid_index);
        free(radius);
        free(h_q_result);
    }
    else if(strcmp(method, "Hypercube") == 0)
    {
        int k = number_of_hypercube_dimensions;
        // create 2 tables with 2 dimension and we put inside 0,1//
        int** f = (int**)malloc(input_items_counter * sizeof(int*));
        for(int i=0; i<input_items_counter; i++)
        {
            f[i] = (int*)malloc(sizeof(int) * k);
        }
        for (i=0;i<input_items_counter; i++){
            for (j=0;j<k;j++){
                f[i][j]=rand() % 2;
            }
        }

        int** f1 = (int**)malloc(number_of_clusters * sizeof(int*));
        for(int i=0; i<number_of_clusters; i++)
        {
            f1[i] = (int*)malloc(sizeof(int) * k);
        }
        for (i=0;i<number_of_clusters; i++){
            for (j=0;j<k;j++){
                f1[i][j]=rand() % 2;
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

        // Hash table for input file same as before
        int TableSize = pow(2,k);
        int hash_index;
        int M=max_number_M_hypercube;
        int probes=number_of_probes;
        struct Hash_Node* hash_tables[input_items_counter];

        for(int i=0; i<(input_items_counter); i++)
        {
            hash_tables[i] = NULL;
        }

        int ID;
        struct timeval start, stop;
        gettimeofday(&start, 0);
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

        int** previous_centroid_index = malloc(sizeof(int*) * number_of_clusters);
        for(int i=0; i<number_of_clusters; i++)
        {
            previous_centroid_index[i] = malloc(sizeof(int) * (dimension + 1));
        }
        for(int i=0; i<number_of_clusters; i++)
        {
            for(int z=1; z<dimension; z++)
            {
                previous_centroid_index[i][z] = -1;
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

        float** h_q_result = malloc(sizeof(float*) * number_of_clusters); // array with the results of the h function
        for(int i=0; i<number_of_clusters; i++)
            h_q_result[i] = malloc(sizeof(float) * k);
        while(1)
        {
            for(int i=0; i<number_of_clusters; i++)
            {
                srand(time(0));
                for(int j=0; j<k; j++)
                {
                    h_q_result[i][j] = h_function(centroid_index, i, dimension);   // h_function
                    for(int a=0;a<i;a++){
                        for(int b=0;b<j;b++){
                            if(h_q_result[i][j] == h_q_result[a][b]){   //if we find the same result we put the number 0 in f1 table//
                                f1[i][j]=0;
                            }
                        }
                    }
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
            int * a1=h_p(f1,k,number_of_clusters);
            for(int m=0; m<number_of_clusters; m++)    // for every query show the results
            {
                q_hash_index[m]=a1[m];
                k_ID[m]=q_hash_index[m];
                q_hash_index[m]=con(a1[m],k);
            }                
            int count[number_of_clusters];
            for(int m=0; m<number_of_clusters; m++)
                count[m] = 0;
            int cntr = 0;
            int cp;
            // exactly the same as lsh before but with the cube algorithm
            while(1)
            {
                for(int m=0; m<number_of_clusters; m++)    // for every query show the results
                {
                    for(int t=0;t<TableSize;t++){
                    while(hash_tables[q_hash_index[m]] != NULL)
                    {
                        cp=0;
                        int hdi=hammingDistance(hash_index,t);
                        if((k_ID[m] == hash_tables[q_hash_index[m]]->ID || hdi<=probes) && cp<=M)  // compare the IDs
                        {
                            // calculate distance
                            int dist = 0;
                            for(int d=1; d<=dimension; d++)
                            {
                                dist = dist + pow((centroid_index[m][d] - p[hash_tables[q_hash_index[m]]->item - 1][d]), 2);
                            }
                            dist = sqrt(dist);
                            
                            // vectors in range r
                            if(dist < R)
                            {
                                int j_dist = j[m];
                                int flag = 0;
                                for(int i=0; i<=j_dist; i++)
                                {
                                    if(hash_tables[q_hash_index[m]]->item == radius[m][i])
                                    {
                                        flag = 1;
                                    }
                                }
                                if(flag == 0)
                                {
                                    radius[m][j[m]] = hash_tables[q_hash_index[m]]->item;
                                    j[m]++;
                                    for(int i=0; i<number_of_clusters; i++)
                                    {
                                        if(i != m)
                                        {
                                            for(int k=0; k<j[i]; k++)
                                            {
                                                if(hash_tables[q_hash_index[m]]->item == radius[i][k])
                                                {
                                                    int sec_dist = 0;
                                                    for(int d=1; d<=dimension; d++)
                                                    {
                                                        sec_dist = sec_dist + pow((centroid_index[i][d] - p[hash_tables[q_hash_index[m]]->item - 1][d]), 2);
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
                        hash_tables[q_hash_index[m]] = hash_tables[q_hash_index[m]]->next;
                    }
                    }
                    if(j_temp[m] == j[m])   // when to break
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
                break;
            }
        }

        int count = 0;
        int min = -1;
        int centroids[input_items_counter];
        for(int k=0; k<input_items_counter; k++)
        {
            for(int m=0; m<number_of_clusters; m++)
            {
                for(int i=0; i<j[m]; i++)
                {
                    if(p[k][0] == radius[m][i])
                    {
                        centroids[k] = m;
                        count++;
                    }
                }
            }
            if(count == 0)
            {
                min = min_distance_index(p, k, centroid_index, number_of_clusters, dimension);  // what is left
                radius[min][j[min]] = p[k][0];
                j[min]++;
                centroids[k] = min;
            }
            count = 0;
        }

        // Clustering time
        gettimeofday(&stop, 0);
        long sec = stop.tv_sec - start.tv_sec;
        long mic_sec = stop.tv_usec - start.tv_usec;
        double clustering_time = sec + mic_sec*1e-6;

        if(complete != 0)
        {   for(int m=0; m<number_of_clusters; m++)
            {
                fprintf(output_file_ptr, "CLUSTER-%d ", m+1);
                fprintf(output_file_ptr, "size: %d, ", j[m]);
                fprintf(output_file_ptr, "centroid: ");
                for(int i=0; i<dimension; i++)
                    fprintf(output_file_ptr, "%d ", centroid_index[m][i]);
                for(int i=0; i<j[m]; i++)
                    fprintf(output_file_ptr, ", %d", radius[m][i]);
                fprintf(output_file_ptr, ")\n\n");
            }
        }
        else
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
        
            // Silhouette is exactly the same
            int dist;
            int min_ind[number_of_clusters];
            for(int i=0; i<number_of_clusters; i++)     // which is the closest centroid to each centroid
            {
                min = INT_MAX;
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
                            min_ind[i] = j;
                        }
                    }
                }
            }

            int clust, a_i, b;
            int sum = 0;
            int s[input_items_counter];
            for(int i=0; i<input_items_counter; i++)
            {
                clust = centroids[i];
                for(int k=0; k<j[clust]; k++)
                {
                    dist = 0;
                    for(int d=1; d<=dimension; d++)
                    {
                        dist = dist + pow((p[i][d] - p[radius[clust][k] - 1][d]), 2);
                    }
                    dist = sqrt(dist);
                    sum = sum + dist;
                }
                a_i = sum / j[clust];
                sum = 0;

                clust = min_ind[clust];     // closest centroid
                for(int k=0; k<j[clust]; k++)
                {
                    dist = 0;
                    for(int d=1; d<=dimension; d++)
                    {
                        dist = dist + pow((p[i][d] - p[radius[clust][k] - 1][d]), 2);
                    }
                    dist = sqrt(dist);
                    sum = sum + dist;
                }
                b = sum / j[clust];
                sum = 0;

                if(a_i > b)
                    s[i] = (b / a_i) -1;
                else if(b > a_i)
                    s[i] = 1 - (a_i / b);
                else
                    s[i] = 0;
            }

            float average_per_cluster[number_of_clusters];
            for(int i=0; i<number_of_clusters; i++)
                average_per_cluster[i] = 0;
            for(int i=0; i<input_items_counter; i++)
            {
                for(int m=0; m<number_of_clusters; m++)
                {
                    if(centroids[i] == m)
                    {
                        average_per_cluster[m] = average_per_cluster[m] + s[i];
                        continue;
                    }
                }
            }

            fprintf(output_file_ptr, "Silhouette: [");
            for(int m=0; m<number_of_clusters; m++)
            {
                fprintf(output_file_ptr, "%f, ", average_per_cluster[m] / j[m]);
            }

            float total_average_sil = 0;
            for(int i=0; i<input_items_counter; i++)
                total_average_sil = total_average_sil + s[i];
            total_average_sil = total_average_sil / input_items_counter;
            fprintf(output_file_ptr, "%f]", total_average_sil);
        }
        
        // free memory
        for(int i=0; i<input_items_counter; i++)
        {
            free(h_p_result[i]);
            free(f[i]);
        }
        free(h_p_result);
        free(f);
        for(int i=0; i<number_of_clusters; i++)
        {
            free(previous_centroid_index[i]);
            free(radius[i]);
            free(h_q_result[i]);
            free(f1[i]);
        }
        free(previous_centroid_index);
        free(radius);
        free(h_q_result);
        free(f1);
    }




    return 0;
}