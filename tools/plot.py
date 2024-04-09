# Script: plot.py
#
# Plots the averages across all varying input sizes and parallel programming 
# approaches.
#
# Debugging: 
# - Installing `matplotlib`: $ pip3 install matplotlib --user
# - `plt.show()` will not display interactive plot through SSH remote... run on local machine

from matplotlib import pyplot as plt

sizes = []
for i in range(0, 9):
    sizes += [2**(i)*128]

avg_serial = [
    0.0001,
    0.0001,
    0.0002,
    0.0009,
    0.0030,
    0.0122,
    0.0486,
    0.6739,
    2.7139,
]

avg_mpi = [
    0.0009,
    0.0016,
    0.0029,
    0.0070,
    0.0172,
    0.0548,
    0.1611,
    0.4924,
    1.8024,
]

avg_omp = [
    1.2103,
    1.9098,
    3.689,
    2.6312,
    4.5335,
    2.9939,
    3.2503,
    3.1223,
    5.4589,
]

avg_hybrid = [
    0.4379,
    2.2206,
    1.3052,
    2.3736,
    4.6641,
    7.728,
    11.3994,
    35.5157,
    25.0968,
]

plt.title('HiPerGator Port LCS Average Runtimes')
plt.ylabel('Execution time (sec)')
plt.xlabel('Sequence length')


plt.xscale('log', base=2)
plt.xticks(sizes, labels=sizes)

# record the data in the plot
plt.plot(sizes, avg_serial, markersize=3, label='Sequential', marker='o', color='royalblue')
plt.plot(sizes, avg_omp, markersize=3, label='OpenMP', marker='o', color='darkorange')
plt.plot(sizes, avg_mpi, markersize=3, label='MPI', marker='o', color='grey')
plt.plot(sizes, avg_hybrid, markersize=3, label='Hybrid', marker='o', color='gold')

plt.legend(loc='lower center', bbox_to_anchor=(0.5, -0.23),
          fancybox=False, shadow=False, ncol=5)

# interactive display
# plt.show()
# save to file (assuming being EXECUTED FROM ROOT OF PROJECT)
plt.savefig('./images/port-results.png', bbox_inches='tight')
