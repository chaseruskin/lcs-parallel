# Script: avg.py
#
# Computes the average of a series of space-delimited decimal numbers.
# Asks for a single line of input

# accept inputs
samples = input('Samples: ')
samples = [float(x) for x in samples.split()]
# compute the average
avg = 0.0
for x in samples: avg += x
avg = avg/len(samples)
# return outputs
print(round(avg, 4))