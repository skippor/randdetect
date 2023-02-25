/* --------------------------------------------------------------------------
   Title       :  The NIST Statistical Test Suite

   Date        :  December 1999

   Programmer  :  Juan Soto

   Summary     :  For use in the evaluation of the randomness of bitstreams
                  produced by cryptographic random number generators.

   Package     :  Version 1.0

   Copyright   :  (c) 1999 by the National Institute Of Standards & Technology

   History     :  Version 1.0 by J. Soto, October 1999
                  Revised by J. Soto, November 1999
                  Revised by Larry Bassham, March 2008

   Keywords    :  Pseudorandom Number Generator (PRNG), Randomness, Statistical 
                  Tests, Complementary Error functions, Incomplete Gamma 
                  Function, Random Walks, Rank, Fast Fourier Transform, 
                  Template, Cryptographically Secure PRNG (CSPRNG),
                  Approximate Entropy (ApEn), Secure Hash Algorithm (SHA-1), 
                  Blum-Blum-Shub (BBS) CSPRNG, Micali-Schnorr (MS) CSPRNG, 

   Source      :  David Banks, Elaine Barker, James Dray, Allen Heckert, 
                  Stefan Leigh, Mark Levenson, James Nechvatal, Andrew Rukhin, 
                  Miles Smid, Juan Soto, Mark Vangel, and San Vo.

   Technical
   Assistance  :  Larry Bassham, Ron Boisvert, James Filliben, Daniel Lozier,
                  and Bert Rust.

   Warning     :  Portability Issues.

   Limitation  :  Amount of memory allocated for workspace.

   Restrictions:  Permission to use, copy, and modify this software without 
                  fee is hereby granted, provided that this entire notice is 
                  included in all copies of any software which is or includes
                  a copy or modification of this software and in all copies 
                  of the supporting documentation for such software.
   -------------------------------------------------------------------------- */
#if 1
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../include/decls.h"
#include "../include/cephes.h"  
#include "../include/utilities.h"
#include "../include/stat_fncs.h"

void	partitionResultFile(int numOfFiles, int numOfSequences, int option, int testNameID);
void	postProcessResults(int option);
int		cmp(const double *a, const double *b);
int		computeMetrics(char *s, int test);

#define MAX_DETECT_LEN 4000
#define DETECT_LEN  1000
#define MAX_FILENAME_LEN 256
#define BLOCK_LEN 20

BitSequence	*g_epsilon;
char g_fileName[MAX_FILENAME_LEN];

FILE * g_resfile;


extern int g_filesize;
extern BitSequence *g_fileBitStream;

void ReadRandData()
{

}

int initSystem()
{
	testVector[TEST_CUSUM] = 1;
	//离散傅立叶检测
	testVector[TEST_FFT] = 1;
	//单比特频数检测
	testVector[TEST_FREQUENCY]=1;
	//块频数检测
	testVector[TEST_BLOCK_FREQUENCY] = 1;

	//2.15
	testVector[TEST_SERIAL] = 1;

	//2.16 游程总数统计
	testVector[TEST_RUNS] = 1;


	//2.18  快内最大1游程检测
	testVector[TEST_LONGEST_RUN] = 1;

	//2.23  11
	testVector[TEST_APEN] = 1;

	//2.24  15
	testVector[TEST_LINEARCOMPLEXITY] = 1;
	
	//2.25
	testVector[TEST_UNIVERSAL] = 1;

	//2.21[TEST_RANK]
	testVector[TEST_RANK] = 1;
	
	readRandBinary(g_fileName);
	openOutputStreams(0);

	
	return 0;
}

int  rndDetect()
{
	T_LONGLONG timebefore;
	T_LONGLONG timeafter;
	int cusumRet = 0;
	int nFFTRet = 0;
	int nFrequencyRet = 0;
	int nBlockFreRet = 0;
	int nRunsRet = 0; 
	int nLongestRunsRet = 0;
	int nApenrRet = 0;
	int nLinearRet = 0;
	int nTestRet = 0;
	
	int nRet = 0;
	
	g_epsilon = (BitSequence*)malloc(g_filesize*8*sizeof(char));
	memcpy(g_epsilon,g_fileBitStream,g_filesize*8*sizeof(char));
	epsilon = g_epsilon;
	timebefore = getTickCount();
	cusumRet = CumulativeSums(g_filesize*8);
	timeafter = getTickCount();
	nRet += cusumRet;
	printf("CumulativeSums time:%lld ret:%d \n",timeafter - timebefore, cusumRet);

	memcpy(g_epsilon,g_fileBitStream,g_filesize*8*sizeof(char));
	timebefore = getTickCount();
	nFFTRet = DiscreteFourierTransform(g_filesize*8);
	timeafter = getTickCount();
	nRet += nFFTRet;
	printf("DiscreteFourierTransform time:%lld ret:%d \n",timeafter - timebefore, nFFTRet);

	//单比特频数检测  1
	memcpy(g_epsilon,g_fileBitStream,g_filesize*8*sizeof(char));
	timebefore = getTickCount();
	nFrequencyRet = Frequency(g_filesize*8);
	timeafter = getTickCount();
	nRet += nFrequencyRet;
	printf("Frequency time:%lld ret:%d \n",timeafter - timebefore, nFrequencyRet);

	//块频数检测  2
	memcpy(g_epsilon,g_fileBitStream,g_filesize*8*sizeof(char));
	timebefore = getTickCount();
	nBlockFreRet = BlockFrequency(BLOCK_LEN, g_filesize*8);
	timeafter = getTickCount();
	nRet += nBlockFreRet;
	printf("BlockFrequency time:%lld ret:%d \n",timeafter - timebefore, nBlockFreRet);	

	//2.15 
	memcpy(g_epsilon,g_fileBitStream,g_filesize*8*sizeof(char));
	timebefore = getTickCount();
	nTestRet = Serial(2,g_filesize*8);
	timeafter = getTickCount();
	nRet += nLinearRet;
	printf("Serial time:%lld ret:%d \n",timeafter - timebefore, nTestRet);
	

	//游程总数统计 2.16  4
	memcpy(g_epsilon,g_fileBitStream,g_filesize*8*sizeof(char));
	timebefore = getTickCount();
	nRunsRet = Runs(g_filesize*8);
	timeafter = getTickCount();
	nRet += nRunsRet;
	printf("Runs time:%lld ret:%d \n",timeafter - timebefore, nRunsRet);		

	//2.18  5
	memcpy(g_epsilon,g_fileBitStream,g_filesize*8*sizeof(char));
	timebefore = getTickCount();
	nLongestRunsRet = LongestRunOfOnes(g_filesize*8);
	timeafter = getTickCount();
	nRet += nLongestRunsRet;
	printf("LongestRunOfOnes time:%lld ret:%d \n",timeafter - timebefore, nLongestRunsRet);


	//2.20  6  二元矩阵
	memcpy(g_epsilon,g_fileBitStream,g_filesize*8*sizeof(char));
	timebefore = getTickCount();
	nTestRet = Rank(g_filesize*8);
	timeafter = getTickCount();
	nRet += nTestRet;
	printf("Rank binary matrix  time:%lld ret:%d \n",timeafter - timebefore, nTestRet);


	//2.23 11
	memcpy(g_epsilon,g_fileBitStream,g_filesize*8*sizeof(char));
	timebefore = getTickCount();
	nApenrRet = ApproximateEntropy(5,g_filesize*8);
	timeafter = getTickCount();
	nRet += nApenrRet;
	printf("ApproximateEntropy time:%lld ret:%d \n",timeafter - timebefore, nApenrRet);


	//2.24 15
	memcpy(g_epsilon,g_fileBitStream,g_filesize*8*sizeof(char));
	timebefore = getTickCount();
	nLinearRet = LinearComplexity(500, g_filesize*8);
	timeafter = getTickCount();
	nRet += nLinearRet;
	printf("LinearComplexity time:%lld ret:%d \n",timeafter - timebefore, nLinearRet);

	//2.25 10 
	memcpy(g_epsilon,g_fileBitStream,g_filesize*8*sizeof(char));
	timebefore = getTickCount();
	nTestRet = Universal(g_filesize*8);
	timeafter = getTickCount();
	nRet += nTestRet;
	printf("Universal time:%lld ret:%d \n",timeafter - timebefore, nTestRet);


	//2.19  16
	memcpy(g_epsilon,g_fileBitStream,g_filesize*8*sizeof(char));
	timebefore = getTickCount();
	nTestRet = BinaryDerivative(3,g_filesize*8);
	timeafter = getTickCount();
	nRet += nTestRet;
	printf("BinaryDerivative time:%lld ret:%d \n",timeafter - timebefore, nTestRet);	

	//2.14 扑克检测
	timebefore = getTickCount();
	nTestRet = Poker(4,g_filesize*8);
	timeafter = getTickCount();
	nRet += nTestRet;
	printf("Poker time:%lld ret:%d \n",timeafter - timebefore, nTestRet);


	//2.20 自相关检测
	timebefore = getTickCount();
	nTestRet = autocorrelation(g_filesize);
	timeafter = getTickCount();
	nRet += nTestRet;
	printf("autocorrelation time:%lld ret:%d \n",timeafter - timebefore, nTestRet);

	memcpy(g_epsilon,g_fileBitStream,g_filesize*8*sizeof(char));
	timebefore = getTickCount();
	nTestRet = RunsDistribution(g_filesize*8);
	timeafter = getTickCount();
	nRet += nTestRet;
	printf("RunsDistribution time:%lld ret:%d \n",timeafter - timebefore, nTestRet);
	return nRet;
}


int main(int argc, char *argv[])
{
#if 0
	int		i;
	int		option;			/* TEMPLATE LENGTH/STREAM LENGTH/GENERATOR*/
	char	*streamFile;	/* STREAM FILENAME     */
	

	if ( argc != 2 ) {
		printf("Usage: %s <stream length>\n", argv[0]);
		printf("   <stream length> is the length of the individual bit stream(s) to be processed\n");
		return 0;
	}

	tp.n = atoi(argv[1]);
	tp.blockFrequencyBlockLength = 128;
	tp.nonOverlappingTemplateBlockLength = 9;
	tp.overlappingTemplateBlockLength = 9;
	tp.approximateEntropyBlockLength = 10;
	tp.serialBlockLength = 16;
	tp.linearComplexitySequenceLength = 500;
	tp.numOfBitStreams = 1;
	option = generatorOptions(&streamFile);
	chooseTests();
	fixParameters();
	openOutputStreams(option);
	invokeTestSuite(option, streamFile);
	fclose(freqfp);
	for( i=1; i<=NUMOFTESTS; i++ ) {
		if ( stats[i] != NULL )
			fclose(stats[i]);
		if ( results[i] != NULL )
			fclose(results[i]);
	}
	if ( (testVector[0] == 1) || (testVector[TEST_CUSUM] == 1) ) 
		partitionResultFile(2, tp.numOfBitStreams, option, TEST_CUSUM);
	if ( (testVector[0] == 1) || (testVector[TEST_NONPERIODIC] == 1) ) 
		partitionResultFile(MAXNUMOFTEMPLATES, tp.numOfBitStreams, option, TEST_NONPERIODIC);
	if ( (testVector[0] == 1) || (testVector[TEST_RND_EXCURSION] == 1) )
		partitionResultFile(8, tp.numOfBitStreams, option, TEST_RND_EXCURSION);
	if ( (testVector[0] == 1) || (testVector[TEST_RND_EXCURSION_VAR] == 1) )
		partitionResultFile(18, tp.numOfBitStreams, option, TEST_RND_EXCURSION_VAR);
	if ( (testVector[0] == 1) || (testVector[TEST_SERIAL] == 1) )
		partitionResultFile(2, tp.numOfBitStreams, option, TEST_SERIAL);
	fprintf(summary, "------------------------------------------------------------------------------\n");
	fprintf(summary, "RESULTS FOR THE UNIFORMITY OF P-VALUES AND THE PROPORTION OF PASSING SEQUENCES\n");
	fprintf(summary, "------------------------------------------------------------------------------\n");
	fprintf(summary, "   generator is <%s>\n", streamFile);
	fprintf(summary, "------------------------------------------------------------------------------\n");
	fprintf(summary, " C1  C2  C3  C4  C5  C6  C7  C8  C9 C10  P-VALUE  PROPORTION  STATISTICAL TEST\n");
	fprintf(summary, "------------------------------------------------------------------------------\n");
	postProcessResults(option);
	fclose(summary);
#endif

	if ( argc != 2 ) 
	{
		printf("Usage: %s <filename>\n", argv[0]);
		printf("   <filename> is the file will be processed\n");
		return 0;
	}

	strncpy(g_fileName,argv[1],MAX_FILENAME_LEN-1);

	if (initSystem() != 0)
	{
		return -1;
	}
	return rndDetect();
}

void
partitionResultFile(int numOfFiles, int numOfSequences, int option, int testNameID)
{ 
	int		i, k, m, j, start, end, num, numread;
	float	c;
	FILE	**fp = (FILE **)calloc(numOfFiles+1, sizeof(FILE *));
	int		*results = (int *)calloc(numOfFiles, sizeof(int *));
	char	*s[MAXFILESPERMITTEDFORPARTITION];
	char	resultsDir[200];
	
	for ( i=0; i<MAXFILESPERMITTEDFORPARTITION; i++ )
		s[i] = (char*)calloc(200, sizeof(char));
	
	sprintf(resultsDir, "experiments/%s/%s/results.txt", generatorDir[option], testNames[testNameID]);
	
	if ( (fp[numOfFiles] = fopen(resultsDir, "r")) == NULL ) {
		printf("%s", resultsDir);
		printf(" -- file not found. Exiting program.\n");
		exit(-1);
	}
	
	for ( i=0; i<numOfFiles; i++ ) {
		if ( i < 10 )
			sprintf(s[i], "experiments/%s/%s/data%1d.txt", generatorDir[option], testNames[testNameID], i+1);
		else if (i < 100)
			sprintf(s[i], "experiments/%s/%s/data%2d.txt", generatorDir[option], testNames[testNameID], i+1);
		else
			sprintf(s[i], "experiments/%s/%s/data%3d.txt", generatorDir[option], testNames[testNameID], i+1);
	}
	numread = 0;
	m = numOfFiles/20;
	if ( (numOfFiles%20) != 0 )
		m++;
	for ( i=0; i<numOfFiles; i++ ) {
		if ( (fp[i] = fopen(s[i], "w")) == NULL ) {
			printf("%s", s[i]);
			printf(" -- file not found. Exiting program.\n");
			exit(-1);
		}
		fclose(fp[i]);
	}
	for ( num=0; num<numOfSequences; num++ ) {
		for ( k=0; k<m; k++ ) { 			/* FOR EACH BATCH */
			
			start = k*20;		/* BOUNDARY SEGMENTS */
			end   = k*20+19;
			if ( k == (m-1) )
				end = numOfFiles-1;
			
			for ( i=start; i<=end; i++ ) {		/* OPEN FILE */
				if ( (fp[i] = fopen(s[i], "a")) == NULL ) {
					printf("%s", s[i]);
					printf(" -- file not found. Exiting program.\n");
					exit(-1);
				}
			}
			
			for ( j=start; j<=end; j++ ) {		/* POPULATE FILE */
				fscanf(fp[numOfFiles], "%f", &c);
				fprintf(fp[j], "%f\n", c);
				numread++;
			}

			for ( i=start; i<=end; i++ )		/* CLOSE FILE */
				fclose(fp[i]);
		}
	}
	fclose(fp[numOfFiles]);
	for ( i=0; i<MAXFILESPERMITTEDFORPARTITION; i++ )
		free(s[i]);

	return;
}

int
cmp(const double *a, const double *b)
{
	if ( *a < *b )
		return -1;
	if ( *a == *b )
		return 0;
	return 1;
}

void
postProcessResults(int option)
{
	int		i, k, randomExcursionSampleSize, generalSampleSize;
	int		passRate, case1, case2, numOfFiles = 2;
	double	p_hat;
	char	s[200];
	
	for ( i=1; i<=NUMOFTESTS; i++ ) {		// FOR EACH TEST
		if ( testVector[i] ) {
			// SPECIAL CASES -- HANDLING MULTIPLE FILES FOR A SINGLE TEST
			if ( ((i == TEST_CUSUM) && testVector[TEST_CUSUM] ) ||
				 ((i == TEST_NONPERIODIC) && testVector[TEST_NONPERIODIC] ) ||
				 ((i == TEST_RND_EXCURSION) && testVector[TEST_RND_EXCURSION]) ||
				 ((i == TEST_RND_EXCURSION_VAR) && testVector[TEST_RND_EXCURSION_VAR]) || 
				 ((i == TEST_SERIAL) && testVector[TEST_SERIAL]) ) {
				
				if ( (i == TEST_NONPERIODIC) && testVector[TEST_NONPERIODIC] )  
					numOfFiles = MAXNUMOFTEMPLATES;
				else if ( (i == TEST_RND_EXCURSION) && testVector[TEST_RND_EXCURSION] ) 
					numOfFiles = 8;
				else if ( (i == TEST_RND_EXCURSION_VAR) && testVector[TEST_RND_EXCURSION_VAR] ) 
					numOfFiles = 18;
				else
					numOfFiles = 2;
				for ( k=0; k<numOfFiles; k++ ) {
					if ( k < 10 )
						sprintf(s, "experiments/%s/%s/data%1d.txt", generatorDir[option], testNames[i], k+1);
					else if ( k < 100 )
						sprintf(s, "experiments/%s/%s/data%2d.txt", generatorDir[option], testNames[i], k+1);
					else
						sprintf(s, "experiments/%s/%s/data%3d.txt", generatorDir[option], testNames[i], k+1);
					if ( (i == TEST_RND_EXCURSION) || (i == TEST_RND_EXCURSION_VAR) ) 
						randomExcursionSampleSize = computeMetrics(s,i);
					else
						generalSampleSize = computeMetrics(s,i);
				}
			}
			else {
				sprintf(s, "experiments/%s/%s/results.txt", generatorDir[option], testNames[i]);
				generalSampleSize = computeMetrics(s,i);
			}
		}
	}

	fprintf(summary, "\n\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
	case1 = 0;
	case2 = 0;
	if ( testVector[TEST_RND_EXCURSION] || testVector[TEST_RND_EXCURSION_VAR] ) 
		case2 = 1;
	for ( i=1; i<=NUMOFTESTS; i++ ) {
		if ( testVector[i] && (i != TEST_RND_EXCURSION) && (i != TEST_RND_EXCURSION_VAR) ) {
			case1 = 1;
			break;
		}
	}
	p_hat = 1.0 - ALPHA;
	if ( case1 ) {
		if ( generalSampleSize == 0 ) {
			fprintf(summary, "The minimum pass rate for each statistical test with the exception of the\n");
			fprintf(summary, "random excursion (variant) test is undefined.\n\n");
		}
		else {
			passRate = (p_hat - 3.0 * sqrt((p_hat*ALPHA)/generalSampleSize)) * generalSampleSize;
			fprintf(summary, "The minimum pass rate for each statistical test with the exception of the\n");
			fprintf(summary, "random excursion (variant) test is approximately = %d for a\n", generalSampleSize ? passRate : 0);
			fprintf(summary, "sample size = %d binary sequences.\n\n", generalSampleSize);
		}
	}
	if ( case2 ) {
		if ( randomExcursionSampleSize == 0 )
			fprintf(summary, "The minimum pass rate for the random excursion (variant) test is undefined.\n\n");
		else {
			passRate = (p_hat - 3.0 * sqrt((p_hat*ALPHA)/randomExcursionSampleSize)) * randomExcursionSampleSize;
			fprintf(summary, "The minimum pass rate for the random excursion (variant) test\n");
			fprintf(summary, "is approximately = %d for a sample size = %d binary sequences.\n\n", passRate, randomExcursionSampleSize);
		}
	}
	fprintf(summary, "For further guidelines construct a probability table using the MAPLE program\n");
	fprintf(summary, "provided in the addendum section of the documentation.\n");
	fprintf(summary, "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
}

int
computeMetrics(char *s, int test)
{
	int		j, pos, count, passCount, sampleSize, expCount, proportion_threshold_min, proportion_threshold_max;
	int		freqPerBin[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	double	*A, *T, chi2, proportion, uniformity, p_hat, tmp;
	float	c;
	FILE	*fp;
	
	if ( (fp = fopen(s, "r")) == NULL ) {
		printf("%s",s);
		printf(" -- file not found. Exiting program.\n");
		exit(-1);
	}
	
	if ( (A = (double *)calloc(tp.numOfBitStreams, sizeof(double))) == NULL ) {
		printf("Final Analysis Report aborted due to insufficient workspace\n");
		return 0;
	}
	
	/* Compute Metric 1: Proportion of Passing Sequences */
	
	count = 0; 		
	sampleSize = tp.numOfBitStreams;
	
	if ( (test == TEST_RND_EXCURSION) || (test == TEST_RND_EXCURSION_VAR) ) { /* Special Case: Random Excursion Tests */
		if ( (T = (double *)calloc(tp.numOfBitStreams, sizeof(double))) == NULL ) {
			printf("Final Analysis Report aborted due to insufficient workspace\n");
			return 0;
		}
		for ( j=0; j<sampleSize; j++ ) {
			fscanf(fp, "%f", &c);
			if ( c > 0.000000 )
				T[count++] = c;
		}
		
		if ( (A = (double *)calloc(count, sizeof(double))) == NULL ) {
			printf("Final Analysis Report aborted due to insufficient workspace\n");
			return 0;
		}
		
		for ( j=0; j<count; j++ )
			A[j] = T[j];
		
		sampleSize = count;
		count = 0;
		for ( j=0; j<sampleSize; j++ )
			if ( A[j] < ALPHA )
				count++;
		free(T);
	}
	else {
		if ( (A = (double *)calloc(sampleSize, sizeof(double))) == NULL ) {
			printf("Final Analysis Report aborted due to insufficient workspace\n");
			return 0;
		}
		for ( j=0; j<sampleSize; j++ ) {
			fscanf(fp, "%f", &c);
			if ( c < ALPHA )
				count++;
			A[j] = c;
		}
	}
	if ( sampleSize == 0 )
		passCount = 0;
	else
		passCount = sampleSize - count;
	
	p_hat = 1.0 - ALPHA;
	proportion_threshold_max = (p_hat + 3.0 * sqrt((p_hat*ALPHA)/sampleSize)) * sampleSize;
	proportion_threshold_min = (p_hat - 3.0 * sqrt((p_hat*ALPHA)/sampleSize)) * sampleSize;
	
	/* Compute Metric 2: Histogram */
	
	qsort((void *)A, sampleSize, sizeof(double), (void *)cmp);
	for ( j=0; j<sampleSize; j++ ) {
		pos = (int)floor(A[j]*10);
		if ( pos == 10 )
			pos--;
		freqPerBin[pos]++;
	}
	chi2 = 0.0;
	expCount = sampleSize/10;
	if ( expCount == 0 )
		uniformity = 0.0;
	else {
		for ( j=0; j<10; j++ )
			chi2 += pow(freqPerBin[j]-expCount, 2)/expCount;
		uniformity = cephes_igamc(9.0/2.0, chi2/2.0);
	}
	
	for ( j=0; j<10; j++ )			/* DISPLAY RESULTS */
		fprintf(summary, "%3d ", freqPerBin[j]);
	
	if ( expCount == 0 )
		fprintf(summary, "    ----    ");
	else if ( uniformity < 0.0001 )
		fprintf(summary, " %8.6f * ", uniformity);
	else
		fprintf(summary, " %8.6f   ", uniformity);
	
	if ( sampleSize == 0 )
		fprintf(summary, " ------     %s\n", testNames[test]);
	//	else if ( proportion < 0.96 )
	else if ( (passCount < proportion_threshold_min) || (passCount > proportion_threshold_max))
		fprintf(summary, "%4d/%-4d *  %s\n", passCount, sampleSize, testNames[test]);
	else
		fprintf(summary, "%4d/%-4d    %s\n", passCount, sampleSize, testNames[test]);
	
	fclose(fp);
	free(A);
	
	return sampleSize;
}
#endif

