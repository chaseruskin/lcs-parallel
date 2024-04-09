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
1 | 128    | 0.000371 0.000023 0.000371 0.000020 0.000017 0.000015 0.000016 0.000018 0.000017 0.000015 | 0.0001
2 | 256    | 0.000441 0.000049 0.000049 0.000049 0.000046 0.000064 0.000062 0.000048 0.000050 0.000064 | 0.0001
3 | 512    | 0.000207 0.000207 0.000203 0.000198 0.000201 0.000206 0.000204 0.000205 0.000199 0.000201 | 0.0002
4 | 1024   | 0.000779 0.000853 0.001245 0.000796 0.000869 0.000905 0.000810 0.000938 0.000801 0.000814 | 0.0009
5 | 2048   | 0.002980 0.002948 0.003000 0.002997 0.003033 0.002953 0.003064 0.003038 0.002962 0.002958 | 0.0030
6 | 4096   | 0.012194 0.011941 0.011916 0.012104 0.011995 0.011965 0.012476 0.012556 0.012582 0.011893 | 0.0122
7 | 8192   | 0.049810 0.049226 0.049130 0.045651 0.049054 0.049443 0.048955 0.049533 0.048858 0.046275 | 0.0486
8 | 16384  | 0.231989 0.233137 0.786195 0.789942 0.782391 0.776745 0.784816 0.778986 0.786684 0.788419 | 0.6739
9 | 32768  | 2.606464 2.605415 2.609088 2.604304 2.797391 2.868034 2.777950 2.811413 2.661989 2.796924 | 2.7139

### MPI

This resource configuration used 4 MPI ranks, with 1 node per rank.

Data File | Sequence Length | Execution Times (sec) | Average Time (sec) |
-- | -- | -- | -- |
1 | 128    | 0.000781 0.000826 0.000909 0.000933 0.000885 0.000928 0.000881 0.000843 0.000812 0.000783 | 0.0009
2 | 256    | 0.001420 0.001642 0.001606 0.001502 0.001574 0.001534 0.001533 0.001779 0.001542 0.001595 | 0.0016
3 | 512    | 0.002765 0.002794 0.003021 0.003032 0.002808 0.002895 0.002748 0.002837 0.002865 0.002917 | 0.0029
4 | 1024   | 0.006607 0.007608 0.007272 0.007079 0.007396 0.006403 0.006653 0.006762 0.007156 0.007018 | 0.0070
5 | 2048   | 0.016473 0.016739 0.016742 0.016796 0.017201 0.018152 0.017032 0.016428 0.018390 0.017900 | 0.0172
6 | 4096   | 0.065361 0.056541 0.067667 0.057429 0.046166 0.051763 0.051218 0.048193 0.051687 0.051998 | 0.0548
7 | 8192   | 0.162353 0.162699 0.163085 0.161936 0.163740 0.167107 0.158388 0.162769 0.163486 0.145924 | 0.1611
8 | 16384  | 0.507004 0.493545 0.487270 0.485374 0.477792 0.487721 0.501135 0.512924 0.496231 0.474635 | 0.4924
9 | 32768  | 1.770670 2.129402 1.757656 1.761916 1.780164 1.767418 1.749707 1.778065 1.773650 1.755343 | 1.8024

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

The following table approximates the recorded measurements referenced in the paper's graph (Figure 2 (a)).

![](./images/fig2a.png)

Data File | Sequence Length | Sequential (sec) | MPI (sec) | OpenMP (sec) | OpenMP+MPI (sec)
-- | -- | -- | -- | -- | -- |
1 | 128    | ~0 | ~0 | ~0 | ~0
2 | 256    | ~0 | ~0 | ~0 | ~0
3 | 512    | ~0 | ~0 | ~0 | ~0
4 | 1024   | ~0 | 1 | ~0 | 1
5 | 2048   | ~0 | 2.5 | ~0 | 2
6 | 4096   | ~0 | 5 | ~0 | 4
7 | 8192   | 0.5 | 11 | ~0 | 10
8 | 16384  | 2 | 28 | 0.5 | 32
9 | 32768  | 7 | 33 | 4 | 40


# References

[1] https://www.ncbi.nlm.nih.gov/pmc/articles/PMC6458724/  
[2] https://github.com/RayhanShikder/lcs_parallel/tree/master