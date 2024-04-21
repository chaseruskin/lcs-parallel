#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include<time.h>
#include "omp.h"
#include "lcs.h"




//global variables
char *string_A;
char *string_B;
char *unique_chars_C; //unique alphabets
int c_len;
int **P_Matrix;
int *DP_Results; //to store the DP values
int *dp_prev_row; // to store the previous row of DP results


int main(int argc, char *argv[])
{

    if(argc <= 1){
        printf("Error: No input file specified! Please specify the input file, and run again!\n");
        return 0;
    }
    printf("\nYour input file: %s \n",argv[1]);
    

    FILE *fp;
    int len_a,len_b;
    double start_time, stop_time;

    fp = fopen(argv[1], "r");
    fscanf(fp, "%d %d %d", &len_a, &len_b, &c_len);
    printf("1 : %d %d %d\n", len_a, len_b, c_len );

    string_A = (char *)malloc((len_a+1) * sizeof(char *));
    string_B = (char *)malloc((len_b+1) * sizeof(char *));
    unique_chars_C = (char *)malloc((c_len+1) * sizeof(char *));

    fscanf(fp, "%s %s %s", string_A,string_B,unique_chars_C);
    //printf("Strings :  %s\n",  string_B );
    printf("length of string B: %zu \n C: %zu\n", strlen(string_B), strlen(unique_chars_C));
    printf("string C is: %s\n",unique_chars_C);
//allocate memory for DP Results

    DP_Results = (int *)malloc((len_b+1) * sizeof(int));
    dp_prev_row = (int *)malloc((len_b+1) * sizeof(int));
/*
 for(int k=0;k<len_a+1;k++)
    {
        DP_Results[k] = (int *)calloc((len_b+1), sizeof(int));
        if(DP_Results[k]==NULL)
        {
                printf("Can not malloc anymore on %d \n",k);
                break;
        }
    }
*/
        //allocate memory for P_Matrix array
    P_Matrix = (int **)malloc(c_len * sizeof(int *));
    for(int k=0;k<c_len;k++)
    {
        P_Matrix[k] = (int *)calloc((len_b+1), sizeof(int));
    }


//    printf("initial DP_Results: \n");
//    print_matrix(DP_Results,len_a+1,len_b+1);

//    printf("lcs is: %d\n",lcs(DP_Results,string_A,string_B,len_a,len_b));
//    printf("after normal lcs, DP_Results: \n");
//    print_matrix(DP_Results,len_a+1,len_b+1);

   // calc_P_matrix_v2(P_Matrix,string_B,len_b,unique_chars_C,c_len);
//    printf("\n P_Matrix is: \n");
//    print_matrix(P_Matrix,len_c,len_b+1);

//resetting dp array
    for(int k=0;k<len_b+1;k++){
        DP_Results[k] = 0;
        dp_prev_row[k] = 0;
    }
  //  printf("\n");
//    print_matrix(DP_Results,len_a+1,len_b+1);
    start_time = omp_get_wtime();
    calc_P_matrix_v2(P_Matrix,string_B,len_b,unique_chars_C,c_len);
    int res = lcs_yang_v2(DP_Results, dp_prev_row, P_Matrix,string_A,string_B,unique_chars_C,len_a,len_b,c_len);
    //printf("lcs_yang_v2 is: %d\n",res);
    stop_time = omp_get_wtime();
    printf("lcs_yang_v2 is: %d\n",res);
    printf("total time taken: %lf\n",stop_time-start_time);
//    printf("final DP_Results: \n");
//    print_matrix(DP_Results,len_a+1,len_b+1);
    //deallocate pointers
    free(P_Matrix);
    free(DP_Results);
    return 0;
}
