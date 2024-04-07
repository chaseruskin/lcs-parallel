# Analyzing LCS for DNA Sequencing using Parallel Programming Techniques

The goal of this project is to port multiple implementations for a parallelized LCS aglorithm to the HiPerGator computing platform to reproduce results, profile the program, and experiment with code modifications to improve performance specifically for the HiPerGator computing platform.

# 1. HiPerGator Port

Porting the application required no code modification as the applications used compatible versions of MPI and OpenMP available on the HiPerGator computing platform. Regarding resource configuration, separate job scripts were created according to the original mapping of resources outlined in the existing repository with respect to the class resource constraints.

## Benchmark Results

We used the same simulated data files for the experimental trials of varying sequence lengths. Without modification of the resource configuration, we ran the applications 4 times and averaged the results. For all experiments, we used the space optimized version 2 of the algorithm, which does not include branching.

To replicate the results and port to HiPerGator, we compiled the source code for MPI and OpenMP respectively using the available tools on the HiPerGator platform, and then translated the available SLURM scripts to SLURM scripts compatible with the HiPerGator resource constraints.

### Sequential

Data File | Sequence Length | Execution Times (sec) | Average Time (sec) |
-- | -- | -- | -- |
1 | 128    |
2 | 256    |
3 | 512    |
4 | 1024   |
5 | 2048   |
6 | 4096   |
7 | 8192   |
8 | 16384  |
9 | 32768  |

### MPI

Data File | Sequence Length | Execution Times (sec) | Average Time (sec) |
-- | -- | -- | -- |
1 | 128    |
2 | 256    |
3 | 512    |
4 | 1024   |
5 | 2048   |
6 | 4096   |
7 | 8192   |
8 | 16384  |
9 | 32768  |

### OpenMP

This resource configuration used 16 HW threads, with 1 SW thread (OMP thread) per HW thread.

Data File | Sequence Length | Execution Times (sec) | Average Time (sec) |
-- | -- | -- | -- |
1 | 128    |
2 | 256    |
3 | 512    |
4 | 1024   |
5 | 2048   |
6 | 4096   |
7 | 8192   |
8 | 16384  | 0.764816 5.962355 1.543291 3.058150 0.840464 | 2.4338
9 | 32768  | 4.186157 1.117337 6.321267 4.008988 1.214055 1.929784 1.033326 | 2.8301

### OpenMP+MPI

This resource configuration used 4 MPI ranks, with 1 node per rank, along with 8 HW threads with 2 SW threads (OMP threads) per HW thread. Therefore, there was a total of 16 threads per MPI rank.

Expected Output: _lcs_parallel/Experimental Codes/hybrid/row_wise_v2/output.txt_
```
chunk_p: 1 chunk_dp: 8192 procs: 4
lcs_yang_v2 is: 9793
time taken for lcs_yang_v2 is: 84.181711
```
Received Output:
```
Your input file: /home/c.ruskin/eel6763/project/data/9.txt 
chunk_p: 1 chunk_dp: 8192 procs: 4
lcs_yang_v2 is: 9793
time taken for lcs_yang_v2 is: 18.410843
```

Data File | Sequence Length | Execution Times (sec) | Average Time (sec)
-- | -- | -- | -- |
1 | 128    |
2 | 256    |
3 | 512    |
4 | 1024   |
5 | 2048   |
6 | 4096   |
7 | 8192   |
8 | 16384  | 73.621940 7.296778 14.122365 14.062538 61.619310 5.462781 59.965208 | 33.7358
9 | 32768  | 5.878036 75.386506 52.658042 4.455568 16.348655 18.984072 37.001678 13.972587 11.180536 2.654305 8.979272 18.410843 | 22.1592

# 2. Comparison with Published Results

TODO

# References

[1] https://www.ncbi.nlm.nih.gov/pmc/articles/PMC6458724/  
[2] https://github.com/RayhanShikder/lcs_parallel/tree/master