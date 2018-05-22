/*
 * utility.cpp
 *
 *  Created on: 29 set 2015
 *      Author: piero
 */


//#include <dfa.h>
//#include <dfaEDSM.h>
#include "utilities.h"
#include "dfa.h"

#include <sstream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <exception>

#include <boost/math/distributions/normal.hpp> 	// for normal_distribution
using boost::math::normal; 						// typedef provides default type is double.


#include <boost/accumulators/accumulators.hpp>	// For Accumulators
#include <boost/accumulators/statistics.hpp>	// To calculate mean and variance for an accumulator

// Scores of Zalpha for alpha = {0.05, 0.025, 0.01} from the Zscore table
//#define Z_FOR_ALPHA_05 		1.645
//#define Z_FOR_ALPHA_025 		1.96
//#define Z_FOR_ALPHA_01 		2.33
//#define Z_FOR_ALPHA_20		0.845			// Valore di Zalpha per alpha = 0.20

using namespace std;



string intTostring(int a)
{
    ostringstream temp;
    temp<<a;
    return temp.str();
}

//Implemented in way that str="3 4" return 4 and if there is a element that isn't a number return 0
int stringToint(string str)
{
	stringstream stream(str);

	int n;
	while(1) {
	   stream >> n;
	   if(!stream)
	      break;
	}
	return n;
}

//Implemented in way that str="3 4" return 4 and if there is a element that isn't a number return 0
double stringTodouble( const string& str )
{
   stringstream stream(str);
   double n;

   while(1) {
	   stream >> n;
	   if(!stream)
	      break;
	}
	return n;
}

string doubleTostring(double a)
{
   ostringstream temp;
   temp << a;
   return temp.str();
}

string charToString(char c)
{
	stringstream ss;
	string s;
	ss << c;
	ss >> s;

	return s;
}

string trimRight(string str)
{
	while(str[str.length()-1] == ' ')
		str = str.substr(0,str.length()-1);

	return str;
}


double u_alpha_score(double alpha)
{
	if(alpha == 1)
		throw "Alpha values must be less than one";

	double u_alpha = 0;


	// Construct a standard normal distribution s
	//(default mean = zero, and standard deviation = unity)
	normal s;


	double complementary_alpha = 1 - alpha;


	// Quantile returns the value of area under the standard normal distribution, it is the tabulated value.
	u_alpha = quantile(s, complementary_alpha);


	// CHECK VALUE OF Z AND ALPHA
	//cout << "95% of area has a z below " << quantile(s, complementary_alpha) << endl;
	// 95% of area has a z below 1.64485

	return u_alpha;
}



/*Implemented following Knuth shuffle. More details at https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle */
void shuffleArray(int* array,int size)
{
  int k;
  int temp;
  int n = size;
  while (n > 1)
  {
    // 0 <= k < n.
    k = rand()%n;

    // n is now the last pertinent index;
    n--;

    // swap array[n] with array[k]
    temp = array[n];
    array[n] = array[k];
    array[k] = temp;
  }
}

//Don't forget to delete the memory returned from this method
int* kfold( int size, int k )
{
  int* indices = new int[ size ];

  for (int i = 0; i < size; i++ )
    indices[ i ] = i%k;

  shuffleArray( indices, size );

  return indices;
}


double z_alpha_from_u_alpha_two_proportions_test(double prop1, double prop2, int sample_size, double alpha, double* dev_std_h0)
{
	double u_alpha	= 0,	z_alpha = 0;
	double p_est	= 0,	q_est	= 0;


	// Selection of the Zalpha value (u_alpha is the value for the standard normal deviate N(0,1))
	u_alpha = u_alpha_score(alpha);


	// - Estimated error rate for population by mean of error rates for samples -
	// p_est is the accumulator set
	//boost::accumulators::accumulator_set<double, boost::accumulators::features<boost::accumulators::tag::mean, boost::accumulators::tag::variance> > p_acc;

	// Insert the values inside the accumulator
	//p_acc(prop1);
	//p_acc(prop2);

	// Compute p estimator and q
	//p_est = boost::accumulators::mean(p_acc);
	p_est = (prop1+prop2) / (double) 2.0;
	q_est = 1-p_est;


	// Questa cosa qui migliorava i risultati perchè vincolavi la stima della proporziona a non scendere troppo oltre una determinata soglia.
	// In pratica, evitati overfitting. Nel nostro caso, per esempio, non ha senso che p_est scenda molto al di sotto del 10%,
	// dato che il dataset è stato generato col 10% di rumore.

	// Check if the error rate is zero
	// Without this threashold, if the proportions is too low the approximation conditions from binomial to gaussian distribution will be not satisfied.
	// And, it's not a problem to cope with very small error as if it's a null error.
//	if( p_est <= 0.04 ){
//
//		#ifdef DEBUG1
//		cout << "zeta-beta: MINF"<<endl;
//		#endif
//
//		return DMINF;
//	}



	//TODO: Questo controllo non va fatto sulle stime, ma ti dice la condizione in cui ha senso approsiamre con una gaussiana.
	//      è una condizione sui dati. Nel nostro caso abbiamo un errore del 10% su un insieme di almeno mille stringhe, per cui Np>5 e Nq>5, sono
	//		sempre garantite
	// Check approximation conditions
	//if(!approximation_conditions_binomial_to_gaussian_distr(p_est, q_est, sample_size)){
		//cerr << "ERR: The process stop!" << endl;

		//return DMINF;
		////throw "Constraints approximation to Normal Gaussian distr not satisfied";
	//}


	// Variance and dev. std. under the H0 hypothesis
	double variance_h0 = (double) (2*p_est*q_est) / (double) sample_size;
	*dev_std_h0 = sqrt( variance_h0 );


	// Z-alpha: u_alpha is the z_alpha value in the normal gaussian distribution, multiplicated for std_err became usefull for the specific distribution
	z_alpha = u_alpha * (*dev_std_h0);


	return z_alpha;
}


double z_alpha_from_u_alpha_two_proportions_test(double prop1, double prop2, int sample_size, double alpha)
{
	double tmp = 0;
	return z_alpha_from_u_alpha_two_proportions_test(prop1, prop2, sample_size, alpha, &tmp);
}



bool approximation_conditions_binomial_to_gaussian_distr(double p_est, double q_est, int sample_size)
{
	// Necessary approximation conditions to normal distribution for binomial distribution
	if ( !(sample_size * p_est > 5  && sample_size * q_est > 5) ){
		cerr << endl << "ERR: Approximation conditions to Normal Distributions are not satisfied."<<endl;
		cerr << "ERR: Details: p: "<<p_est <<", q: "<<q_est<< ", N: "<<sample_size<< endl;
		cerr << "ERR: N*p: "<<sample_size * p_est << ", N*q: "<<sample_size * q_est << endl;
		return false;

	} else {

		return true;
	}
}


int getPoisson(double lambda)
{
	//srand(time(NULL));

	double L = exp(-lambda);
	double p = 1.0;
	int k = 0;

	do{
		k++;
		double tmp = ((double) rand() / (RAND_MAX));				// Random between 0 and 1
		p *= tmp;
	}while(p > L);

	return k-1;
}


// Normalized Compression Distance (NCD)
double ncd(double comp_x, double comp_y, double comp_xy)
{
	return ( comp_xy - fmin(comp_x, comp_y) ) / fmax(comp_x, comp_y);
}






/////////////////////////////////////////
/// INFORMATION RETRIEVAL


void compute_ir_stats(const Dfa* dfa1 ,const Dfa* target,const vector<vector<string>> &test_set,ir_statistical_measures &stats)
{
	stats.tp = stats.tn = stats.fp = stats.fn = 0;
	//cout<<"Test set dimension is "<<test_set.size()<<endl;
	//dfa1 is the test dfa
	//target is the target dfa
	for(auto &sample : test_set)
	{
		if (target->membership_query(sample)) //classification of the target (true model of the sample)
	    {
			++stats.dim_positive;
			if(dfa1->membership_query(sample))
			   ++stats.tp;
			else
			   ++stats.fn;
		}
		else //in the target sample is rejecting
		{
			++stats.dim_negative;
			if(dfa1->membership_query(sample)) //positive in dfa1 and negative in the target dfa
			   ++stats.fp;
			else //in dfa1 is sample is rejecting too
			   ++stats.tn;
		}
	}

	////////////////////////////
	// Calculates statical index

	   stats.accuracy = (bigNum) (stats.tp + stats.tn) / (bigNum) (stats.dim_positive + stats.dim_negative);

	if(stats.tp != 0 || stats.fp != 0)
	{
		stats.precision		= (bigNum) stats.tp / (bigNum) (stats.tp + stats.fp);
		stats.recall 		= (bigNum) stats.tp / (bigNum) (stats.tp + stats.fn);
		if(stats.tp != 0)
			stats.f_measure = (bigNum) (2.0 * stats.precision * stats.recall ) / (bigNum) (stats.precision + stats.recall);
		else
			stats.f_measure	= 0.0;
	}

	if(stats.tn != 0 || stats.fp != 0)
	{
		stats.specificity 	= (bigNum) stats.tn / (bigNum) (stats.tn + stats.fp);
		stats.bcr 			= (bigNum) (stats.recall + stats.specificity) / (bigNum) 2.0;
	}
	     //Following formula is direct but can have overflow problems
	    //stats.matthews      = ( (bigNum) (stats.tp*stats.tn - stats.fp*stats.fn) ) / sqrt((bigNum) ((stats.tp+stats.fp)*(stats.tp+stats.fn)*(stats.tn+stats.fp)*(stats.tn+stats.fn))) ;
	    int n               = test_set.size();
	    bigNum s            = (bigNum) (stats.tp + stats.fn) / n;
	    bigNum p            = (bigNum) (stats.tp + stats.fp) / n;

	    stats.matthews      = ( stats.tp / (bigNum) n - s*p ) / sqrt( p * s * (1-s) * (1-p) ) ;
	if( boost::math::isnan( stats.matthews )  )
	    stats.matthews      = -1;
	//if( std::isnan(stats.matthews) )
	 //   stats.matthews      = -1;  //because values are in [-1 1]
}



void print_ir_stats(ir_statistical_measures &stats)
{
     cout<<endl<<endl<<"*****************************************"<<endl<<
	                   "* STATISTICAL MEASURES ARE:"<<setw(14)<<right<<"*"<<endl<<setprecision(2)<<fixed<<left<<
             setw(23)<<"* TP"<<setw(8)<<left<<stats.tp<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* TN"<<setw(8)<<left<<stats.tn<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* FP"<<setw(8)<<left<<stats.fp<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* FN"<<setw(8)<<left<<stats.fn<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* POSITIVE SAMPLES"<<setw(8)<<left<<stats.dim_positive<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* NEGATIVE SAMPLES"<<setw(8)<<left<<stats.dim_negative<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* ACCURACY"<<setw(8)<<left<<stats.accuracy<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* PRECISION+"<<setw(8)<<left<<stats.precision<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* RECALL+(SENSITIVITY)"<<setw(8)<<left<<stats.recall<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* RECALL-(SPECIFICITY)"<<setw(8)<<left<<stats.specificity<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* F-MEASURE"<<setw(8)<<left<<stats.f_measure<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* BCR"<<setw(8)<<left<<stats.bcr<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* MATTHEWS"<<setw(8)<<left<<stats.matthews<<setw(10)<<right<<"*"<<endl<<
			           "*****************************************"<<endl<<endl;

	 cerr<<endl<<endl<<"*****************************************"<<endl<<
	                   "* STATISTICAL MEASURES ARE:"<<setw(14)<<right<<"*"<<endl<<setprecision(2)<<fixed<<left<<
             setw(23)<<"* TP"<<setw(8)<<left<<stats.tp<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* TN"<<setw(8)<<left<<stats.tn<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* FP"<<setw(8)<<left<<stats.fp<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* FN"<<setw(8)<<left<<stats.fn<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* POSITIVE SAMPLES"<<setw(8)<<left<<stats.dim_positive<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* NEGATIVE SAMPLES"<<setw(8)<<left<<stats.dim_negative<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* ACCURACY"<<setw(8)<<left<<stats.accuracy<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* PRECISION+"<<setw(8)<<left<<stats.precision<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* RECALL+(SENSITIVITY)"<<setw(8)<<left<<stats.recall<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* RECALL-(SPECIFICITY)"<<setw(8)<<left<<stats.specificity<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* F-MEASURE"<<setw(8)<<left<<stats.f_measure<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* BCR"<<setw(8)<<left<<stats.bcr<<setw(10)<<right<<"*"<<endl<<left<<
			 setw(23)<<"* MATTHEWS"<<setw(8)<<left<<stats.matthews<<setw(10)<<right<<"*"<<endl<<
			           "*****************************************"<<endl<<endl;
}


//void compute_ir_stats(dfaEDSM* dfa1, ir_statistical_measures &stats, vector<SYMBOL>* positive, int dim_positive, vector<SYMBOL>* negative, int dim_negative, int* &wp, int* &wn)
//{
//
//
//	for(int i=0; i<dim_positive; ++i)
//	{
//		if( dfa1->membership_query( positive[i]) ){
//			if(wp == NULL)
//				++stats.tp;
//			else
//				stats.tp += wp[i];
//		}else{
//			if(wp == NULL)
//				++stats.fn;
//			else
//				stats.fn += wp[i];
//		}
//	}
//
//	for(int i=0; i<dim_negative; ++i)
//	{
//		if( !dfa1->membership_query( negative[i]) ){
//			if(wn == NULL)
//				++stats.tn;
//			else
//				stats.tn += wn[i];
//		}else{
//			if(wn == NULL)
//				++stats.fp;
//			else
//				stats.fp += wn[i];
//		}
//	}
//
//	////////////////////////////
//	// Calculates statical index
//	if(stats.tp != 0 || stats.fp != 0){
//		stats.precision		= (double) stats.tp / (double) (stats.tp + stats.fp);
//		stats.recall 		= (double) stats.tp / (double) (stats.tp + stats.fn);
//		if(stats.tp != 0)
//			stats.f_measure = (double) (2.0 * stats.precision * stats.recall ) / (double) (stats.precision + stats.recall);
//		else
//			stats.f_measure	= 0.0;
//	}
//
//	if(stats.tn != 0 || stats.fp != 0){
//		stats.specificity 	= (double) stats.tn / (double) (stats.tn + stats.fp);
//		stats.bcr 			= (double) (stats.recall + stats.specificity) / (double) 2.0;
//	}
//}


///////////////////////////////////////



//int main()
//{
//  try
//  {
//    throw MyException();
//  }
//  catch(MyException& e)
//  {
//    std::cout << "MyException caught" << std::endl;
//    std::cout << e.what() << std::endl;
//  }
//  catch(std::exception& e)
//  {
//    //Other errors
//  }
//}

//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

// Note - The x86 and x64 versions do _not_ produce the same results, as the
// algorithms are optimized for their respective platforms. You can still
// compile and run any of them on any platform, but your performance with the
// non-native version will be less than optimal.


//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#if defined(_MSC_VER)

#define FORCE_INLINE	__forceinline



#define ROTL32(x,y)	_rotl(x,y)
#define ROTL64(x,y)	_rotl64(x,y)

#define BIG_CONSTANT(x) (x)

// Other compilers

#else	// defined(_MSC_VER)

#define	FORCE_INLINE inline __attribute__((always_inline))

inline uint32_t rotl32 ( uint32_t x, int8_t r )
{
  return (x << r) | (x >> (32 - r));
}

inline uint64_t rotl64 ( uint64_t x, int8_t r )
{
  return (x << r) | (x >> (64 - r));
}

#define	ROTL32(x,y)	rotl32(x,y)
#define ROTL64(x,y)	rotl64(x,y)

#define BIG_CONSTANT(x) (x##LLU)

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

FORCE_INLINE uint32_t getblock32 ( const uint32_t * p, int i )
{
  return p[i];
}

FORCE_INLINE uint64_t getblock64 ( const uint64_t * p, int i )
{
  return p[i];
}

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

FORCE_INLINE uint32_t fmix32 ( uint32_t h )
{
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}

//----------

FORCE_INLINE uint64_t fmix64 ( uint64_t k )
{
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xff51afd7ed558ccd);
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
  k ^= k >> 33;

  return k;
}

//-----------------------------------------------------------------------------



void MurmurHash3_x86_128 ( const void * key, const int len,
                           uint32_t seed, void * out )
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 16;

  uint32_t h1 = seed;
  uint32_t h2 = seed;
  uint32_t h3 = seed;
  uint32_t h4 = seed;

  const uint32_t c1 = 0x239b961b;
  const uint32_t c2 = 0xab0e9789;
  const uint32_t c3 = 0x38b34ae5;
  const uint32_t c4 = 0xa1e38b93;

  //----------
  // body

  const uint32_t * blocks = (const uint32_t *)(data + nblocks*16);

  for(int i = -nblocks; i; i++)
  {
    uint32_t k1 = getblock32(blocks,i*4+0);
    uint32_t k2 = getblock32(blocks,i*4+1);
    uint32_t k3 = getblock32(blocks,i*4+2);
    uint32_t k4 = getblock32(blocks,i*4+3);

    k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;

    h1 = ROTL32(h1,19); h1 += h2; h1 = h1*5+0x561ccd1b;

    k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;

    h2 = ROTL32(h2,17); h2 += h3; h2 = h2*5+0x0bcaa747;

    k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;

    h3 = ROTL32(h3,15); h3 += h4; h3 = h3*5+0x96cd1c35;

    k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;

    h4 = ROTL32(h4,13); h4 += h1; h4 = h4*5+0x32ac3b17;
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*16);

  uint32_t k1 = 0;
  uint32_t k2 = 0;
  uint32_t k3 = 0;
  uint32_t k4 = 0;

  switch(len & 15)
  {
  case 15: k4 ^= tail[14] << 16;
  case 14: k4 ^= tail[13] << 8;
  case 13: k4 ^= tail[12] << 0;
           k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;

  case 12: k3 ^= tail[11] << 24;
  case 11: k3 ^= tail[10] << 16;
  case 10: k3 ^= tail[ 9] << 8;
  case  9: k3 ^= tail[ 8] << 0;
           k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;

  case  8: k2 ^= tail[ 7] << 24;
  case  7: k2 ^= tail[ 6] << 16;
  case  6: k2 ^= tail[ 5] << 8;
  case  5: k2 ^= tail[ 4] << 0;
           k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;

  case  4: k1 ^= tail[ 3] << 24;
  case  3: k1 ^= tail[ 2] << 16;
  case  2: k1 ^= tail[ 1] << 8;
  case  1: k1 ^= tail[ 0] << 0;
           k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
  };

  //----------
  // finalization

  h1 ^= len; h2 ^= len; h3 ^= len; h4 ^= len;

  h1 += h2; h1 += h3; h1 += h4;
  h2 += h1; h3 += h1; h4 += h1;

  h1 = fmix32(h1);
  h2 = fmix32(h2);
  h3 = fmix32(h3);
  h4 = fmix32(h4);

  h1 += h2; h1 += h3; h1 += h4;
  h2 += h1; h3 += h1; h4 += h1;

  ((uint32_t*)out)[0] = h1;
  ((uint32_t*)out)[1] = h2;
  ((uint32_t*)out)[2] = h3;
  ((uint32_t*)out)[3] = h4;
}

//-----------------------------------------------------------------------------

void MurmurHash3_x64_128 ( const void * key, const int len,
                           const uint32_t seed, void * out )
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 16;

  uint64_t h1 = seed;
  uint64_t h2 = seed;

  const uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
  const uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

  //----------
  // body

  const uint64_t * blocks = (const uint64_t *)(data);

  for(int i = 0; i < nblocks; i++)
  {
    uint64_t k1 = getblock64(blocks,i*2+0);
    uint64_t k2 = getblock64(blocks,i*2+1);

    k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;

    h1 = ROTL64(h1,27); h1 += h2; h1 = h1*5+0x52dce729;

    k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

    h2 = ROTL64(h2,31); h2 += h1; h2 = h2*5+0x38495ab5;
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*16);

  uint64_t k1 = 0;
  uint64_t k2 = 0;

  switch(len & 15)
  {
  case 15: k2 ^= ((uint64_t)tail[14]) << 48;
  case 14: k2 ^= ((uint64_t)tail[13]) << 40;
  case 13: k2 ^= ((uint64_t)tail[12]) << 32;
  case 12: k2 ^= ((uint64_t)tail[11]) << 24;
  case 11: k2 ^= ((uint64_t)tail[10]) << 16;
  case 10: k2 ^= ((uint64_t)tail[ 9]) << 8;
  case  9: k2 ^= ((uint64_t)tail[ 8]) << 0;
           k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

  case  8: k1 ^= ((uint64_t)tail[ 7]) << 56;
  case  7: k1 ^= ((uint64_t)tail[ 6]) << 48;
  case  6: k1 ^= ((uint64_t)tail[ 5]) << 40;
  case  5: k1 ^= ((uint64_t)tail[ 4]) << 32;
  case  4: k1 ^= ((uint64_t)tail[ 3]) << 24;
  case  3: k1 ^= ((uint64_t)tail[ 2]) << 16;
  case  2: k1 ^= ((uint64_t)tail[ 1]) << 8;
  case  1: k1 ^= ((uint64_t)tail[ 0]) << 0;
           k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;
  };

  //----------
  // finalization

  h1 ^= len; h2 ^= len;

  h1 += h2;
  h2 += h1;

  h1 = fmix64(h1);
  h2 = fmix64(h2);

  h1 += h2;
  h2 += h1;

  ((uint64_t*)out)[0] = h1;
  ((uint64_t*)out)[1] = h2;
}

DfaSim::DfaSim(const Dfa* reference, const Dfa* subject, vector<long double> &w_method_stats, vector<vector<double>> &neighbour_matching_stats)
{
      dfa_reference_=reference;
      dfa_subject_=subject;

      true_positives_=w_method_stats[0];
      false_negatives_=w_method_stats[1];
      true_negatives_=w_method_stats[2];
      false_positives_=w_method_stats[3];
      precision_=w_method_stats[4];
      recall_=w_method_stats[5];
      linguistical_f_measure_=w_method_stats[6];
      specifity_=w_method_stats[7];
      bcr_=w_method_stats[8];

      nodes_sim_matrix_.reserve(neighbour_matching_stats.size());
	    copy(neighbour_matching_stats.begin(),neighbour_matching_stats.end(),back_inserter(nodes_sim_matrix_));
      structural_f_measure_=neighbour_matching_stats[neighbour_matching_stats.size()-1][0];

      nodes_sim_matrix_.pop_back();

}
	
DfaSim::~DfaSim(){
		nodes_sim_matrix_.clear();
		delete dfa_reference_;
		delete dfa_subject_;
}

void DfaSim::print_sim()const{
	cout<<"======================================"<<endl;
	cout<<"********** W-METHOD RESULTS **********"<<endl;
	cout<<"True Positives = "<<true_positives_<<endl;
	cout<<"True Negatives = "<<true_negatives_<<endl;
	cout<<"False Positives = "<<false_positives_<<endl;
	cout<<"False Negatives = "<<false_negatives_<<endl;
	cout<<"--------------------------------------"<<endl;
	cout<<"Precision = "<<precision_<<endl;
	cout<<"Recall = "<<recall_<<endl;
	cout<<"F-measure = "<<linguistical_f_measure_<<endl;
	cout<<"Specifity = "<<specifity_<<endl;
	cout<<"Balanced Classification Rate = "<<bcr_<<endl;
	cout<<"======================================"<<endl;
	cout<<"***** NEIGHBOUR MATCHING RESULTS *****";
	printf("\nNode structural similarity matrix:\n\n");
    for(int i=0; i<nodes_sim_matrix_.size(); i++)
    {
        printf(" [ ");
        for(int j=0; j<nodes_sim_matrix_[0].size(); j++){
       		printf("%lf ", nodes_sim_matrix_[i][j]);
		}
        printf("]\n");
    }
	cout<<"--------------------------------------"<<endl;
	cout <<"Structural similarity between the Dfas: " <<structural_f_measure_ << endl;
	cout<<"======================================"<<endl;
}

//-----------------------------------------------------------------------------
