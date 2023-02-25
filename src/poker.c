#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../include/externs.h"
#include "../include/cephes.h"

extern BitSequence *g_fileCharStream;

inline unsigned char getBufValue(unsigned int m,unsigned int j)
{
	unsigned int n = (j/8)*8;
	unsigned int k = j%8;
	unsigned char ucValue = g_fileCharStream[n/8];
	unsigned char tmp = ucValue << k;
	ucValue = tmp >> 8 -m; 
	return ucValue;
}



int Poker(int m, int n)
{
	double x;
	int nRet = 0;
	unsigned int uComNum = 1<<m;//多少种组合
	unsigned int uCount	= n/uComNum;
	unsigned int* pn = (unsigned int*)malloc(uComNum*sizeof(int));
	if(!pn)
	{
		return 0;
	}
	memset(pn,0,uComNum*sizeof(int));
	
	double a=0;
	unsigned int i = 0;
	unsigned int k = 0;
	unsigned int j = 0;
	//unsigned int n = 0;

	for(i=0; i< uCount; i++)
	{
		j=i*m;
		//k = 0;
		k = getBufValue(m,j);
		*(pn+k) = *(pn+k) + 1;
	}

	for(i=0; i<uComNum; i++)
	{
		a += pow(pn[i], 2);
		
    }

	free(pn);    
	x = a*uComNum/uCount - uCount;

	if(x>1.03 && x<57.4)
		return 0;
	else
		return 1;	
}


