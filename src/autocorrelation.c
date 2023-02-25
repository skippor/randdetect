#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <string.h>
#include "../include/externs.h"
#include "../include/cephes.h"


 double g_accuracy = 0.000001;//����
 double g_standard_alpha = 0.01;//������ˮƽ
 int g_frequency_block_sub_sequence_length = 100;//Ƶ�����Ŀ��ڷ��ص������г���Ϊ100���ɹ������淶
 int g_run_block_sub_sequence_length = 10000;//���1���γ̼��Ŀ��ڷ��ص������г���Ϊ10000���ɹ������淶
 int g_byte_bits = 8;//һ���ֽ���8λ
 
extern BitSequence *g_fileCharStream; 

 /*
  * ��������:ͳ�����������Ʊ�ʾ��1�ĸ���
  * @param [IN]n ��ͳ�Ƶ�����
  * @return ͳ�ƽ��������1�ĸ���
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
  * ��������:���������������Ĵ�С�ȽϽ��
  * @param [IN]value_a	�����ı��ش�
  * @param [IN]value_b	���ش��ĳ���
  * @return  1 value_a > value_b
  * @return  0 value_a = value_b
  * @return  -1 value_a < value_b
  */
 static int double_compare(double value_a, double value_b)
 { 
	 double fDelta = value_a - value_b;
 
	 if( fDelta > g_accuracy )
	 {
		 return 1; //���ڣ�����1
	 }
	 else if( fDelta < g_accuracy )
	 {
		 return -1; //С�ڣ�����-1
	 }
	 return 0; //���ڣ�����0
 } 


 /*
 * ��������:�����ش��Ƿ�ͨ��������ؼ�⡱
 * @param [IN]buf  �����ı��ش�
 * @param [IN]count  ���ش��ĳ���
 * @param [IN]d_value �淶ȡ��dֵ
 * @param [IN]standard_alpha  ��׼ȷ��������ˮƽ��ֵ
 * @return  0 ���������ͨ����������ؼ��
 * @other 1 ���������ûͨ����������ؼ��
 */
int autocorrelation_check(const void *buf,size_t count,int d_value,double standard_alpha)
{
	if((buf == NULL) || (count <= 0) || (d_value <= 0))
	{
		return 1;
	}
	
	//���ش���1��0������
	long all_bit_count = (g_byte_bits * count);
	
	//����λ����λȡһ���ֽڵ��ض�һλ����ʱ��λ����ǰ����λ���ں�
	unsigned char bit_arr[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
	
	const unsigned char *buf_ptr = (unsigned char*)buf;
	
	//ad_value �� A(d)��ֵ
	int ad_value = 0;
	long i = 0;

	long index_ei = 0;
	long residue_ei = 0;

	long index_ei_d = d_value/g_byte_bits;
	long residue_ei_d = d_value%g_byte_bits;
	
	for(i = 0; i <= (all_bit_count-d_value-1); ++i)
	{
		unsigned char ei = 0;//��(i)
		unsigned char ei_d = 0;//��(i+d)

		//�������(i)��ֵ��1��0��
		if(residue_ei == g_byte_bits)
		{
			residue_ei = 0;
			++index_ei;
		}
		if(buf_ptr[index_ei] & bit_arr[residue_ei])
		{
			ei = 0x01;
		}
		
		//�������(i+d)��ֵ��1��0��
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
	double n_d = all_bit_count - d_value;//n-d��ֵ
	double statistic_value = 2*(ad_value - (n_d/2))/sqrt(n_d);
	//printf("V:%lf\n",statistic_value);
	double p_value = erfc(fabs(statistic_value) / sqrt(2));
	//printf("p_value:%lf\n",p_value);
	int cmp_result = double_compare(p_value,standard_alpha);
	//printf("cmp_result:%d\n",cmp_result);
	if(cmp_result >= 0)
	{//���ش�ͨ��������Ƶ�����
		return 0;
	}
	return 1;
}


int autocorrelation(int n)
{
	return autocorrelation_check(g_fileCharStream,(size_t)n,16,ALPHA);
}

