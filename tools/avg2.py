# Script: avg2.py
#
# Computes the average of a series of runtime trials from the raw text log copied
# into the `RAW` Python variable.

# copy/paste a snippet of the log runtimes.txt results to format and compute stats
RAW: str = '''
--------- 32768 : 2024-04-09 17:21:09 ---------
total time taken: 6.336605
total time taken: 16.010738
total time taken: 20.323807
total time taken: 4.919467
total time taken: 1.419508
total time taken: 1.542067
total time taken: 1.048408
total time taken: 2.039756
total time taken: 2.150442
total time taken: 13.527157
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