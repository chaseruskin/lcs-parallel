# for i in {2..9};
# do
    DATE="$(date '+%Y-%m-%d %H:%M:%S')"
    # echo "--------- $DATE ---------" >> runtimes.txt
    # export DATA_FILE="$i"
    for j in $(seq 2 10);
    do
        echo "--------- $DATE ---------" >> runtimes.txt
        echo "Test " $j
        # python3 ~/eel6763/al/cli.py row_wise_v2_space.c lcs.c --openmp --wait
        python3 ~/al/cli.py row_wise_v2_space.c lcs.c --openmp --wait
        grep 'time taken' build/last.out >> archive/runtimes.txt
        
    done
# done