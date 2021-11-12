#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <limits.h>

int dimension;
int input_items_counter,d_1;

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


unsigned concatenate(unsigned x, unsigned y) {
    unsigned pow = 10;
    while(y >= pow)
        pow *= 10;                      //concat 2 unsigned numbers

    
    return x * pow + y;        
}


int temp,flag=0,a;
int temp1=0;

int * h_p(int** f, int d_1,int input_items_counter ){

    int *p = malloc(sizeof(int)*input_items_counter);
   // int p[input_items_counter];
   // int *pf=p;

    for(long int i=0;i<input_items_counter;i++){
        for(int j=0;j<d_1;j++){
            if(flag==1){
                temp1++;
                a=concatenate(temp,f[i][j]);
                temp=a;
                if (temp1==d_1-1){
                    p[i]=temp;
                    temp1++;
                    flag=0;                     //concat d' (d_1) numbers 
                }                               // put them in table
            }else{
                temp=f[i][j];
            }
           
            if(temp1==d_1 ){
                temp1=0;
            }else{
                flag=1;
            }

         }
        

     }
    
    // for(int i=0;i<input_items_counter;i++)
    //     printf("%d ",p[i]);
    // printf("input=%d\n",input_items_counter); 


    printf("%p\n",p);
    return p;                                         //return table
}

int con(int x, int d_1){
    int digit;
    int i=0;
    int arr[d_1];
    int mynum=0;
    int temp=0;
    while (x!=0){
        digit=x % 10;
        arr[i]=digit;
        i++;                            
        x=x/10;
    }                                                   // converts a number from binary to decimal
    for(int j=0;j<i;j++)
    {
        temp=arr[j] * pow(2,j);
        mynum=mynum + temp;
    }

    return mynum;
}

int hammingDistance(int n1, int n2)
{
    int x = n1 ^ n2;
     int setBits = 0;

    while (x > 0)
    {                                   //calculate hamming distance between 2 integers
        setBits += x & 1;
        x >>= 1;
    }
 
    return setBits;
}

struct Hash_Node
{
    int item;
    int ID;
    struct Hash_Node* next;
};

int main(int argc, char* argv[])
{
    char* output_file = malloc(sizeof(char*) + 1);
    output_file = "out";


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
    //int input_items_counter = 0;
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
   // printf("dim1=%d\n",dimension);

 
 
    rewind(query_file_ptr);

    printf("query items=%d\n",query_items_counter);
    printf("input items=%d\n",input_items_counter);

    // calculate d'
    int d_1=log10(input_items_counter) -1;
    printf("d'=%d\n",d_1);
    int i = 0, j = 0;

    // create 2 tables with 2 dimension  and we put inside 0,1//
    int** f = (int**)malloc(input_items_counter * sizeof(int*));
    for(int i=0; i<input_items_counter; i++)
    {
        f[i] = (int*)malloc(sizeof(int) * d_1);
    }
    for (i=0;i<input_items_counter; i++){
        for (j=0;j<d_1;j++){
            f[i][j]=rand() % 2;
        }
    }
    
    int** f1 = (int**)malloc(query_items_counter * sizeof(int*));
    for(int i=0; i<query_items_counter; i++)
    {
        f1[i] = (int*)malloc(sizeof(int) * d_1);
    }
    for (i=0;i<query_items_counter; i++){
        for (j=0;j<d_1;j++){
            f1[i][j]=rand() % 2;
        }
    }

    // Work for input file
    int** p = malloc(sizeof(int*) * input_items_counter * sizeof(int*));    // array of the items of dataset
    for( i=0; i<input_items_counter; i++)
        p[i] = malloc(sizeof(int) * (dimension + 1));
     i=0;
     j=0;
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

    int k = d_1;


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

    // Same for query file
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

    

    printf("dimension=%d\n",dimension);

    int * a=h_p(f,d_1,input_items_counter);

   // Hash table for input file
    int TableSize = pow(2,d_1);
    int hash_index;
    int M=10;
    int probes =2;
    int L = 5;
    struct Hash_Node* hash_tables[input_items_counter];

    for(int i=0; i<(input_items_counter); i++)
    {
        hash_tables[i] = NULL;
    }

    int r[k];
    int ID;

    // we put them in hash table
    for(int i=0; i<input_items_counter; i++)
    {
        hash_index =a[i];
        ID=hash_index;
        hash_index= con(a[i],d_1);
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


    FILE *output_file_ptr;
    output_file_ptr = fopen(output_file, "w");    // open output file
    if(output_file_ptr == NULL)
    {
        perror("Error\n");
        exit(1);
    }

    int * a1=h_p(f1,d_1,query_items_counter);

       // fprintf(output_file_ptr, "Hash Table no%d\n", g);
        struct Hash_Node* temp;
        for(int m=0; m<query_items_counter; m++)    // for every query show the results
        {
            hash_index= a1[m];
            int k_ID=hash_index;
            hash_index= con(a1[m],d_1);
            int min_dist = INT_MAX;
            int nearest_neighbor = -1;
            temp = hash_tables[hash_index];
            int counter = 0;
            while(hash_tables[hash_index] != NULL)   // count the list
            {
                counter++;
                hash_tables[hash_index] = hash_tables[hash_index]->next;
            }
            int N = 2;
            int R = 200;
            int *k_min_dist=malloc(sizeof(int)*counter);
            int * k_nearest_neighbor=malloc(sizeof(int)*counter);
            i = 0, j = 0;
            hash_tables[hash_index] = temp;
            int *radius=malloc(sizeof(int)*counter);
            struct timeval start, stop;
            gettimeofday(&start, 0);

            for(int t=0;t<TableSize;t++){
                while(hash_tables[hash_index] != NULL)
                {
                        int hdi=hammingDistance(hash_index,t);
                    // printf("hdi=%d\n",hdi);
                        if(k_ID == hash_tables[hash_index]->ID || hdi<=2 )  // compare the IDs
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
                    //}
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
        }



    fclose(query_file_ptr);
    fclose(input_file_ptr);
    return 0;
}