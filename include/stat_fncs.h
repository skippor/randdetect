#ifndef _STAT_FNCS_H_20141121
#define _STAT_FNCS_H_20141121

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     S T A T I S T I C A L  T E S T  F U N C T I O N  P R O T O T Y P E S 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int	    Frequency(int n);
int 	BlockFrequency(int M, int n);
int 	CumulativeSums(int n);
int 	Runs(int n);
int 	LongestRunOfOnes(int n);
int 	Rank(int n);
int 	DiscreteFourierTransform(int n);
int 	NonOverlappingTemplateMatchings(int m, int n);
int 	OverlappingTemplateMatchings(int m, int n);
int 	Universal(int n);
int 	ApproximateEntropy(int m, int n);
int 	RandomExcursions(int n);
int 	RandomExcursionsVariant(int n);
int 	LinearComplexity(int M, int n);
int 	Serial(int m, int n);
int 	BinaryDerivative(int k, int n);
int     Poker(int m, int n);
int		RunsDistribution(int n); 	


#endif

