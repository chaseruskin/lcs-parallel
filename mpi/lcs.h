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
#include "omp.h"
#include <time.h>

// Macros
#define ALPHABET_LENGTH 4
#define max(x,y) ((x)>(y)?(x):(y))

#define PROFILE 0
#define USE_VERSION 1
#define DEBUG 0
#define CAPTAIN 0
// the iteration in the main loop when to take an execution time sample for the yang algorithm
#define PROFILE_YANG_ITER_SAMPLE 2

struct timespec *begin;
struct timespec *end;

// Find the time difference (b - a).
double tdiff(struct timespec a, struct timespec b);

// Return the current time.
struct timespec now();

// Return the number of computational "units" for rank `rank` when there are 
// `N` units and `size` ranks.
int get_computation_size(int N, int rank, int size);

int get_index_of_character(char *str,char x, int len);

void print_matrix(int **x, int row, int col);

void calc_P_matrix_v2(int *p_global, char *b, int len_b, char *c, int len_c, int rank, int num_ranks);

int lcs_yang_v2(int *DP, int *prev_row,  int *P, char *A, char *B, char *C, int len_a, int len_b, int len_c, int rank, int units_per_self);

// additional "break-out" functions to help identify bottlenecks in application when profiling
void sync_dp(int *DP, int *dp_i_recv, int chunk_size);

void distribute_p(int *P, int count, int rank);
