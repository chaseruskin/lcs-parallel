for i in {7..9};
do
    DATE="$(date '+%Y-%m-%d %H:%M:%S')"
    echo "---------  $((2**($i-1)*128)) : $DATE ---------" >> runtimes.txt
    export DATA_FILE="$i"
    for j in $(seq 1 10);
    do
        echo "Test " $j
        python3 ~/eel6763/al/cli.py row_wise_v2.c --openmp --wait
        grep 'time taken' build/last.out >> runtimes.txt
        
    done
done