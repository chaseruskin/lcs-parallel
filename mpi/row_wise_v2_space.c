// Project: EEL6763 Final Project
// Author: Joel Alvarez, Chase Ruskin
// File: row_wise_v2_space.c
//
// Finds the longest common subsequence (LCS) from a file storing DNA data.
//
// This file is adapted from an existing repository (https://github.com/RayhanShikder/lcs_parallel).

#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include "mpi.h"
#include<time.h>
#include <stdint.h>
#include "lcs.h"


#define CAPTAIN 0

//global variables
char *string_A;
char *string_B;
char *unique_chars_C; //unique alphabets
int c_len;
int *P_Matrix;
int *DP_Results; //to store the DP values
int *dp_prev_row;


int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);

    int my_rank;
    int num_procs;
    // chunk sizes for DP matrix
    int chunk_size_dp; 
    int res;

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // grab this process's rank
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs); //grab the total num of processes

    if(argc <= 1) {
        printf("Error: No input file specified! Please specify the input file, and run again!\n");
        return 0;
    }

    int len_a, len_b;
    double start_time, stop_time;

    if(my_rank == CAPTAIN) {
        printf("\nYour input file is %s \n", argv[1]);
    }

    FILE *fp = fopen(argv[1], "r");
    if(fp == NULL) {
        printf("Error: Failed to open file \"%s\"\n", argv[1]);
        exit(101);
    }
    
    fscanf(fp, "%d %d %d", &len_a, &len_b, &c_len);

    string_A = (char *)malloc((len_a+1) * sizeof(char *));
    string_B = (char *)malloc((len_b+1) * sizeof(char *));
    unique_chars_C = (char *)malloc((c_len+1) * sizeof(char *));

    fscanf(fp, "%s %s %s", string_A, string_B, unique_chars_C);

    if(my_rank == CAPTAIN) {
        printf("Length of string B: %zu \nLength of string C: %zu\n", strlen(string_B), strlen(unique_chars_C));
        printf("String C is: %s\n",unique_chars_C);
    }

    chunk_size_dp = ((len_b+1)/num_procs);

    printf("P := Rank %d will get chunks %d\n", my_rank, get_computation_size(c_len, my_rank, num_procs));

    if(my_rank == CAPTAIN) {
        printf("chunk_dp: %d procs: %d\n", chunk_size_dp, num_procs);
    }

    DP_Results = (int *)malloc((len_b+1) * sizeof(int));
    dp_prev_row = (int *)malloc((len_b+1) * sizeof(int));

    P_Matrix = (int *)malloc((c_len*(len_b+1)) * sizeof(int));

    // start the timing
    start_time = MPI_Wtime();

    calc_P_matrix_v2(P_Matrix, string_B, len_b, unique_chars_C, c_len, my_rank, num_procs);

    res = lcs_yang_v2(DP_Results, dp_prev_row, P_Matrix,string_A,string_B,unique_chars_C,len_a,len_b,c_len,my_rank, chunk_size_dp);
    
    // halt the timing
    stop_time = MPI_Wtime();

    if(my_rank == CAPTAIN) {
        printf("lcs_yang_v2 is: %d\n", res);
        printf("Time taken for lcs_yang_v2 is: %lf\n", stop_time - start_time);
    }

    // if(my_rank == CAPTAIN) {
    //     for(int i = 0; i < c_len*(len_b+1); i++) {
    //         printf("%d\t", P_Matrix[i]);
    //     }
    //     printf("\n");
    // }


    //deallocate pointers
    free(P_Matrix);
    free(DP_Results);

    MPI_Finalize();
    return 0;
}
