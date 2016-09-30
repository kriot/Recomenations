import numpy as np

with open('out.log') as out:
    a1 = np.array(map(lambda line: float(line.strip()), out))

with open('test.out') as out:
    a2 = np.array(map(lambda line: float(line.strip()), out))

def cov(a, b):
    return (((a - a.mean()) * (b - b.mean())) ** 2).sum()

print cov(a1, a2) / (cov(a1, a1) * cov(a2, a2)) **0.5
