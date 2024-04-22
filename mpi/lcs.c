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


int get_index_of_character(char *str,char x, int len) {
    for(int i=0; i<len; i++) {
        if(str[i]== x) {
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


void calc_P_matrix_v2(int *P, char *b, int len_b, char *c, int len_c, int rank, int num_ranks) {

    // determine number of elements that each rank will receive
    int *send_counts_c = calloc(num_ranks, sizeof(int));
    int *send_counts_p = calloc(num_ranks, sizeof(int));
    // determine offset from source data from which to take outgoing data to i'th rank
    int *displacements_c = calloc(num_ranks, sizeof(int));
    int *displacements_p = calloc(num_ranks, sizeof(int));

    int chunk_size = get_computation_size(len_c, rank, num_ranks);

    uint32_t acc_c = 0;
    uint32_t acc_p = 0;
    for(int i = 0; i < num_ranks; i++) {
        // send two additional rows for neighboring
        send_counts_c[i] = get_computation_size(len_c, i, num_ranks);
        send_counts_p[i] = send_counts_c[i] * (len_b + 1);
        // compute displacement (count how many rows are already processed * elements per row)
        // provide an upper row for neighboring filter computation
        displacements_c[i] = acc_c;
        displacements_p[i] = acc_p;

        // printf("send_counts_c[%d] = %d, displacements_c[%d] = %d\n", i, send_counts_c[i], i, displacements_c[i]);
        // printf("send_counts_p[%d] = %d, displacements_p[%d] = %d\n", i, send_counts_p[i], i, displacements_p[i]);
        
        acc_c += send_counts_c[i];
        acc_p += send_counts_p[i];
    }

    char *teammate_array_for_scatter_c = calloc(chunk_size, sizeof(char));
    // char teammate_array_for_scatter_c[chunk_size];
    int *teammate_array_for_scatter_p = calloc(chunk_size * (len_b+1), sizeof(int));

    // all of this is unneeded
    // Scatter the char array chunks by sending each process a particular chunk
    // MPI_Scatterv(c, send_counts_c, displacements_c, MPI_INT, teammate_array_for_scatter_c, send_counts_c[rank], MPI_INT, CAPTAIN, MPI_COMM_WORLD);
    // Scatter the char array chunks by sending each process a particular chunk
    // MPI_Scatterv(P, send_counts_p, displacements_p, MPI_INT, teammate_array_for_scatter_p, send_counts_p[rank], MPI_INT, CAPTAIN, MPI_COMM_WORLD);
    // Broadcast the whole b array to all processes
    // MPI_Bcast(b, len_b, MPI_CHAR, CAPTAIN, MPI_COMM_WORLD);

    if(chunk_size > 0) {
        // compute the partition of the P matrix
        for(int i=0; i < chunk_size; i++) {
            // printf("rank: %d, char: %c\n", rank, );
            for(int j=1; j < len_b+1; j++) {
                if(b[j-1] == c[displacements_c[rank]+i]) {
                    teammate_array_for_scatter_p[(i*(len_b+1))+j] = j;
                } else {
                    teammate_array_for_scatter_p[(i*(len_b+1))+j] = teammate_array_for_scatter_p[(i*(len_b+1))+j-1];
                }
            }
        }
    }

    // now gather all the calculated values of P matrix in process 0
    MPI_Gatherv(teammate_array_for_scatter_p, chunk_size*(len_b+1), MPI_INT, P, send_counts_p, displacements_p, MPI_INT, CAPTAIN, MPI_COMM_WORLD);
}


void lcs_distribute(int *DP, int chunk_size, int *dp_i_recv) {
    MPI_Scatter(DP, chunk_size, MPI_INT, dp_i_recv, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
}


void lcs_collect(int *DP, int chunk_size, int *dp_i_recv) {
    MPI_Allgather(dp_i_recv, chunk_size, MPI_INT, DP, chunk_size, MPI_INT, MPI_COMM_WORLD);
}


void lcs_init_distribute(int *P, int count) {
    MPI_Bcast(P, count, MPI_INT, 0, MPI_COMM_WORLD);
}


int lcs_yang_v2(int *DP, int *prev_row, int *P, char *A, char *B, char *C, int m, int n, int u, int myrank, int chunk_size)
{
    lcs_init_distribute(P, (u*(n+1)));
    // MPI_Bcast(P, (u*(n+1)), MPI_INT, 0, MPI_COMM_WORLD);
    for(int i=1;i<m+1;i++)
    {
        int c_i = get_index_of_character(C,A[i-1],u);
        int dp_i_receive[chunk_size];
        
        lcs_distribute(DP, chunk_size, dp_i_receive);
        // MPI_Scatter(DP, chunk_size, MPI_INT, &dp_i_receive, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
        
        int start_id = (myrank * chunk_size);
        int end_id = (myrank * chunk_size) + chunk_size;

        int t,s;

        for(int j=start_id;j<end_id;j++)
        {
            if(j==start_id && myrank==0)j=j+1;
            t= (0-P[(c_i*(n+1))+j])<0;
            s= (0 - (prev_row[j] - (t*prev_row[P[(c_i*(n+1))+j]-1]) ));
            dp_i_receive[j-start_id] = ((t^1)||(s^0))*(prev_row[j]) + (!((t^1)||(s^0)))*(prev_row[P[(c_i*(n+1))+j]-1] + 1);
        }
        //now gather all the calculated values of P matrix in process 0
        
        lcs_collect(DP, chunk_size, dp_i_receive);
        // MPI_Allgather(dp_i_receive, chunk_size, MPI_INT,DP, chunk_size, MPI_INT, MPI_COMM_WORLD);

        for(int j=1;j<n+1;j++){
            prev_row[j] = DP[j];
        }
    }
    return DP[n];
}
