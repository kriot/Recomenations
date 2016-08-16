#!/bin/python
import numpy as np
import random
mu = 5.8
U = 16000
M = 1000
D = M * U / 5
T = 3000
dim = 10

k = 10

a = -5.
b = 5.

var = 0.0;

users = np.array([np.array([0.] * dim)] * U)
items = np.array([np.array([0.] * dim)] * M)

for user in users:
	user[0] = mu
	user[1] = 1.
	for i in range(2, dim):
		user[i] = random.uniform(a, b)

for item in items:
	item[0] = 1.
	item[1] = random.uniform(a, b)
	item[2] = 1.
	for i in range(3, dim):
		item[i] = random.uniform(a, b)

with open("test.in", "w") as fin:
	fin.write("{} {} {} {} {}\n".format(k, U, M, D, T))
	for _ in range(D):
		u = random.randint(0, U - 1)
		m = random.randint(0, M - 1)
		r = np.inner(users[u], items[m]) + random.uniform(-var, var)
		fin.write("{} {} {}\n" .format(u, m, r))
	with open("test.out", "w") as fout:
		for _ in range(T):
			u = random.randint(0, U - 1)
			m = random.randint(0, M - 1)
			r = np.inner(users[u], items[m])
			fin.write("{} {}\n".format(u, m))
			fout.write("{}\n".format(r))


