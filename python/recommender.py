import numpy as np

class Recommender(object):
    def __init__(self, UsersSize, ItemSize, Dimension):
        self.users = np.zeros((UsersSize, Dimension))
        self.items = np.zeros((Dimension, ItemSize))
        self.d = Dimension

    def predict(self, uid, iid):
        return np.dot(self.users[uid, :], self.items[:, iid])
