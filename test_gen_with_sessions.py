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
parser.add_option('--sigma_session',
	action = 'store',        
	dest = 'sigma_session',       
	default = '1.0',        
	help = 'sigma for distribution session round user') 
parser.add_option('--noise',
	action = 'store',        
	dest = 'noise',       
	default = '0.1',        
	help = '') 
(options, args) = parser.parse_args()

mu = float(options.mu)
U = 16000
sessions_per_user_max = 10
sessions_per_user_min = 3

M = 1000
D = M * U / 5 
T = 3000
dim = 10

k = 10

sigma_gen = float(options.sigma_gen)
sigma_session = float(options.sigma_session)

noise = float(options.noise);
print noise

users = np.array([np.array([0.] * dim)] * U)
sessions = [ [np.array([0.] * dim) for _ in xrange(random.randint(sessions_per_user_min, sessions_per_user_max))] for _ in xrange(U)] # [uid][sid]
items = np.array([np.array([0.] * dim)] * M)

# generate users and sessions

S = 0
for uid, user in enumerate(users):
	user[0] = mu
	user[1] = 1.
	for i in range(2, dim):
		user[i] = random.gauss(0., sigma_gen)
        for session in sessions[uid]:
            S += 1
            session[0] = mu
            session[1] = 1.
            for i in range(2, dim):
                session[i] = random.gauss(user[i], sigma_session)

# generate items

for item in items:
	item[0] = 1.
	item[1] = random.gauss(0., sigma_gen)
	item[2] = 1.
	for i in range(3, dim):
		item[i] = random.gauss(0., sigma_gen)

# generate test

with open("test.in", "w") as fin:
	fin.write("{} {} {} {} {}\n".format(U, S, M, D, T))
	for _ in range(D):
                # session may hold more than one item, because we mat retake (u, s)
		u = random.randint(0, U - 1)
                s = random.randint(0, len(sessions[u]) - 1)
                m = random.randint(0, M - 1)
                noise_error = random.gauss(0., noise)
                r = np.inner(sessions[u][s], items[m]) + noise_error 
                fin.write("{} {} {} {}\n" .format(u, s, m, r))
	with open("test.out", "w") as fout:
		for _ in range(T):
			u = random.randint(0, U - 1)
			m = random.randint(0, M - 1)
                        noise_error = random.gauss(1., noise)  # not used
			r = np.inner(users[u], items[m]) 
			fin.write("{} {}\n".format(u, m))
			fout.write("{}\n".format(r))


