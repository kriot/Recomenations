import recommender
import logging
import click

def learn_recommender(data, users, items, dimensions):
    r = recommender.Recommender(users, items, dimensions)
    trainer = ConjugateGradient(r, data, 
        step = 0.0002,
        regularization = 0.01,
        iterations = 30)
    trainer.train()
    return r

class Data():
    def __init__(self, k, U, M, D, T):
        self.users_size = U
        self.items_size = M
        self.train = []
        self.test = []
    @staticmethod
    def get_simple_data(stream):
        it = iter(stream)
        k, U, M, D, T = map(int, next(it).split(' '))
        data = Data(k, U, M, D, T)
        for _ in xrange(D):
            data.train.append(next(it).split(' '))
        for _ in xrange(T):
            data.test.append(next(it).split(' '))
        return data
@click.command()
@click.option('--test_in', default = 'test.in')
@click.option('--test_out', default = 'test.out')
@click.option('--out', default = 'out.out')
def main(test_in, test_out, out):
    with open(test_in) as fin:
        data = Data.get_simple_data(fin)
    
    r = learn_recommender(data.train, data.users_size, data.items_size, 10)
    with open(test_out) as true_out, open(out, 'w') as out:
        true_resulst = iter(true_out)
        mse = 0
        for uid, iid in data.test:
            res = r.predict(uid, iid)
            out.write('{}\n'.format(res))
            mse += (next(true_resulst) - res) ** 2
        logging.info('mse: {}'.format(mse))

if __name__ == '__main__':
    main()
