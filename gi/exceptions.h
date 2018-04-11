/*
 * exception.h
 */

#ifndef EXCEPT_H_
#define EXCEPT_H_

#include <exception>

using namespace std;

		////////////////////////////////////////////////////////////////////////////////
		// Exceptions

		class readingExc : public exception
		{
		  public:
			const char * what () const throw ()
			{
				return "Impossible reading DFA file.";
			}
		};

        class readingFileExc : public exception
		{
		  public:
			const char * what () const throw ()
			{
				return "Impossible reading training set file.";
			}
		};



		class invalidFormat : public exception
		{
		  public:
			const char * what () const throw ()
			{
				return "Invalid DFA format in file.";
			}
		};

        class invalidFileFormat : public exception
		{
		  public:
			const char * what () const throw ()
			{
				return "Invalid training set format in file.";
			}
		};

        class unorderedFile : public exception
		{
		  public:
			const char * what () const throw ()
			{
				return "Unordered training set File";
			}
		};


		class indexOutOfBoundTtable : public exception
		{
		public:
			const char * what () const throw ()
			{
				return "Index in Ttable out of bound!";
			}
		};

		class wrongSizes : public exception
		{
		  public:
			const char * what () const throw ()
			{
				return "Incompatible sizes.";
			}
		};

		class incompleteDfa : public exception
		{
		public:
			const char * what () const throw ()
			{
				return "DFA have an undefined behavior!";
			}
		};

		class wrongLineNumber : public exception
		{
		  public:
			const char * what () const throw ()
			{
				return "Number of line in samples file is greater than it was expected.";
			}
		};

		class wrongAlphabetSize : public exception
		{
		public:
			const char * what () const throw ()
			{
				return "Size of alphabet is different from what it was expected.";
			}
		};

		class mandatoryMinimalDFA : public exception
		{
		public:
			const char * what () const throw ()
			{
				return "A minimal DFA it's needed.";
			}
		};

		class witnessFromEquivalentDFA : public exception
		{
		public:
			const char * what () const throw ()
			{
				return "A witness string was requested for 2 equivalent DFAs.";
			}
		};

		class targetDFAwithNoStates : public exception
		{
		public:
			const char * what () const throw ()
			{
				return "Target DFA have no states!";
			}
		};

		class wMethodTestSetTooBig : public exception
		{
		public:
			const char * what () const throw ()
			{
				return "W-Method test set is too big!";
			}
		};

		class notEquivalentDfa : public exception
		{
		public:
			const char * what () const throw ()
			{
				return "Selected DFAs doesn't match!";
			}
		};

		class alphabetLengthMismatch : public exception
		{
		public:
			const char * what () const throw ()
			{
				return "Dfa1 alphabet length is different from Dfa2 alphabet length";
			}
		};

		class alphabetNull : public exception
		{
		public:
			const char * what () const throw ()
			{
				return "Passed Alphabet is a pointer to Null";
			}
		};

		class invalidParameters : public exception
		{
		  public:
			const char * what () const throw ()
			{
				return "Invalid parameters";
			}
		};

		class noApproximableDFA : public exception
		{
		  public:
			const char *  what () const throw ()
			{
				return "All classifiers aren't able to linearly separate data. You can attempt to augment training set dimension";
			}
		};

		class poorTrainingSet : public exception
		{
		public:
			const char * what () const throw ()
			{
				return "Training/test set is too small";
			}
		};

		class hypothesisTooBig : public exception
		{
		public:
			const char * what () const throw ()
			{
				return "States number of hypothesis is too big";
			}
		};

        class notFoundCounterexample : public exception
		{
		public:
			const char * what () const throw ()
			{
				return "Hypothesis and classifier aren't equals (threshold non overcomed) but you aren't able to find a counterexample";
			}

};



#endif /* EXCEPT_H_ */
