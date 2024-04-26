# Script: avg-prof.py
#
# Takes the best, worst, and average of profiling output.

# PROFILE FILE: using 32 ranks, 11.txt

REF_MPI = 'ref-mpi-11.out'

OPT_MPI = 'opt-mpi-11.out'

FILE = './tools/prof/' + OPT_MPI

lines = []

map = {
    # P matrix steps
    'P matrix computation time:': [],
    'P matrix mpi gather time:': [],
    'P matrix mpi broadcast time:': [],
    # R matrix steps
    'DP partial computation time iteration 10:': [],
    'DP matrix mpi gather time iteration 10:': [],
    'DP matrix previous row update time iteration 10:': [],
}

with open(FILE, 'r') as fp:
    for l in fp.readlines():
        lines += [l.strip()]
    pass

rec: str
for rec in lines:
    if rec.startswith('Rank ') == False:
        continue
    rank, other = rec[5:].split(' ', maxsplit=1)
    # print(rank)
    # print(other)
    i = other.find(':')
    # print(other[:i+1])
    if other[:i+1] in map.keys():
        map[other[:i+1]] += [float(other[i+1:].strip())]
    else:
        print(other[:i+1])
        print('missed!')
    pass
# print(lines)
for step, times in map.items():
    print(step)
    min = times[0]
    max = times[0]
    avg = 0
    for t in times:
        if t < min:
            min = t
        if t > max:
            max = t
        avg += t
        pass
    print('worst:', f'{max:.12f}')
    print('best:', f'{min:.12f}')
    print('average: ' + f'{avg/len(times):.12f}')
    print()
