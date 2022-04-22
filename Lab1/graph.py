import numpy as np
import matplotlib.pyplot as plt
import os
from time import sleep


def time_graph(n_procs):
    t = []
    for i in range(1, n_procs+1):
        os.system('mpirun -np %d a.out 36000 5000 4000 10' % i)
        sleep(10)
        f_time = open('time.txt', 'r')
        t.append(float(f_time.readline()))
    t_res = [t[0] / i for i in t]
    n = np.linspace(1, n_procs, n_procs)
    plt.plot(n, t_res)
    plt.xlabel('кол-во процессов')
    plt.ylabel('ускорение')
    plt.savefig('time.png')


def main():
    os.system('mpicc Lab1.c')
    time_graph(4)

    os.system('mpirun -np 3 a.out 120 50 10 1')
    sleep(3)

    f = open('logs.txt', 'r')

    line = f.readline().split(' ')
    M = int(line[0])
    K = int(line[1])

    m = np.linspace(0, M-1, M)
    k = np.linspace(0, K-1, K)

    u_ = []
    u = []
    for line in f:
        new_line = line.split(' ')
        u_ = []
        for i in new_line[:-1]:
            u_.append(float(i))
        u.append(u_)
    f.close()

    u = np.array(u)
    plt.matshow(u)
    plt.savefig('graph2d.png')

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    X, Y = np.meshgrid(m, k)
    ax.plot_surface(X, Y, u, rstride=1, cstride=1, cmap='viridis')
    ax.view_init(0, 20)
    ax.set_xlabel('m - шаги по х')
    ax.set_ylabel('k - шаги по времени')
    ax.set_zlabel('u')
    plt.savefig('graph3d.png')


if __name__ == '__main__':
    main()
