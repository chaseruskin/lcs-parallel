INPUT_SIZE=128


for i in {9..9};
do
    DATE="$(date '+%Y-%m-%d %H:%M:%S')"
    echo "---------  $((2**($i-1)*128)) : $DATE ---------" >> runtimes.txt
    export DATA_FILE="$i"
    for j in $(seq 1 10);
    do
        echo "Test " $j
        python3 ~/al/cli.py --wait --mpi row_wise_v2_space_opt.c
        grep 'time taken' build/last.out >> runtimes.txt
        
    done
done

