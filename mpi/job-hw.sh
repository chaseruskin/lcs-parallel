#!/usr/bin/env bash
# Project: EEL6763 Final Project
# Authors: Joel Alvarez, Chase Ruskin
# File: job-hw.sh
#
# A bash script to submit a job to the HiPerGator computing system. For optimal
# performance, ideally want all hardware threads, where each core deals with
# only 1 software thread.
#
# Options:
#   --cpus-per-task         number of cores/CPUs per task (HW threads)
#   --nodes                 number of physical nodes (CPUs)
#   --ntasks                number of processes (ranks)
#   --ntasks-per-node       number of processes per node (ranks/node)
#   OMP_NUM_THREADS         number of SW threads (infinite!)
#
# This file is used as an example of how to call `srun` and configure system
# parameters for HiPerGator.

#SBATCH --job-name=al
#SBATCH --mail-type=FAIL
#SBATCH --mail-user=c.ruskin@ufl.edu
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=8
#SBATCH --ntasks=8
#SBATCH --ntasks-per-node=1
#SBATCH --ntasks-per-socket=1
#SBATCH --sockets-per-node=1
#SBATCH --mem-per-cpu=1000mb
#SBATCH -t 00:05:00
#SBATCH --cpus-per-task=1
export OMP_NUM_THREADS=1

DATA_FILE="11"

INPUT_PATH="/home/c.ruskin/eel6763/project/data/$DATA_FILE.txt"
# INPUT_PATH="/home/joelalvarez/final_project/lcs-parallel/data/$DATA_FILE.txt"

export GMON_OUT_PREFIX="gmon.out-"

srun --mpi=pmix_v3 -o "hpg-$2.out" -e "hpg-$2.err" $1 $INPUT_PATH

# Run application
# $ al ./row_wise_v2_space.c ./lcs.c --mpi --omp --opt --wait

# View gprof summary stats
# $ gprof ./build/row_wise_v2_space ./build/gmon.sum

# Capture graph using dot format
# $ gprof ./build/row_wise_v2_space ./build/gmon.sum | gprof2dot | dot -Tpng -o lcs-mpi-profile.png