// Project: EEL6763 Final Project
// Author: Joel Alvarez, Chase Ruskin
// File: row_wise_v2_space.c
//
// Finds the longest common subsequence (LCS) from a file storing DNA data.
//
// This file is adapted from an existing implementation (https://github.com/RayhanShikder/lcs_parallel)
// in an attempt to improve its performance on UF's HiPerGator HPC computing platform.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"
#include "omp.h"
#include <time.h>
#include <stdint.h>
#include "lcs.h"

// Global variables
char *A_str;
char *B_str;
char *C_ustr; 
int *P_Matrix;
int *DP_Results;
int *dp_prev_row;


int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);

    int my_rank;
    int num_procs;
    int chunk_size_dp; 
    int result;

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if(argc <= 1) {
        printf("Error: No input file specified! Please specify the input file, and run again!\n");
        return 0;
    }

    int len_a, len_b, len_c;
    double start_time, stop_time;

    if(my_rank == CAPTAIN) {
        // print stats about resource utilization
        #pragma omp parallel
        {   
            #pragma omp single
            {
                printf("Loading DNA file \"%s\" on each of %d processes (%d threads per rank)...\n", argv[1], num_procs, omp_get_num_threads());
                if(USE_VERSION == 1) {
                    printf("Branching: enabled (version 1)\n");
                } else {
                    printf("Branching: disabled (version 2)\n");
                }

            }
        }
    }

    FILE *fp = fopen(argv[1], "r");
    if(fp == NULL) {
        printf("Error: Failed to open file \"%s\"\n", argv[1]);
        exit(101);
    }
    
    fscanf(fp, "%d %d %d", &len_a, &len_b, &len_c);

    // define the number of rows and columns in the P matrix
    int ROWS = len_c;
    int COLS = len_b+1;

    A_str = (char *)malloc((len_a+1) * sizeof(char *));
    B_str = (char *)malloc((len_b+1) * sizeof(char *));
    // the set of unique characters
    C_ustr = (char *)malloc((len_c+1) * sizeof(char *));

    fscanf(fp, "%s %s %s", A_str, B_str, C_ustr);

    if(my_rank == CAPTAIN) {
        printf("Length of string B: %zu \n", strlen(B_str));
        printf("Length of string C: %zu\n", strlen(C_ustr));
        printf("String C is: %s\n", C_ustr);
    }
    
    // partition the number of units among all processes evenly
    chunk_size_dp = get_computation_size(len_b+1, my_rank, num_procs);

    if(DEBUG > 0) {
        printf("P := Rank %d assigned %d chunks\n", my_rank, get_computation_size(len_c, my_rank, num_procs));
        printf("DP := Rank %d assigned %d chunks\n", my_rank, chunk_size_dp);
    }

    DP_Results = (int *)malloc((len_b+1) * sizeof(int));
    dp_prev_row = (int *)malloc((len_b+1) * sizeof(int));

    P_Matrix = (int *)malloc((ROWS*(COLS)) * sizeof(int));

    begin = calloc(1, sizeof(struct timespec));
    end = calloc(1, sizeof(struct timespec));

    // start timing immediately before distributing data
    *begin = now();

    calc_P_matrix_v2(P_Matrix, B_str, len_b, C_ustr, len_c, my_rank, num_procs);

    result = lcs_yang_v2(DP_Results, dp_prev_row, P_Matrix, A_str, B_str, C_ustr, len_a, len_b, len_c, my_rank, chunk_size_dp);
    
    // halt the timing
    *end = now();

    if(my_rank == CAPTAIN) {
        printf("lcs_yang_v2: %d\n", result);

        double exec_time = tdiff(*begin, *end);
        printf("Execution time: %lf\n", exec_time);
    }

    if(DEBUG > 0) {
        if(my_rank == CAPTAIN) {
            for(int i = 0; i < len_c*(len_b+1); i++) {
                printf("%d\t", P_Matrix[i]);
            }
            printf("\n");
        }
    }

    // deallocate pointers
    free(A_str);
    free(B_str);
    free(C_ustr);
    free(dp_prev_row);

    free(P_Matrix);
    free(DP_Results);

    free(begin);
    free(end);

    MPI_Finalize();
    return 0;
}
