# Script: avg2.py
#
# Computes the average of a series of runtime trials from the raw text log copied
# into the `RAW` Python variable.

# copy/paste a snippet of the log runtimes.txt results to format and compute stats
RAW: str = '''
---------  128 : 2024-04-09 21:42:44 ---------
time taken for lcs_yang_v1 is: 0.019729
time taken for lcs_yang_v1 is: 0.022177
time taken for lcs_yang_v1 is: 0.038230
time taken for lcs_yang_v1 is: 0.056542
time taken for lcs_yang_v1 is: 0.048627
time taken for lcs_yang_v1 is: 0.032777
time taken for lcs_yang_v1 is: 0.053743
time taken for lcs_yang_v1 is: 0.044773
time taken for lcs_yang_v1 is: 0.028842
time taken for lcs_yang_v1 is: 0.046976
'''

# separate into lines
lines = RAW.splitlines(keepends=False)
samples = []
for l in lines:
    if len(l) == 0 or l.startswith('-'):
        continue
    samples += [float(l.rsplit(' ')[-1])]
    pass

# display the samples in nice format
print('SAMPLES: ')
for s in samples:
    print(s, end=' ')
print()

# display the average time of all trials
print('AVERAGE: ')
avg = 0.0
for x in samples: avg += x
avg = avg/len(samples)
print(round(avg, 4))