# Analyzing LCS for DNA Sequencing using Parallel Programming Techniques

The goal of this project is to port multiple implementations for a parallelized LCS aglorithm to the HiPerGator computing platform to reproduce results, profile the program, and experiment with code modifications to improve performance specifically for the HiPerGator computing platform.


## Results

We used the same simulated data files for the experimental trials of varying sequence lengths. Without modification of the resource configuration, we ran the applications 4 times and averaged the results. For all experiments, we used the space optimized version 2 of the algorithm, which does not include branching.

To replicate the results and port to HiPerGator, we compiled the source code for MPI and OpenMP respectively using the available tools on the HiPerGator platform, and then translated the available SLURM scripts to SLURM scripts compatible with the HiPerGator resource constraints.

### Sequential

128    :
256    :
512    :
1024   :
2048   :
4096   :
8192   :
16384  :
32768  :
658042 :

### MPI

128    :
256    :
512    :
1024   :
2048   :
4096   :
8192   :
16384  :
32768  :
658042 :

### OpenMP

This resource configuration used 16 HW threads, with 1 SW thread (OMP thread) per HW thread.

128    :
256    :
512    :
1024   :
2048   :
4096   :
8192   :
16384  :
32768  : 0.764816 5.962355 1.543291 3.058150 0.840464
658042 : 4.186157 1.117337 6.321267 4.008988 1.214055 1.929784 1.033326

### OpenMP+MPI

This resource configuration used 4 MPI ranks, with 1 node per rank, along with 8 HW threads with 2 SW threads (OMP threads) per HW thread. Therefore, there was a total of 16 threads per MPI rank.

128    :
256    :
512    :
1024   :
2048   :
4096   :
8192   :
16384  :
32768  : 73.621940 7.296778 14.122365 14.062538 61.619310 5.462781 59.965208
658042 : 5.878036 75.386506 52.658042 4.455568 16.348655 18.984072 37.001678 13.972587 11.180536 2.654305 8.979272


## References

[1] https://www.ncbi.nlm.nih.gov/pmc/articles/PMC6458724/  
[2] https://github.com/RayhanShikder/lcs_parallel/tree/master