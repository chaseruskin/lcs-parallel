source /home/joelalvarez/.bashrc
INPUT_SIZE=32768

echo "--------- $INPUT_SIZE ---------" >> runtimes.txt

for i in $(seq 1 10);
do
    echo "Test " $i
    python3 ~/al/cli.py --wait --openmp --mpi lcs.c
    grep 'Time taken' build/last.out >> runtimes.txt
    
done