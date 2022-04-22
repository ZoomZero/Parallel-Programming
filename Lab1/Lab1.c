#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

double f(double x, double t){
    return x+t;
}

double fi(double x){
    return x;
}

double ksi(double t){
    return t;
}


int main(int argc, char* argv[]){
    if (argc > 4)
    {
      int rank = 0, size = 0;
      double start_time = 0, finish_time = 0;

      int X = atoi(argv[3]), T = atoi(argv[4]);

      int M = atoi(argv[1]), K = atoi(argv[2]); //  M кратно числу процессов
      double h = 0, tau = 0; // шаги
      double a = 0; //константа из уравнения

      int k = 0, m = 0;
      int i = 0;

      MPI_Status status;

      MPI_Init(&argc, &argv);
      MPI_Comm_size(MPI_COMM_WORLD, &size);
      MPI_Comm_rank(MPI_COMM_WORLD, &rank);

      int n = M / size; // количество узлов, которые рассчитывает 1 процесс

      h = (double)X/M;
      tau = (double)T/K;

      double * u = (double*)malloc((M*K) * sizeof(double));
      double * x = (double*)malloc(M * sizeof(double));
      double * t = (double*)malloc(K * sizeof(double));

  //    задание сетки и начальных условий
      for (m = 0; m < M; m++)
      {
          x[m] = m * h;
          u[m] = fi(x[m]);
      }
      for (k = 0; k < K; k++)
      {
          t[k] = k * tau;
          u[k*M] = ksi(t[k]);
      }

  //  буфер, в котором каждый процесс считает свою часть на временном слое
      double * u_procs = (double*)malloc(n * sizeof(double));

  //    старт времени
      if(rank == 0) start_time = MPI_Wtime();

  // на каждом временном слое записываю данные на каждом процессоре в буфер u_procs и затем во всех процессах соединяю их в кажом временном слое u
      for (k = 1; k < K; k++){

  // разделил на все процессы вычисление точек по х, результат записывается в u_procs на каждом процессе
          for (m = 0; m < n; m++){
              if ((m + rank*n) == M-1)
                  u_procs[m] = u[(k-1)*M + rank*n + m] - tau/h*(u[(k-1)*M + rank*n + m] - u[(k-1)*M + rank*n + (m-1)]) + tau*f(x[m + rank*n], t[k-1]);
              else if ((m + rank*n) == 0)
                  u_procs[m] = u[k*M];
              else
                  u_procs[m] = 0.5*(1 - tau/h)*u[(k-1)*M + rank*n + (m+1)] + 0.5*(1 + tau/h)*u[(k-1)*M + rank*n + (m-1)] + tau*f(x[m + rank*n], t[k-1]);
          }
  //  в каждом процессе собирается полный временной слой
          MPI_Allgather(u_procs, n, MPI_DOUBLE, (u + k*M), n, MPI_DOUBLE, MPI_COMM_WORLD);
      }

  //    в главном процессе выводится результат
      if (rank == 0){
          finish_time = MPI_Wtime();

          /*FILE * f_dot = fopen("log.txt", "w+");
          assert(f_dot);
          for(int i = 0; i < K; i++)
          {
            for(int m = 0; m < M; m++) fprintf(f_dot, "%f\t| ", u[i*M + m]);
          }
          fclose(f_dot);*/
          if (M*K <= 10000)
          {
            FILE * f_dot = fopen("logs.txt", "w+");
            assert(f_dot);
            fprintf(f_dot, "%d %d \n", M, K);
            for(int i = 0; i < K; i++)
            {
              for(int m = 0; m < M; m++) fprintf(f_dot, "%f ", u[i*M + m]);
              fprintf(f_dot, "\n");
            }
            fclose(f_dot);
          }

          float time = (float)(finish_time - start_time)*1000;

          FILE * f_time = fopen("time.txt", "w+");
          assert(f_time);
          fprintf(f_time, "%f", time);
          fclose(f_time);

          //printf("u = %f \n", u[(K-1)*M + M-1]);
          //printf("Time: %f \n", time);
          //printf("\n");
      }


      free(u);
      free(x);
      free(t);
      free(u_procs);

      fflush(stdout);
      MPI_Finalize();
    }
    else printf("Enter M, K, X, T\n");

    return 0;
}
