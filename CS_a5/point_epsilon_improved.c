//compilation instructions/examples:
//gcc -fopenmp point_epsilon_starter.c -o point_epsilon_starter
//sometimes you need to link against the math library with -lm:
//gcc -fopenmp point_epsilon_starter.c -lm -o point_epsilon_starter

//math library needed for the square root

#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <omp.h>

//N is 100000 for the submission. However, you may use a smaller value of testing/debugging.
#define N 100000
//Do not change the seed, or your answer will not be correct
#define SEED 72


struct pointData{
double x;
double y;
int captured;
int median_id;
};



void generateDataset(struct pointData * data);
void mergeSort(struct pointData data[N], int l, int r);


int main(int argc, char *argv[])
{

    //Read epsilon distance from command line
    if (argc!=2)
    {
        printf("\nIncorrect number of input parameters. Please input an epsilon distance.\n");
        return 0;
    }

    char inputEpsilon[20];
    strcpy(inputEpsilon,argv[1]);
    double epsilon=atof(inputEpsilon);



    //generate dataset:
    struct pointData * data;
    data=(struct pointData*)malloc(sizeof(struct pointData)*N);
    printf("\nSize of dataset (MiB): %f\n",(2.0*sizeof(double)*N*1.0)/(1024.0*1024.0));
    generateDataset(data);


    //change OpenMP settings:
    omp_set_num_threads(8);


    double tstart=omp_get_wtime();

    int is_eps = 0;
    int median_table[N];
    int bound_median_list[N];
    int within_bound_count_uncap;
    int within_bound_count_total;
    int median_iter = -1;
    int bound_median_frequency;
    double u_bound;
    double d_bound;
    double l_bound;
    double r_bound;
    double current_left_y;
    double current_right_y;
    bool continue_left_search;
    bool continue_right_search;

    mergeSort(data, 0, N-1);
    for( int k = 0; k < N; k++ ) median_table[k] = 0;

    int i;
    int left_iter;
    int right_iter;

    for( i = 0; i < N; i++ )
        {
            if( data[i].median_id == -1 )
                {
                    median_iter++;
                    bound_median_frequency = 0;
                    within_bound_count_uncap = 1;
                    within_bound_count_total = 1;
                    u_bound = data[i].y + 5;
                    d_bound = data[i].y - 5;
                    l_bound = data[i].x - 5;
                    r_bound = data[i].x + 5;
                    data[i].median_id = median_iter;
                    left_iter = i - 1;
                    right_iter = i + 1;
                    continue_left_search = true;
                    continue_right_search = true;

                    while( continue_left_search || continue_right_search )
                        {

                            if( left_iter <= 0 )
                                {
                                    continue_left_search = false;
                                }
                            else
                                {
                                    if( data[left_iter].x <= l_bound )
                                        {
                                            continue_left_search = false;
                                        }
                                }

                            if( right_iter > N )
                                {
                                    continue_right_search = false;
                                }
                            else
                                {
                                    if( data[right_iter].x >= r_bound )
                                        {
                                            continue_right_search = false;
                                        }
                                }

                            if( continue_left_search )
                                {
                                    current_left_y = data[left_iter].y;
                                    if(
                                       current_left_y < u_bound &&
                                       current_left_y > d_bound )
                                        {
                                            if(data[left_iter].median_id == -1) within_bound_count_uncap++;
                                            else
                                                {
                                                    median_table[data[right_iter].median_id]++;
                                                    bound_median_list[bound_median_frequency] = data[right_iter].median_id;
                                                    bound_median_frequency++;
                                                }
                                            within_bound_count_total++;
                                            data[left_iter].median_id = median_iter;
                                        }
                                }

                            if( continue_right_search )
                                {
                                    current_right_y = data[right_iter].y;
                                    if(
                                       current_right_y < u_bound &&
                                       current_right_y > d_bound )
                                        {
                                            if(data[right_iter].median_id == -1) within_bound_count_uncap++;
                                            else
                                                {
                                                    median_table[data[right_iter].median_id]++;
                                                    bound_median_list[bound_median_frequency] = data[right_iter].median_id;
                                                    bound_median_frequency++;
                                                }
                                            within_bound_count_total++;
                                            data[right_iter].median_id = median_iter;
                                        }
                                }

                            left_iter--;
                            right_iter++;
                        }

                    int median_cluster;
                    for( int i = 0; i < N; i++ )
                        {
                            median_cluster = median_table[bound_median_list[i]];

                            is_eps += (within_bound_count_total-median_table[i])*median_table[i];

                            median_table[i] = 0;
                        }

                    is_eps += within_bound_count_uncap*(within_bound_count_uncap - 1);
                }

        }

    double tend=omp_get_wtime();

    printf("\nTotal within epsilon (s): %d",is_eps);
    printf("\nTotal time (s): %f",tend-tstart);


    free(data);
    printf("\n");
    return 0;
}


//Do not modify the dataset generator or you will get the wrong answer
void generateDataset(struct pointData * data)
{

    //seed RNG
    srand(SEED);


    for (unsigned int i=0; i<N; i++){
        data[i].x=1000.0*((double)(rand()) / RAND_MAX);
        data[i].y=1000.0*((double)(rand()) / RAND_MAX);
        data[i].captured = 0;
        data[i].median_id = -1;
    }


}

void merge(struct pointData data[N], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    /* create temp arrays */
    struct pointData L[n1], R[n2];

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = data[l + i];
    for (j = 0; j < n2; j++)
        R[j] = data[m + 1 + j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2) {
        if (L[i].x <= R[j].x) {
            data[k] = L[i];
            i++;
        }
        else {
            data[k] = R[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
    are any */
    while (i < n1) {
        data[k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there
    are any */
    while (j < n2) {
        data[k] = R[j];
        j++;
        k++;
    }
}

/* l is for left index and r is right index of the
sub-array of arr to be sorted */
void mergeSort(struct pointData data[N], int l, int r)
{
    if (l < r) {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l + (r - l) / 2;

        // Sort first and second halves
        mergeSort(data, l, m);
        mergeSort(data, m + 1, r);

        merge(data, l, m, r);
    }
}


