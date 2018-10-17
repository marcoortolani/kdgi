#ifndef LEARNER_H
#define LEARNER_H

#include <tuple>

template <class Iter, class Oracle>
class Learner{
    protected:
        Oracle* oracle;
        vector<symbol_> alphabet;

        int equivalence_cost;		/*!< The number of equivalence queries done. */
        int membership_cost;		/*!< The number of membership queries done. */
        int symbol_cost;			/*!< The total number of symbol requested by all membership queries. */
        int repetition_cost;		/*!< The number of membership queries already asked by the learner to the oracle.
        								 It should be noted that if a query is found "redundant" it will not be asked to the oracle but "rebembered" by the learner itself,
        								 thus it will not increment any other cost */

        map<vector<symbol_>, bool> asked_queries;	/*!< All the membership queries done. */

        /**
         * The single operation of asking the acceptance of a phrase.
         * It also updates the relative costs.
         * @param phrase	The phrase to verify.
         * @return			The acceptance of the phrase.
         */
        bool ask_membership(vector<symbol_> phrase);

        /**
         * The single operation of evaluating an hypotesis (It's basically handed over to the oracle).
         * It also update the equivalence cost.
         * @param hypotesis			A pointer to the dfa to evaluate.
         * @param counterexample	The counterexample that separates the hypotesis from the real solution (if they aren't equal).
         * @return					Whether hypotesis and true solution are equal or not.
         */
        bool ask_equivalence(Dfa<Iter>* hypotesis, vector<symbol_>&counterexample);

    public:
        //Learner();
        virtual ~Learner() = default;

        /**
         * It returns all the costs sustained by the Learner up until this point.
         * @return	A tuple containing eq. cost, mem. cost, sym. cost and repet. cost in this order.
         */
        tuple<int, int, int, int> get_costs();

        /**
         * Reset all the costs (and the learner memory).
         * Useful for example when we want to start over with a new dfa or both a new dfa and a new language.
         */
        void reset_costs();

        /**
         * Constructor.
         * @param o		The oracle this Learner will ask the queries to.
         * @param ab	The alphabet of this language (probably unnecessary, maybe it should be deleted).
         */
        Learner(Oracle* o, vector <string> ab);

        /**
         * It sets the oracle.
         * @param o		The oracle to be set.
         */
        void set_oracle(Oracle* o);

        /**
         * It sets the alphabet (probably unnecessary, maybe it should be deleted).
         * @param ab	The alphabet to be set.
         */
        void set_alphabet(vector <string> ab);

        /**
         * The core method of this class, it must be implemented in every Learner.
         * @return	A pointer to the dfa target learned by the chosen Learner/algorithm.
         */
        virtual Dfa<Iter>* learn()=0;
};

#include "learner.tcc"
#endif
