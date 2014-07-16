/***********************************************************************
This file is part of the librjmcmc project source files.

Copyright : Institut Geographique National (2008-2012)
Contributors : Mathieu Brédif, Olivier Tournaire, Didier Boldo
email : librjmcmc@ign.fr

This software is a generic C++ library for stochastic optimization.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software. You can use,
modify and/or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty and the software's author, the holder of the
economic rights, and the successive licensors have only limited liability.

In this respect, the user's attention is drawn to the risks associated
with loading, using, modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean that it is complicated to manipulate, and that also
therefore means that it is reserved for developers and experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and, more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

***********************************************************************/

#ifndef RJMCMC_REJECTION_VARIATE_HPP
#define RJMCMC_REJECTION_VARIATE_HPP
#include <algorithm>

namespace rjmcmc {

    namespace rejection_variate_normalizer
    {
        class constant_normalizer
        {
            double m_inv_probability;
        public:
            constant_normalizer(double p) : m_inv_probability(1./p) {}
            void fail() const {}
            void pass() const {}
            double inv_probability() const { return m_inv_probability; }
            void clear() const {}
        };

        // Monte Carlo estimation of the success rate of the predicate (assuming it is constant)
        class monte_carlo
        {
            mutable unsigned long long m_test, m_pass;
            mutable double m_inv_probability;
        public:
            monte_carlo() { clear(); }
            void fail() const { ++m_test;           m_inv_probability = double(m_test)/m_pass; }
            void pass() const { ++m_test; ++m_pass; m_inv_probability = double(m_test)/m_pass; }
            double inv_probability() const { return m_inv_probability; }
            void clear() {  m_pass = m_test = 0; m_inv_probability = 0.; }
        };

        // Monte Carlo estimation of the success rate of the predicate based on the last N tests
        template<int N> class sliding_monte_carlo
        {
            mutable bool m_test[N];
            mutable int m_i;
            mutable unsigned long long m_pass;
            mutable double m_inv_probability;
        public:
            sliding_monte_carlo() { clear(); }
            void fail() const { if( m_test[m_i]) {--m_pass; m_test[m_i]=false; m_inv_probability = double(N)/m_pass;} m_i=(m_i+1)%N; }
            void pass() const { if(!m_test[m_i]) {++m_pass; m_test[m_i]=true ; m_inv_probability = double(N)/m_pass;} m_i=(m_i+1)%N; }
            double inv_probability() const { return m_inv_probability; }
            void clear() {
                for(int i=0;i<N;++i) m_test[i]=false;
                m_i=0;
                m_pass=0;
                m_inv_probability = 0.;
            }
        };
        // Monte Carlo estimation of the success rate of the predicate based with (decay^n) sample weights where n is the age of the sample (in number of iterations)
        class decay_monte_carlo
        {
            double m_decay; // 0<decay<1, usually close to 1
            mutable double m_inv_probability;
        public:
            decay_monte_carlo(double decay=0.999) : m_decay(decay)
            {
                assert(decay>0 && decay<1);
                clear();
            }

            //   p(i+1) = decay*p(i) + (1-decay)*(pass?1:0)
            // 1/p(i+1) = 1/(decay*p(i) + (1-decay)*(pass?1:0)) = (1/p(i)) / (decay + (1-decay)*(pass?(1/p(i)):0))
            //  fail ->   (1/p(i)) /  decay
            //  pass ->   (1/p(i)) / (decay + (1-decay)*(1/p(i)))
            void fail() const { m_inv_probability /= m_decay; }
            void pass() const { m_inv_probability /= m_decay+(1.-m_decay)*m_inv_probability; }
            double inv_probability() const { return m_inv_probability; }
            void clear() { m_inv_probability = 1.; }
        };
    }

    template<typename Variate, typename Predicate, typename Normalizer=rejection_variate_normalizer::monte_carlo>
    class rejection_variate
    {
        Variate m_variate;
        Predicate m_pred;
        Normalizer m_normalizer;

    public:
        typedef typename Variate::value_type value_type;        
        enum { dimension = Variate::dimension };

        template<typename Engine, typename OutputIterator>
        inline double operator()(Engine& e, OutputIterator it) const {
            value_type val[dimension];
            double res = m_variate(e,val);
            if(!m_pred(val))
            {
                m_normalizer.fail();
                return 0; // sampling failure code
            }
            m_normalizer.pass();
            std::copy(val,val+dimension,it); // copy is not mandatory if we know we can inspect the output iterator directly
            return res*m_normalizer.inv_probability(); // renormalization by the current estimate of the rejection probability
        }

        template<typename ForwardIterator>
        inline double pdf(ForwardIterator it) const {
            if(!m_pred(it)) return 0.; // it is outside the support
            return m_variate.pdf(it)*m_normalizer.inv_probability();  // renormalization by the current estimate of the rejection probability
        }

        Normalizer& normalizer() { return m_normalizer;  }
        const Normalizer& normalizer() const { return m_normalizer;  }

        rejection_variate(const Variate& variate, const Predicate& pred, Normalizer normalizer = Normalizer())
            : m_variate(variate),  m_pred(pred), m_normalizer(normalizer)
        {
        }
    };

}; // namespace rjmcmc

#endif // RJMCMC_REJECTION_VARIATE_HPP
