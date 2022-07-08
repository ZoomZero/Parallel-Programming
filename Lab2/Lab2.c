#include "Lab2.h"

int main(int argc, char* argv[])
{
	if (argc != 3)
  {
		printf("Usage: %s precision number_of_threads\n", argv[0]);
	}
  else
  {
		double prec = strtod(argv[1], NULL);
		unsigned int threadNum = strtoul(argv[2], NULL, 10);
    double result = integrate(0.001, 2, prec, threadNum);
		printf("Result = %lf\n", result);
	}
	return 0;
}

double integrate(double leftLim, double rightLim, double prec, unsigned int threadNum)
{
  pthread_t thrId[threadNum];
  ThrData thrData[threadNum];
  ThrBounds* thrBounds = (ThrBounds*)malloc(threadNum * sizeof(ThrBounds));
  int err = 0;
	double dx = (rightLim - leftLim) / threadNum;

	for (unsigned int i = 0; i < threadNum; i++)
	{
		thrBounds[i].leftBound = i*dx;
		thrBounds[i].rightBound = (i+1)*dx;
	}

  unsigned long steps = stepsNum(prec);
  for(unsigned int i = 0; i < threadNum; i++)
  {
    thrData[i].bounds = &thrBounds[i];
		thrData[i].n = steps;
    if ((err = pthread_create(&thrId[i], NULL, thrIntegrate, &thrData[i])))
    {
      printf("Error in creating thread: %d\n", err);
      exit(EXIT_FAILURE);
    }
  }

  double * resBuf = NULL;
  double sum = 0;
  for(unsigned int i = 0; i < threadNum; i++)
  {
    if ((err = pthread_join(thrId[i], (void**)&resBuf)))
    {
      printf("Error in joining thread: %d\n", err);
      exit(EXIT_FAILURE);
    }
    sum += *resBuf;
    free(resBuf);
  }

  return sum;
}

void * thrIntegrate(void * arg)
{
  ThrData* data = (ThrData*)arg;
  double * subSum = (double *)malloc(1*sizeof(double));
  double h = 0;

  h = (data->bounds->rightBound - data->bounds->leftBound) / data->n;
	for (unsigned long j = 0; j < data->n; j++)
	{
		*subSum += h * sinFunc(data->bounds->leftBound + j*h + h*0.5);
	}
  return (void*)subSum;
}

unsigned long int stepsNum(double prec)
{
  return (unsigned long int)(sqrt(1.69 / prec) + 1);
}

double sinFunc(double x)
{
  return sin(1.0 / x);
}
