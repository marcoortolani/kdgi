/*#ifndef ORACLE_H
#define ORACLE_H

#include "concretedfa.h"
#include <string>

template <class Iter>
class Oracle{
private:
	int equivalence_query_complexity_;
	int membership_query_complexity_;
	int symbol_complexity_;
	vector<pair<vector<symbol_>, bool>> query_history_;

	map<vector<symbol_>, bool> distinct_queries_;
	int reduntant_queries_;

    public:
		void membership_cost(vector<symbol_> phrase, bool query_result){
			query_history_.push_back(std :: make_pair(phrase, query_result));
			membership_query_complexity_ += 1;
			symbol_complexity_ += phrase.size();

			auto it = distinct_queries_.find(phrase);
			if(it == distinct_queries_.end()){
				distinct_queries_[phrase] = query_result;
			}
			else{
				++reduntant_queries_;
			}
		}

		void equivalence_cost(){
			equivalence_query_complexity_ += 1;
		}

		tuple<int, int, int, int> get_costs(){
			return std :: make_tuple(equivalence_query_complexity_, membership_query_complexity_, symbol_complexity_, reduntant_queries_);
		}

		vector<pair<vector<symbol_>, bool>> get_query_history(){
			return query_history_;
		}

		void reset(){
			distinct_queries_.clear();
			reduntant_queries_ = 0;

			query_history_.clear();
			equivalence_query_complexity_ = membership_query_complexity_ = symbol_complexity_ = 0;
		}

		Oracle(){
			reset();
		}

        virtual bool membership_query(vector<string> str) = 0;

        virtual bool equivalence_query(Dfa<Iter>* dfa_hp , vector<string>& witness_results) = 0;
};

#endif
*/
