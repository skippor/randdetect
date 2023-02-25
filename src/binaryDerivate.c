#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../include/externs.h"
#include "../include/cephes.h"


int 	BinaryDerivative(int k, int n)
{
  int nRet = 0;
  int i = 0;
  int j = 0;
  int nSquare = 0x00000001;
  int nCount = 0;
  int nSum = 0;
  for (i =0; i < k; ++i)
  {
  	nSquare = 0x00000001 << (k+1);
	nCount = n/nSquare;
  	for (j =0 ; j < nCount; ++j)
  	{
  		epsilon[j*nSquare] = epsilon[j*nSquare] ^ epsilon[j*nSquare + nSquare];
  	}
  }

  char* ptr = malloc(nCount);
  if (!ptr)
  {
  	return nRet;
  }
  
  for (j =0 ; j < nCount; ++j)
  {
  	if (epsilon[j*nSquare])
  	{
  		ptr[j] = 1;
  	}
	else
	{
		ptr[j] = -1;
	}
  }

  for (j =0 ; j < nCount; ++j)
  {
  	nSum += ptr[j];
  }

  double v = fabs(nSum)/sqrt(n-k);
  double p_value = erfc(fabs(v)/sqrt(2.0));

	if (p_value < ALPHA)
	{
		nRet = 1;
	}
	return nRet;

  
}

