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
#SBATCH --mail-user=joelalvarez@ufl.edu
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --mem-per-cpu=8000mb
#SBATCH -t 00:05:00
#SBATCH --cpus-per-task=1

# DATA_FILE="9"
INPUT_PATH="/home/joelalvarez/final_project/lcs-parallel/data/$DATA_FILE.txt"

srun --mpi=pmix_v3 -o "hpg-$2.out" -e "hpg-$2.err" $1 $INPUT_PATH
