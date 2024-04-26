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


void calc_P_matrix(int *p_global, char *b, int len_b, char *c, int len_c, int rank, int num_ranks) {

    #if PROFILE
        *prof_mark = now();
    #endif
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
    #if PROFILE
        double exec_time = tdiff(*prof_mark, now());
        if(chunk_size > 0) {
            printf("Rank %d P matrix computation time: %f\n", rank, exec_time);
        }
        *prof_mark = now();
    #endif
    // gather all the calculated values of P matrix in process 0 and redistribute
    MPI_Gatherv(p_local, chunk_size*(len_b+1), MPI_INT, p_global, counts_p, displs_p, MPI_INT, CAPTAIN, MPI_COMM_WORLD);
    #if PROFILE
        exec_time = tdiff(*prof_mark, now());
        if(rank == CAPTAIN) {
            printf("Rank %d P matrix mpi gather time: %f\n", rank, exec_time);
        }
    #endif
}


void sync_r(int *R_prev_row, int *R_part_row, int rank, int *units_per_rank, int *displ_per_rank, int num_procs, int offset) {

    int units_per_self = units_per_rank[rank];
    // maybe use async send/recv with a barrier?
    switch(rank) {
        case CAPTAIN: {
            // wait for the last MPI_Isend to complete before progressing further.
            // MPI_Wait(request, MPI_STATUS_IGNORE);
            // always just send data to the upstream neighbor
            for(int i = 0; i < NEIGHBOR_DIST; i++) {
                // break early if there are no more neighbors in the chain to pass to
                if(rank+1+i >= num_procs) {
                    break;
                }
                // if we have an upstream neighbor, then send data to it!
                MPI_Send(R_part_row, units_per_self, MPI_INT, rank+1+i, TAG_NEXT_R_SEGMENT, MPI_COMM_WORLD);
            }
            break;
        }
        default: {
            for(int i = 0; i < NEIGHBOR_DIST; i++) {
                // break early if there are no more neighbors in the chain to pass to
                if(rank+1+i >= num_procs) {
                    break;
                }
                // if we have an upstream neighbor, then send data to it!
                MPI_Send(R_part_row, units_per_self, MPI_INT, rank+1+i, TAG_NEXT_R_SEGMENT, MPI_COMM_WORLD);
            }
            // update the previous row for the next iteration with the current row
            for(int i = 0; i < NEIGHBOR_DIST; i++) {
                // break early if there are no more neighbors to get data from 
                if(rank-1-i < 0) {
                    break;
                }
                // receive the computed data from the downstrem neighbor in the right location of the R array
                MPI_Recv(&R_prev_row[displ_per_rank[rank-1-i]-offset], units_per_rank[rank-1-i], MPI_INT, rank-1-i, TAG_NEXT_R_SEGMENT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            break;
        }
    }
}


void distribute_p(int *P, int count, int rank) {
    #if PROFILE
        *prof_mark = now();
    #endif
    MPI_Bcast(P, count, MPI_INT, CAPTAIN, MPI_COMM_WORLD);
    #if PROFILE
        double exec_time = tdiff(*prof_mark, now());
        printf("Rank %d P matrix mpi broadcast time: %f\n", rank, exec_time);
    #endif
}


int lcs_yang(int *R_prev_row, int *P, char *A, char *B, char *C, int len_a, int len_b, int len_c, int rank, int *units_per_rank, int *displ_per_rank, int num_procs, int len_r_prev_row_size) {
    // distribute the P matrix to all processes
    distribute_p(P, (len_c*(len_b+1)), rank);

    int m = len_a;
    int n = len_b;
    int u = len_c;

    #if DEBUG > 0
        printf("Loop iterations (m): %d\n", m);
    #endif

    int units_per_self = units_per_rank[rank];

    // these "zones" are consistent for 'm' operations
    int start_id = 0;
    int offset = 0;
    for(int i = 0; i < rank; i++) {
        start_id += units_per_rank[i];
        if(i < rank-NEIGHBOR_DIST) {
            offset += units_per_rank[i];
        }
    }
    // include the number of work units for this particular rank
    int end_id = start_id + units_per_rank[rank];

    int lowest_access = start_id+end_id+1;
    int highest_access = -1;
    int lowest_i = -1;
    int highest_i = -1;

    int result = -1;

    // difference to align with partial R previous row for space optimization
    #if DEBUG > 0
        printf("Rank %d aligning data in R previous row with offset %d\n", rank, offset);
    #endif

    // this variable is the solution to space optimization by only keeping the last row for the next iteration of computations
    int R_part_row[units_per_self];

    for(int i=1; i < m+1; i++) {
        #if PROFILE
            if(i == PROFILE_YANG_ITER_SAMPLE) {
                printf("Iterations: %d\n", m+1);
                *prof_mark = now();
            }
        #endif
        int c_i = get_index_of_character(C, A[i-1], u);

        // reset the partial R row array for this iteration on this particular rank
        memset(&R_part_row, 0, units_per_self);

        int t, s, access;

        #pragma omp parallel for shared(n, c_i, i, A, B, P, R_prev_row) private(t, s, access) schedule(static)
        for(int j = start_id; j < end_id; j++) {
            if(j == start_id && rank == CAPTAIN) {
                j = j+1;
            }
            /* VERSION 1 (branching implementation) */
            #if USE_VERSION == 1
                if(A[i-1] == B[j-1]) {
                    R_part_row[j-start_id] = R_prev_row[j-1-offset] + 1;
                }
                else if(P[(c_i*(n+1))+j] == 0) {
                    R_part_row[j-start_id] = max(R_prev_row[j-offset], 0);
                }
                else {
                    R_part_row[j-start_id] = max(R_prev_row[j-offset], R_prev_row[P[(c_i*(n+1))+j]-1-offset] + 1);
                }
            /* VERSION 2 (no branching implementation) */
            #else
                t = (0-P[(c_i*(n+1))+j]) < 0;
                s = (0 - (R_prev_row[j-offset] - (t*R_prev_row[P[(c_i*(n+1))+j]-1-offset]) ));
                R_part_row[j-start_id] = ((t^1)||(s^0))*(R_prev_row[j-offset]) + (!((t^1)||(s^0)))*(R_prev_row[P[(c_i*(n+1))+j]-1-offset] + 1);
            #endif
            // provide a level of error-handling
            if(rank-NEIGHBOR_DIST >= 0 && P[(c_i*(n+1))+j]-1 < displ_per_rank[rank-NEIGHBOR_DIST]) {
                printf("ERROR: Rank %d is missing data from neighbor %d; result may be incorrect\n", rank, rank-NEIGHBOR_DIST);
                fflush(stdout);
                MPI_Abort(MPI_COMM_WORLD, 103);
            }
            #if DEBUG > 0
                if(highest_access < j-1) {
                    highest_access = j-1;
                }
                if(lowest_access > j-1) {
                    lowest_access = j-1;
                }
                if(highest_access < j) {
                    highest_access = j;
                }
                if(lowest_access > j) {
                    lowest_access = j;
                }
                if(highest_access < P[(c_i*(n+1))+j]-1) {
                    highest_access = P[(c_i*(n+1))+j]-1;
                }
                if(lowest_access > P[(c_i*(n+1))+j]-1) {
                    lowest_access = P[(c_i*(n+1))+j]-1;
                }
            #endif
        }

        #if PROFILE
            if(i == PROFILE_YANG_ITER_SAMPLE) {
                double exec_time = tdiff(*prof_mark, now());
                printf("Rank %d DP partial computation time iteration %d: %f\n", rank, i, exec_time);
                *prof_mark = now();
            }
        #endif

        // IDEA: only distribute information to the neighboring nodes that need it
        // gather and redistribute all the calculated values of DP matrix -> done!
        sync_r(R_prev_row, R_part_row, rank, units_per_rank, displ_per_rank, num_procs, offset);
        #if PROFILE
        if(i == PROFILE_YANG_ITER_SAMPLE) {
            double exec_time = tdiff(*prof_mark, now());
            printf("Rank %d DP matrix mpi gather time iteration %d: %f\n", rank, i, exec_time);
            *prof_mark = now();
        }
        #endif

        // update the previous row with the latest computed values from this iteration
        #pragma omp parallel for shared(R_part_row, displ_per_rank, rank, offset) schedule(static)
        for(int i = 0; i < units_per_self; i++) {
            R_prev_row[displ_per_rank[rank]-offset+i] = R_part_row[i];
        }

        result = R_prev_row[len_r_prev_row_size-1];

        #if PROFILE
            if(i == PROFILE_YANG_ITER_SAMPLE) {
                double exec_time = tdiff(*prof_mark, now());
                printf("Rank %d DP matrix previous row update time iteration %d: %f\n", rank, i, exec_time);
            }
        #endif
    }
    #if DEBUG > 0
        printf("Rank %d === start_id %d end_id %d\n", rank, start_id, end_id);
        printf("Rank %d === lowest_access %d highest_access %d\n", rank, lowest_access, highest_access);
        printf("COMM REQUIREMENT: Rank %d needed data from segment belonging to rank %d\n", rank, lowest_access/units_per_self);
        printf("COMM REQUIREMENT: Rank %d needed data from segment belonging to rank %d\n", rank, highest_access/units_per_self);
    #endif

    int LAST_RANK = num_procs-1;

    // wait on final result from last node in the linear chain
    if(rank == LAST_RANK) {
        #if DEBUG > 1
            printf("LAST RANK R MATRIX:\n");
            for(int i = 0; i < len_r_prev_row_size; i++) {
                printf("%d\t", R_prev_row[i]);
            }
            printf("\nEND LAST RANK R MATRIX\n");
        #endif

        MPI_Send(&result, 1, MPI_INT, CAPTAIN, TAG_FINAL_R_VALUE, MPI_COMM_WORLD);
    }
    if(rank == CAPTAIN) {
        MPI_Recv(&result, 1, MPI_INT, LAST_RANK, TAG_FINAL_R_VALUE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    return result;
}
