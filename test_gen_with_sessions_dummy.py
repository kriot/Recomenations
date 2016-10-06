#!/bin/python
import numpy as np
import random
from optparse import OptionParser, OptionError

parser = OptionParser()
parser.add_option('--mu',
	action = 'store',        
	dest = 'mu',       
	default = '5.8',        
	help = '') 
parser.add_option('--sigma_gen',
	action = 'store',        
	dest = 'sigma_gen',       
	default = '1.0',        
	help = '') 
parser.add_option('--noise',
	action = 'store',        
	dest = 'noise',       
	default = '0.1',        
	help = '') 
(options, args) = parser.parse_args()

mu = float(options.mu)
U = 16000
M = 1000
D = M * U / 5
T = 3000
dim = 10

k = 10

sigma_gen = float(options.sigma_gen)

noise = float(options.noise);
print noise

users = np.array([np.array([0.] * dim)] * U)
items = np.array([np.array([0.] * dim)] * M)

for user in users:
	user[0] = mu
	user[1] = 1.
	for i in range(2, dim):
		user[i] = random.gauss(0., sigma_gen)

for item in items:
	item[0] = 1.
	item[1] = random.gauss(0., sigma_gen)
	item[2] = 1.
	for i in range(3, dim):
		item[i] = random.gauss(0., sigma_gen)

with open("test.in", "w") as fin:
	fin.write("{} {} {} {} {}\n".format(U, U, M, D, T))
	for _ in range(D):
		u = random.randint(0, U - 1)
		m = random.randint(0, M - 1)
                noise_error = random.gauss(0., noise)
		r = np.inner(users[u], items[m]) + noise_error 
		fin.write("{} {} {} {}\n" .format(u, u, m, r))
	with open("test.out", "w") as fout:
		for _ in range(T):
			u = random.randint(0, U - 1)
			m = random.randint(0, M - 1)
                        noise_error = random.gauss(0., noise)
			r = np.inner(users[u], items[m]) 
			fin.write("{} {}\n".format(u, m))
			fout.write("{}\n".format(r))


