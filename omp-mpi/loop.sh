
# for i in {7..9};
# do
    # DATE="$(date '+%Y-%m-%d %H:%M:%S')"
    # echo "---------  $((2**($i-1)*128)) : $DATE ---------" >> runtimes.txt
    # export DATA_FILE="$i"
    for j in $(seq 1 10);
    do
        echo "Test " $j
        # python3 ~/eel6763/al/cli.py row_wise_v2_space.c lcs.c --mpi --openmp --wait
        python3 ~/al/cli.py row_wise_v2_space.c --mpi --openmp --wait
        cat build/last.out >> archive/runtimes.txt
        
    done
# done