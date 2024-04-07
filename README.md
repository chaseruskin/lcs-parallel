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
1 | 128    | 1.314714 1.412736 1.314001 1.516043 1.808250 1.642842 | 1.5014
2 | 256    | 0.222870 2.696717 0.180971 2.435397 2.722103 0.139056 | 1.3995
3 | 512    | 5.433041 1.222936 2.702578 0.007876 1.221288 0.291711 | 1.8132
4 | 1024   | 0.159683 1.667672 0.490166 1.238659 10.862847 5.194864 | 3.269
5 | 2048   | 2.734164 2.101928 0.827958 0.220648 0.490926 0.075460 | 1.0752
6 | 4096   | 0.168352 0.835724 0.131870 0.060411 1.706948 1.974329 | 0.8129
7 | 8192   | 0.259645 0.754014 0.315039 0.843389 2.297642 0.230828 | 0.7834
8 | 16384  | 0.764816 5.962355 1.543291 3.058150 0.840464 0.428736 | 2.0996
9 | 32768  | 4.186157 1.117337 6.321267 4.008988 1.214055 1.929784 1.033326 | 2.8301

### OpenMP+MPI

This resource configuration used 4 MPI ranks, with 1 node per rank, along with 8 HW threads with 2 SW threads (OMP threads) per HW thread. Therefore, there was a total of 16 threads per MPI rank. We tried to push as much as the threading to the HW level as possible.

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
1 | 128    | 1.561666 1.723676 2.273994 0.147545 1.289653 | 1.3993
2 | 256    | 0.941684 0.564978 2.759977 2.469612 1.321112 | 1.6115
3 | 512    | 0.169044 6.491837 0.115668 0.216684 0.354489 | 1.4695
4 | 1024   | 0.096458 4.939732 3.450649 0.274236 0.310873 | 1.8144
5 | 2048   | 0.116023 0.129666 0.863543 7.504455 9.909542 0.611656 | 3.1891
6 | 4096   | 2.304642 7.187428 1.844061 27.419291 0.475772 35.046540 17.837730 | 13.1594
7 | 8192   | 3.942340 1.644256 7.460767 7.954295 | 5.2504
8 | 16384  | 73.621940 7.296778 14.122365 14.062538 61.619310 5.462781 59.965208 | 33.7358
9 | 32768  | 5.878036 75.386506 52.658042 4.455568 16.348655 18.984072 37.001678 13.972587 11.180536 2.654305 8.979272 18.410843 | 22.1592

# 2. Comparison with Published Results

TODO

# References

[1] https://www.ncbi.nlm.nih.gov/pmc/articles/PMC6458724/  
[2] https://github.com/RayhanShikder/lcs_parallel/tree/master