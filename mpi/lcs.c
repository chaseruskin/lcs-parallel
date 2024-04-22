// Project: EEL6763 Final Project
// Author: Joel Alvarez, Chase Ruskin
// File: lcs.c
//
// Finds the longest common subsequence (LCS) from a file storing DNA data.
//
// This file is adapted from an existing implementation (https://github.com/RayhanShikder/lcs_parallel)
// in an attempt to improve its performance on UF's HiPerGator HPC computing platform.

#include "lcs.h"


double tdiff(struct timespec a, struct timespec b) {
    double dt = (( b.tv_sec - a.tv_sec ) + ( b.tv_nsec - a.tv_nsec ) / 1E9);
    return dt;
}


struct timespec now() {
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return t;
}


int get_computation_size(int N, int rank, int size) {
    uint64_t units_per_rank = (uint64_t)(N) / (uint64_t)(size);
    uint32_t ranks_with_plus_one = (uint32_t)((uint64_t)(N) % (uint64_t)(size));

    int comp_size = (int)(units_per_rank);
    // only give potential +1 to teammates
    if(rank < ranks_with_plus_one) { 
        comp_size += 1; 
    }
    return comp_size;
}


int get_index_of_character(char *str, char x, int len) {
    for(int i=0; i < len; i++) {
        if(str[i] == x) {
            return i;
        }
    }
    // not found the character x in str
    return -1;
}


void print_matrix(int **x, int row, int col) {
    for(int i=0; i<row; i++) {
        for(int j=0; j<col; j++) {
            printf("%d ",x[i][j]);
        }
        printf("\n");
    }
}


void calc_P_matrix_v2(int *p_global, char *b, int len_b, char *c, int len_c, int rank, int num_ranks) {

    if(PROFILE > 0) {
        *begin = now();
    }
    // determine number of elements that each rank will receive
    int *counts_c = calloc(num_ranks, sizeof(int));
    int *counts_p = calloc(num_ranks, sizeof(int));
    // determine offset from source data from which to take outgoing data to i'th rank
    int *displs_c = calloc(num_ranks, sizeof(int));
    int *displs_p = calloc(num_ranks, sizeof(int));

    // determine number of chunks that this rank must compute
    int chunk_size = get_computation_size(len_c, rank, num_ranks);

    uint32_t acc_c = 0;
    uint32_t acc_p = 0;
    for(int i = 0; i < num_ranks; i++) {
        // send two additional rows for neighboring
        counts_c[i] = get_computation_size(len_c, i, num_ranks);
        counts_p[i] = counts_c[i] * (len_b + 1);
        // compute displacement (count how many rows are already processed * elements per row)
        // provide an upper row for neighboring filter computation
        displs_c[i] = acc_c;
        displs_p[i] = acc_p;

        acc_c += counts_c[i];
        acc_p += counts_p[i];
    }

    int *p_local = calloc(chunk_size * (len_b+1), sizeof(int));

    // do somep computations for P
    if(chunk_size > 0) {
        // compute the partition of the P matrix
        for(int i=0; i < chunk_size; i++) {
            for(int j=1; j < len_b+1; j++) {
                if(b[j-1] == c[displs_c[rank]+i]) {
                    p_local[(i*(len_b+1))+j] = j;
                } else {
                    p_local[(i*(len_b+1))+j] = p_local[(i*(len_b+1))+j-1];
                }
            }
        }
    }
    if(PROFILE > 0) {
        double exec_time = tdiff(*begin, now());
        if(chunk_size > 0) {
            printf("Rank %d P matrix computation time: %f\n", rank, exec_time);
        }
        *begin = now();
    }
    // gather all the calculated values of P matrix in process 0 and redistribute
    MPI_Gatherv(p_local, chunk_size*(len_b+1), MPI_INT, p_global, counts_p, displs_p, MPI_INT, CAPTAIN, MPI_COMM_WORLD);
    if(PROFILE > 0) {
        double exec_time = tdiff(*begin, now());
        if(rank == CAPTAIN) {
            printf("Rank %d P matrix mpi gather time: %f\n", rank, exec_time);
        }
    }
}


void sync_dp(int *DP, int *dp_i_recv, int chunk_size) {
    MPI_Allgather(dp_i_recv, chunk_size, MPI_INT, DP, chunk_size, MPI_INT, MPI_COMM_WORLD);
}


void distribute_p(int *P, int count, int rank) {
    if(PROFILE > 0) {
        *begin = now();
    }
    MPI_Bcast(P, count, MPI_INT, CAPTAIN, MPI_COMM_WORLD);
    if(PROFILE > 0) {
        double exec_time = tdiff(*begin, now());
        printf("Rank %d P matrix mpi broadcast time: %f\n", rank, exec_time);
    }
}


int lcs_yang_v2(int *DP, int *prev_row, int *P, char *A, char *B, char *C, int len_a, int len_b, int len_c, int rank, int units_per_self) {
    // distribute the P matrix to all processes
    distribute_p(P, (len_c*(len_b+1)), rank);

    int m = len_a;
    int n = len_b;
    int u = len_c;

    int start_id = (rank * units_per_self);
    int end_id = (rank * units_per_self) + units_per_self;
    
    for(int i=1; i < m+1; i++) {
        if(PROFILE > 0) {
            if(i == PROFILE_YANG_ITER_SAMPLE) {
                printf("Iterations: %d\n", m+1);
                *begin = now();
            }
        }
        int c_i = get_index_of_character(C, A[i-1], u);
        // this variable is the solution to space optimization by only keeping
        // the last row for the next iteration of computations
        int partial_dp_local[units_per_self];
        
        int t, s;

        #pragma omp parallel for shared(n, i, A, B, P, DP) private(t, s) schedule(static)
        for(int j=start_id; j < end_id; j++) {
            if(j == start_id && rank == CAPTAIN) {
                j = j+1;
            }
            /* VERSION 1 (branching implementation) */
            if(USE_VERSION == 1) {
                if(A[i-1] == B[j-1]) {
                    partial_dp_local[j-start_id] = prev_row[j-1] + 1;
                }
                else if(P[(c_i*(n+1))+j] == 0) {
                    partial_dp_local[j-start_id] = max(prev_row[j], 0);
                }
                else {
                    partial_dp_local[j-start_id] = max(prev_row[j], prev_row[P[(c_i*(n+1))+j]-1] + 1);
                }
            /* VERSION 2 (no branching implementation) */
            } else {
                t = (0-P[(c_i*(n+1))+j]) < 0;
                s = (0 - (prev_row[j] - (t*prev_row[P[(c_i*(n+1))+j]-1]) ));
                partial_dp_local[j-start_id] = ((t^1)||(s^0))*(prev_row[j]) + (!((t^1)||(s^0)))*(prev_row[P[(c_i*(n+1))+j]-1] + 1);
            }
        }

        if(PROFILE > 0) {
            if(i == PROFILE_YANG_ITER_SAMPLE) {
                double exec_time = tdiff(*begin, now());
                printf("Rank %d DP partial computation time iteration %d: %f\n", rank, i, exec_time);
                *begin = now();
            }
        }
        // gather and redistribute all the calculated values of DP matrix
        sync_dp(DP, partial_dp_local, units_per_self);
        if(PROFILE > 0) {
            if(i == PROFILE_YANG_ITER_SAMPLE) {
                double exec_time = tdiff(*begin, now());
                printf("Rank %d DP matrix mpi allgather time iteration %d: %f\n", rank, i, exec_time);
                *begin = now();
            }
        }

        // update the previous row for the next iteration with the current row
        #pragma omp parallel for schedule(static)
        for(int j=1; j < n+1; j++) {
            prev_row[j] = DP[j];
        }
        if(PROFILE > 0) {
            if(i == PROFILE_YANG_ITER_SAMPLE) {
                double exec_time = tdiff(*begin, now());
                printf("Rank %d DP matrix previous row update time iteration %d: %f\n", rank, i, exec_time);
            }
        }
    }
    return DP[n];
}
