#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "../include/externs.h"
#include "../include/cephes.h"

int getK(int n, double* ptr)
{
	int i = 0;
	double e = 0;
	int flag =1;
	int nRet = 0;
	for (i=1; i<= n; ++i)
	{
		int nSqueue = 2 << (i+2);
		e = (n - i + 3)/nSqueue ;
		ptr[i-1]=e;
		if (e < 5)
		{
			//上一个值就是大于等于5
			ptr[i-1]=0;
			return i - 1;
		}
	}
}

int getEG(int n,int *ptrb,int* ptrg)
{
	int flag = 0;
	int i=0;
	int count = 0;
	
	if (epsilon[0])
	{
		flag = 1;
	}
	
	for (i=0;i<n;++i)
	{
		int nNowFlag = epsilon[i];
		if (nNowFlag == flag)
		{
			++count;
		}
		else
		{
			if(flag)
			{
				ptrb[count] = ptrb[count]+1;
			}
			else
			{
				ptrg[count] = ptrg[count]+1;
			}
			count = 0;
			flag = epsilon[i];
		}
	}
	return 0;
}

double getV(int k,double* ptre,int* ptrb,int* ptrg)
{
	int i = 0;
	double v = 0;
	for (i = 0;i<k;++i)
	{
		double d1 = (ptrb[i+1] - ptre[i])*(ptrb[i+1] - ptre[i]);
		double d2 = (ptrg[i+1] - ptre[i])*(ptrg[i+1] - ptre[i]);
		double tmp = (d1+d2)/ptre[i];
		v +=tmp; 
	}
	return v;
}

int		RunsDistribution(int n)
{
	int i = 0;
	int nRet = 0;
	//int 最大32位
	double *ptre = (double *)malloc(32*sizeof(double));
	int k = getK(n,ptre);
	

	int *ptrb = (int *)malloc(n*sizeof(int));
	memset(ptrb,0,n*sizeof(int));
	int *ptrg = (int *)malloc(n*sizeof(int));
	memset(ptrg,0,n*sizeof(int));
	getEG(n,ptrb,ptrg);
	

	double v = getV(k,ptre,ptrb,ptrg);

	double p_value = cephes_igamc((double)(k-1),v/2);

#if 0
	for (i=0;i<k;++i)
	{
		printf("e=%f,b=%d,g=%d\n",ptre[i],ptrb[i],ptrg[i]);
	}
	printf("RunsDistribution k=%d v=%f p_value=%f\n",k,v,p_value);
#endif

	if (p_value < ALPHA)
	{
		nRet = 1;
	}

	return nRet;
	
}

