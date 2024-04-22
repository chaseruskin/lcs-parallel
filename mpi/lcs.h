// Project: EEL6763 Final Project
// Author: Joel Alvarez, Chase Ruskin
// File: lcs.h
//
// Finds the longest common subsequence (LCS) from a file storing DNA data.
//
// This file is adapted from an existing implementation (https://github.com/RayhanShikder/lcs_parallel)
// in an attempt to improve its performance on UF's HiPerGator HPC computing platform.

#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "mpi.h"
#include <time.h>

// Macros
#define ALPHABET_LENGTH 4
#define max(x,y) ((x)>(y)?(x):(y))

#define CAPTAIN 0

// Find the time difference (b - a).
double tdiff(struct timespec a, struct timespec b);

// Return the current time.
struct timespec now();

// Return the number of computational "units" for rank `rank` when there are 
// `N` units and `size` ranks.
int get_computation_size(int N, int rank, int size);

int get_index_of_character(char *str,char x, int len);

void print_matrix(int **x, int row, int col);

void calc_P_matrix_v2(int *P, char *b, int len_b, char *c, int len_c, int rank, int num_ranks);

int lcs_yang_v2(int *DP, int *prev_row,  int *P, char *A, char *B, char *C, int m, int n, int u, int myrank, int chunk_size);

// additional "break-out" functions to help identify bottlenecks in application when profiling
void lcs_collect(int *DP, int chunk_size, int *dp_i_recv);

void lcs_distribute(int *DP, int chunk_size, int *dp_i_recv);

void lcs_init_distribute(int *P, int count);
