INPUT_SIZE=32768
DATE="$(date '+%Y-%m-%d %H:%M:%S')"

echo "--------- $INPUT_SIZE : $DATE ---------" >> runtimes.txt

for i in $(seq 1 10);
do
    echo "Test " $i
    python3 ~/eel6763/al/cli.py --wait --openmp row_wise_v2.c
    grep 'total time taken' build/last.out >> runtimes.txt
    
done