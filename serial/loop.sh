INPUT_SIZE=128


# for i in {1..9};
# do

    # export DATA_FILE="$i"
    for j in $(seq 1 10);
    do
        DATE="$(date '+%Y-%m-%d %H:%M:%S')"
        echo "--------- : $DATE ---------" >> runtimes.txt
        echo "Test " $j
        python3 ~/al/cli.py --wait --mpi --openmp lcs.c
        cat build/last.out >> archive/runtimes.txt
        
    done
# done

