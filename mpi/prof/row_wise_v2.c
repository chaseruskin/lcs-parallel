#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"
#include <stdint.h>
#include <time.h>
//macros
#define ALPHABET_LENGTH 4
#define max(x,y) ((x)>(y)?(x):(y))

#define PROFILE 1
#define PROFILE_YANG_ITER_SAMPLE 10
#define CAPTAIN 0

struct timespec *begin;
struct timespec *prof_mark;
struct timespec *end;

//global variables
char *string_A;
char *string_B;
char *unique_chars_C; //unique alphabets
int c_len;
int *P_Matrix;
int *DP_Results; //to store the DP values
int *dp_prev_row;
//function prototypes
int get_index_of_character(char *str,char x, int len);
void print_matrix(int **x, int row, int col);
void calc_P_matrix_v2(int *P, char *b, int len_b, char *c, int len_c, int myrank, int chunk_size);
int lcs_yang_v2(int *DP, int *prev_row,  int *P, char *A, char *B, char *C, int m, int n, int u, int myrank, int chunk_size);
int lcs(int **DP, char *A, char *B, int m, int n);

// Find the time difference (b - a).
double tdiff(struct timespec a, struct timespec b);

// Return the current time.
struct timespec now();

// Return the number of computational "units" for rank `rank` when there are 
// `N` units and `size` ranks.
int get_computation_size(int N, int rank, int size);


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


double tdiff(struct timespec a, struct timespec b) {
    double dt = (( b.tv_sec - a.tv_sec ) + ( b.tv_nsec - a.tv_nsec ) / 1E9);
    return dt;
}


struct timespec now() {
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return t;
}


int get_index_of_character(char *str,char x, int len)
{
    for(int i=0;i<len;i++)
    {
        if(str[i]== x)
        {
            return i;
        }
    }
    return -1;//not found the character x in str
}


void calc_P_matrix_v2(int *P, char *b, int len_b, char *c, int len_c, int myrank, int num_ranks)
{
    if(PROFILE > 0) {
        *prof_mark = now();
    }


    // determine number of elements that each rank will receive
    int *counts_c = calloc(num_ranks, sizeof(int));
    int *counts_p = calloc(num_ranks, sizeof(int));
    // determine offset from source data from which to take outgoing data to i'th rank
    int *displs_c = calloc(num_ranks, sizeof(int));
    int *displs_p = calloc(num_ranks, sizeof(int));

    // determine number of chunks that this rank must compute
    int chunk_size = get_computation_size(len_c, myrank, num_ranks);

    char receive_array_for_scatter_c[chunk_size];
    int receive_array_for_scatter_p[chunk_size*(len_b+1)];

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

    //Scatter the char array chunks by sending each process a particular chunk
    MPI_Scatterv(c, counts_c, displs_c, MPI_CHAR, &receive_array_for_scatter_c, chunk_size, MPI_CHAR, 0, MPI_COMM_WORLD);
    //Scatter the char array chunks by sending each process a particular chunk
    MPI_Scatterv(P, counts_p, displs_p, MPI_INT, &receive_array_for_scatter_p, chunk_size*(len_b+1), MPI_INT, 0, MPI_COMM_WORLD);

    // Broadcast the whole b  array to everybody
    MPI_Bcast(b, len_b, MPI_CHAR, 0, MPI_COMM_WORLD);

    for(int i=0;i<chunk_size;i++)
    {
        for(int j=1;j<len_b+1;j++)
        {
            if(b[j-1]==receive_array_for_scatter_c[i])
            {
                receive_array_for_scatter_p[(i*(len_b+1))+j] = j;
            }
            else
            {
                receive_array_for_scatter_p[(i*(len_b+1))+j] = receive_array_for_scatter_p[(i*(len_b+1))+j-1];
            }
        }
    }

    if(PROFILE > 0) {
        double exec_time = tdiff(*prof_mark, now());
        if(chunk_size > 0) {
            printf("Rank %d P matrix computation time: %f\n", myrank, exec_time);
        }
        *prof_mark = now();
    }
    // gather all the calculated values of P matrix in process 0 and redistribute
    MPI_Gatherv(receive_array_for_scatter_p, chunk_size*(len_b+1), MPI_INT, P, counts_p, displs_p, MPI_INT, CAPTAIN, MPI_COMM_WORLD);
    if(PROFILE > 0) {
        double exec_time = tdiff(*prof_mark, now());
        if(myrank == CAPTAIN) {
            printf("Rank %d P matrix mpi gather time: %f\n", myrank, exec_time);
        }
    }
}


int lcs_yang_v2(int *DP, int *prev_row, int *P, char *A, char *B, char *C, int m, int n, int u, int myrank, int chunk_size)
{
    if(PROFILE > 0) {
        *prof_mark = now();
    }
    MPI_Bcast(P, (u*(n+1)), MPI_INT, 0, MPI_COMM_WORLD);

    if(PROFILE > 0) {
        double exec_time = tdiff(*prof_mark, now());
        printf("Rank %d P matrix mpi broadcast time: %f\n", myrank, exec_time);
    }

    for(int i=1;i<m+1;i++)
    {
        if(PROFILE > 0) {
            if(i == PROFILE_YANG_ITER_SAMPLE) {
                printf("Iterations: %d\n", m+1);
                *prof_mark = now();
            }
        }
        int c_i = get_index_of_character(C,A[i-1],u);
        int dp_i_receive[chunk_size];
        MPI_Scatter(DP, chunk_size, MPI_INT,&dp_i_receive,chunk_size,MPI_INT, 0, MPI_COMM_WORLD);
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

        if(PROFILE > 0) {
            if(i == PROFILE_YANG_ITER_SAMPLE) {
                double exec_time = tdiff(*prof_mark, now());
                printf("Rank %d DP partial computation time iteration %d: %f\n", myrank, i, exec_time);
                *prof_mark = now();
            }
        }

        //now gather all the calculated values of P matrix in process 0
        MPI_Allgather(dp_i_receive, chunk_size, MPI_INT,DP, chunk_size, MPI_INT, MPI_COMM_WORLD);

        if(PROFILE > 0) {
            if(i == PROFILE_YANG_ITER_SAMPLE) {
                double exec_time = tdiff(*prof_mark, now());
                printf("Rank %d DP matrix mpi gather time iteration %d: %f\n", myrank, i, exec_time);
                *prof_mark = now();
            }
        }
        for(int j=1;j<n+1;j++){
                prev_row[j] = DP[j];
        }
        if(PROFILE > 0) {
            if(i == PROFILE_YANG_ITER_SAMPLE) {
                double exec_time = tdiff(*prof_mark, now());
                printf("Rank %d DP matrix previous row update time iteration %d: %f\n", myrank, i, exec_time);
            }
        }
    }
    return DP[n];
}


int main(int argc, char *argv[])
{
    if(argc <= 1){
        printf("Error: No input file specified! Please specify the input file, and run again!\n");
        return 0;
    }

    int my_rank;
    int num_procs;
    int chunk_size_p,chunk_size_dp;//chunk_size for P matrix and DP matrix
    int res;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); //grab this process's rank
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs); //grab the total num of processes

    FILE *fp;
    int len_a,len_b;
    double start_time,stop_time,start_time_yang,stop_time_yang;

    if(my_rank == 0)printf("\nYour input file: %s \n",argv[1]);
    fp = fopen(argv[1], "r");
    fscanf(fp, "%d %d %d", &len_a, &len_b, &c_len);
    string_A = (char *)malloc((len_a+1) * sizeof(char *));
    string_B = (char *)malloc((len_b+1) * sizeof(char *));
    unique_chars_C = (char *)malloc((c_len+1) * sizeof(char *));

    fscanf(fp, "%s %s %s", string_A,string_B,unique_chars_C);

    chunk_size_p = (c_len/num_procs);
    if(num_procs > 4 && my_rank < 4) {
        chunk_size_p = 1;
    }
    chunk_size_dp = ((len_b+1)/num_procs);

    if(my_rank==0)
    {
        printf("chunk_p: %d chunk_dp: %d procs: %d\n",chunk_size_p,chunk_size_dp,num_procs);
    }

    DP_Results = (int *)malloc((len_b+1) * sizeof(int));
    dp_prev_row = (int *)malloc((len_b+1) * sizeof(int));

    P_Matrix = (int *)malloc((c_len*(len_b+1)) * sizeof(int));


    begin = calloc(1, sizeof(struct timespec));
    prof_mark = calloc(1, sizeof(struct timespec));
    end = calloc(1, sizeof(struct timespec));

    *begin = now();

    calc_P_matrix_v2(P_Matrix,string_B,len_b,unique_chars_C,c_len, my_rank, num_procs);


    res = lcs_yang_v2(DP_Results, dp_prev_row, P_Matrix,string_A,string_B,unique_chars_C,len_a,len_b,c_len,my_rank, chunk_size_dp);

    *end = now();

    if(my_rank == 0) {
        double exec_time = tdiff(*begin, *end);
        printf("lcs_yang_v2 is: %d\n", res);
        printf("time taken for lcs_yang_v2 is: %lf\n", exec_time);
    }
    //deallocate pointers
    free(P_Matrix);
    free(DP_Results);

    free(begin);
    free(prof_mark);
    free(end);

    // Shutdown MPI (important - don't forget!)
    MPI_Finalize();
    return 0;
}
