#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <string.h>
#include "../include/externs.h"
#include "../include/cephes.h"


 double g_accuracy = 0.000001;//精度
 double g_standard_alpha = 0.01;//显著性水平
 int g_frequency_block_sub_sequence_length = 100;//频数检测的块内非重叠子序列长度为100，由国密来规范
 int g_run_block_sub_sequence_length = 10000;//最大“1”游程检测的块内非重叠子序列长度为10000，由国密来规范
 int g_byte_bits = 8;//一个字节有8位
 
extern BitSequence *g_fileCharStream; 

 /*
  * 函数功能:统计整数二进制表示中1的个数
  * @param [IN]n 待统计的整数
  * @return 统计结果，返回1的个数
  */
 static unsigned int get_one_count(unsigned int n)
 {
	  n = (n&0x55555555) + ((n>>1)&0x55555555);
	  n = (n&0x33333333) + ((n>>2)&0x33333333);
	  n = (n&0x0f0f0f0f) + ((n>>4)&0x0f0f0f0f);
	  n = (n&0x00ff00ff) + ((n>>8)&0x00ff00ff);
	  n = (n&0x0000ffff) + ((n>>16)&0x0000ffff);
	  return n;
 } 
 
 /*
  * 函数功能:返回连个浮点数的大小比较结果
  * @param [IN]value_a	待检测的比特串
  * @param [IN]value_b	比特串的长度
  * @return  1 value_a > value_b
  * @return  0 value_a = value_b
  * @return  -1 value_a < value_b
  */
 static int double_compare(double value_a, double value_b)
 { 
	 double fDelta = value_a - value_b;
 
	 if( fDelta > g_accuracy )
	 {
		 return 1; //大于，返回1
	 }
	 else if( fDelta < g_accuracy )
	 {
		 return -1; //小于，返回-1
	 }
	 return 0; //等于，返回0
 } 


 /*
 * 函数功能:检测比特串是否通过“自相关检测”
 * @param [IN]buf  待检测的比特串
 * @param [IN]count  比特串的长度
 * @param [IN]d_value 规范取的d值
 * @param [IN]standard_alpha  标准确定的显著水平α值
 * @return  0 待检测序列通过块内自相关检测
 * @other 1 待检测序列没通过块内自相关检测
 */
int autocorrelation_check(const void *buf,size_t count,int d_value,double standard_alpha)
{
	if((buf == NULL) || (count <= 0) || (d_value <= 0))
	{
		return 1;
	}
	
	//比特串的1和0的总数
	long all_bit_count = (g_byte_bits * count);
	
	//按高位到低位取一个字节的特定一位，此时高位排在前，低位排在后
	unsigned char bit_arr[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
	
	const unsigned char *buf_ptr = (unsigned char*)buf;
	
	//ad_value 是 A(d)的值
	int ad_value = 0;
	long i = 0;

	long index_ei = 0;
	long residue_ei = 0;

	long index_ei_d = d_value/g_byte_bits;
	long residue_ei_d = d_value%g_byte_bits;
	
	for(i = 0; i <= (all_bit_count-d_value-1); ++i)
	{
		unsigned char ei = 0;//ε(i)
		unsigned char ei_d = 0;//ε(i+d)

		//计算出ε(i)的值，1或0。
		if(residue_ei == g_byte_bits)
		{
			residue_ei = 0;
			++index_ei;
		}
		if(buf_ptr[index_ei] & bit_arr[residue_ei])
		{
			ei = 0x01;
		}
		
		//计算出ε(i+d)的值，1或0。
		if(residue_ei_d == g_byte_bits)
		{
			residue_ei_d = 0;
			++index_ei_d;
		}
		if(buf_ptr[index_ei_d] & bit_arr[residue_ei_d])
		{
			ei_d = 0x01;
		}
		ad_value += (ei ^ ei_d);
		++residue_ei;
		++residue_ei_d;
	}
	//printf("A(%d):%d\n",d_value,ad_value);
	double n_d = all_bit_count - d_value;//n-d的值
	double statistic_value = 2*(ad_value - (n_d/2))/sqrt(n_d);
	//printf("V:%lf\n",statistic_value);
	double p_value = erfc(fabs(statistic_value) / sqrt(2));
	//printf("p_value:%lf\n",p_value);
	int cmp_result = double_compare(p_value,standard_alpha);
	//printf("cmp_result:%d\n",cmp_result);
	if(cmp_result >= 0)
	{//比特串通过单比特频数检测
		return 0;
	}
	return 1;
}


int autocorrelation(int n)
{
	return autocorrelation_check(g_fileCharStream,(size_t)n,16,ALPHA);
}

