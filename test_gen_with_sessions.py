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
sessions_per_user = 1
M = 1000
D = M * U / 5 / sessions_per_user
T = 3000
dim = 10

k = 10

sigma_gen = float(options.sigma_gen)
sigma_session = float(options.sigma_session)

noise = float(options.noise);
print noise

users = np.array([np.array([0.] * dim)] * U)
sessions = np.array([np.array([0.] * dim)] * U * sessions_per_user)
items = np.array([np.array([0.] * dim)] * M)

for user in users:
	user[0] = mu
	user[1] = 1.
	for i in range(2, dim):
		user[i] = random.gauss(0., sigma_gen)
        for sid in xrange(i * sessions_per_user, (i + 1) * sessions_per_user):
            sessions[sid][0] = mu
            sessions[sid][1] = 1.
            for i in range(2, dim):
                sessions[sid][i] = random.gauss(user[i], sigma_session)

for item in items:
	item[0] = 1.
	item[1] = random.gauss(0., sigma_gen)
	item[2] = 1.
	for i in range(3, dim):
		item[i] = random.gauss(0., sigma_gen)

with open("test.in", "w") as fin:
	fin.write("{} {} {} {} {}\n".format(U, U * sessions_per_user, M, D * sessions_per_user, T))
	for _ in range(D):
		u = random.randint(0, U - 1)
                for s in xrange(sessions_per_user):
                    m = random.randint(0, M - 1)
                    noise_error = random.gauss(0., noise)
                    sid = u * sessions_per_user + s
                    r = np.inner(sessions[sid], items[m]) + noise_error 
                    fin.write("{} {} {} {}\n" .format(u, sid, m, r))
	with open("test.out", "w") as fout:
		for _ in range(T):
			u = random.randint(0, U - 1)
			m = random.randint(0, M - 1)
                        noise_error = random.gauss(1., noise)  # not used
			r = np.inner(users[u], items[m]) 
			fin.write("{} {}\n".format(u, m))
			fout.write("{}\n".format(r))


