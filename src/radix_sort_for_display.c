
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <omp.h>
#include <time.h>
#include <math.h>

void generate_number(int *arr, int n)
{
    int seed = 13516112 + 13516025;
    srand(seed);
    for (long i = 0; i < n; i++)
    {
        arr[i] = (int)rand();
    }
}

void countSort(int *unsorted, int *sorted, int n_element, int bit_block_size, int exp)
{
    int counter_size = (int)pow((double)2, (double)bit_block_size);

    int count[counter_size];
    int i;
    int number_end_with;

    // Initialize count array
    for (i = 0; i < counter_size; i++)
    {
        count[i] = 0;
    }

    // Store count of occurrences in count[]
    // #pragma omp parallel for private(number_end_with) reduction(+: count[:counter_size])
    for (i = 0; i < n_element; i++)
    {
        number_end_with = (unsorted[i] / exp) % counter_size;
        count[number_end_with]++;
    }

    // Cummulative count
    for (i = 1; i < counter_size; i++)
    {
        count[i] += count[i - 1];
    }

    // Build the output array
    for (i = n_element - 1; i >= 0; i--)
    {
        sorted[count[(unsorted[i] / exp) % counter_size] - 1] = unsorted[i];
        count[(unsorted[i] / exp) % counter_size]--;
    }
}

void swap(int **a, int **b)
{
    int *temp = *a;
    *a = *b;
    *b = temp;
}

void radix_solve(int *unsorted, int *sorted, int n_element, int BIT_BLOCK_SIZE)
{

    // int BIT_BLOCK_SIZE = 8; // pakai 4 core
    int exp = 1;

    // Do counting sort for every digit.
    for (int i = 0; i < (32 / BIT_BLOCK_SIZE); i++)
    {
        exp *= pow(2, BIT_BLOCK_SIZE);

        countSort(unsorted, sorted, n_element, BIT_BLOCK_SIZE, exp);
        swap(&unsorted, &sorted);
    }
}

double get_elapsed_time(struct timespec start, struct timespec end)
{

    long seconds = (end.tv_sec - start.tv_sec);
    long nanosecs = (end.tv_nsec - start.tv_nsec);
    double microsecs = seconds * 1.0e6 + nanosecs * 1.0e-3;

    return microsecs;
}

void display_output(int *sorted, int head_count)
{
    for (int i = 0; i < head_count; i++)
    {
        printf("%d\n", sorted[i]);
    }
}

int main(int argc, char *argv[])
{

    int NUM_OF_ELEMENT;
    int *unsorted;
    int *sorted;
    double elapsed_time = 0;
    double total_elapsed_time = 0;

    int repeat = 10;
    struct timespec start, end;

    int BIT_BLOCK_SIZE = 2;

    if (argc <= 1)
    {
        return -1;
    }

    NUM_OF_ELEMENT = atoi(argv[1]);
    unsorted = malloc(sizeof(int) * NUM_OF_ELEMENT);
    sorted = malloc(sizeof(int) * NUM_OF_ELEMENT);

    for (int j = 0; j < 5; j++){

        for (int i = 0; i < repeat; i++)
        {

            printf("Sorting for %d", NUM_OF_ELEMENT);
            generate_number(unsorted, NUM_OF_ELEMENT);

            // ignore CLOCK_MONOTONIC warning
            clock_gettime(CLOCK_MONOTONIC, &start);
            radix_solve(unsorted, sorted, NUM_OF_ELEMENT, BIT_BLOCK_SIZE);
            clock_gettime(CLOCK_MONOTONIC, &end);

            // printf("============ sorted ============\n");
            // swap(&unsorted, &sorted);
            // display_output(sorted, NUM_OF_ELEMENT);

            elapsed_time = get_elapsed_time(start, end);
            total_elapsed_time += elapsed_time;
            printf("\nTime elapsed (microsec) iter %d : %lg block %d", i, elapsed_time, BIT_BLOCK_SIZE);

        }
        printf("\nTime average elapsed (microsec) : %lg block %d\n", total_elapsed_time / repeat, BIT_BLOCK_SIZE);
        printf("\n\n\n");
        
        BIT_BLOCK_SIZE*=2;
        elapsed_time = 0;
        total_elapsed_time = 0;
    }
    return 0;
}
