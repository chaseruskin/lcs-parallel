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

// the number of downstream neighbors to collect data from and upstream neighbors to send data to
#define NEIGHBOR_DIST 1

// various options for debugging and profiling with various implementations
#define PROFILE 1
#define DEBUG 0
// the iteration in the main loop when to take an execution time sample for the yang algorithm
#define PROFILE_YANG_ITER_SAMPLE 10

// the version of the yang algorithm to use (1: BRANCHING, 2: NO BRANCHING)
#define USE_VERSION 2
// root rank/process ID
#define CAPTAIN 0
// tags for message conversations
#define TAG_NEXT_R_SEGMENT 0
#define TAG_FINAL_R_VALUE 1

struct timespec *begin;
struct timespec *prof_mark;
struct timespec *end;

// Find the time difference (b - a).
double tdiff(struct timespec a, struct timespec b);

// Return the current time.
struct timespec now();

// Return the number of computational "units" for rank `rank` when there are 
// `N` units and `size` ranks.
int get_computation_size(int N, int rank, int size);

int get_index_of_character(char *str,char x, int len);

void calc_P_matrix(int *p_global, char *b, int len_b, char *c, int len_c, int rank, int num_ranks);

int lcs_yang(int *R_prev_row,  int *P, char *A, char *B, char *C, int len_a, int len_b, int len_c, int rank, int *units_per_rank, int *displ_per_rank, int num_procs, int len_r_prev_row_size);

// additional "break-out" functions to help identify bottlenecks in application when profiling
void sync_r(int *R_prev_row, int *R_part_row, int rank, int *units_per_rank, int *displ_per_rank, int num_procs, int offset);

void distribute_p(int *P, int count, int rank);
