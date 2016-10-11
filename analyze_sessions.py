
#@click.command()
def main():
    with open('test.in') as fin:
        U, S, M, D, T = map(int, fin.readline().split(' '))
            
        sessions = {}
        for _ in range(D):
            line = fin.readline()
            u, s, i, r = map(float, line.split(' '))
            if u not in sessions:
                sessions[u] = {}
            if s not in sessions[u]:
                sessions[u][s] = []
            sessions[u][s].append(i)

    count_users = len(sessions)
    print('Users had sessions: {}'.format(float(count_users) / U))
    count_sessions = 0
    count_items = 0
    count_sessions_per_user = [0] * 1000
    for uid, user in sessions.items():
        count_sessions += len(user)
        count_sessions_per_user[len(user)] += 1
        for sid, session in user.items():
            count_items += len(session)

    print('Sessions per user: {}'.format(float(count_sessions) / count_users))
    print('Items per session: {}'.format(float(count_items) / count_sessions))
    print(count_sessions_per_user)
main()
