INPUT_SIZE=128


for i in {1..9};
do
    echo "---------  $((i*128)) ---------" >> runtimes.txt
    export DATA_FILE="$i"
    for i in $(seq 1 10);
    do
        echo "Test " $i
        python3 ~/al/cli.py --wait --mpi row_wise_v2.c
        grep 'time taken' build/last.out >> runtimes.txt
        
    done
done

