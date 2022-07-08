#ifndef LAB2_H
#define LAB2_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

const double PI = acos(-1.0);

typedef struct
{
  double leftBound;
  double rightBound;
} ThrBounds;

typedef struct
{
  ThrBounds * bounds;
  unsigned long n;
} ThrData;

double integrate(double leftLim, double rightLim, double prec, unsigned int threadNum);
void * thrIntegrate(void * arg);
unsigned long stepsNum(double prec);
double sinFunc(double x);

#endif
