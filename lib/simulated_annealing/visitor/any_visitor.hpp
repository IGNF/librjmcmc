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

#ifndef ANY_VISITOR_HPP
#define ANY_VISITOR_HPP

namespace simulated_annealing {

    namespace detail {

        template<typename Configuration, typename Sampler>
        class placeholder
        {
        public:
            virtual ~placeholder() {}
            virtual placeholder* clone() const=0;

            virtual void init(int dump, int save) = 0;
            virtual void begin(const Configuration& config, const Sampler& sample, double t)=0;
            virtual void visit(const Configuration& config, const Sampler& sample, double t)=0;
            virtual void end  (const Configuration& config, const Sampler& sample, double t)=0;
        };

        template<typename Configuration, typename Sampler, typename T>
        class holder : public placeholder<Configuration,Sampler>
        {
        public:
            holder(T const & value) : held(value) {}
            virtual holder<Configuration,Sampler,T>* clone() const {return new holder<Configuration,Sampler,T>(held);}

            virtual void init(int dump, int save) { held.init(dump,save); }
            virtual void begin(const Configuration& config, const Sampler& sample, double t) { held.begin(config,sample,t); }
            virtual void visit(const Configuration& config, const Sampler& sample, double t) { held.visit(config,sample,t); }
            virtual void end  (const Configuration& config, const Sampler& sample, double t) { held.end  (config,sample,t); }

        private:
            T held;
        };

    } // namespace detail

    template<typename Configuration, typename Sampler>
    class any_visitor
    {
    public:
        any_visitor() : content(0) {}

        template<typename T>
        any_visitor(T const & t) : content(new detail::holder<Configuration,Sampler,T>(t)) {}

        any_visitor(any_visitor const & other) :
                content(other.content ? other.content->clone() : 0) {}

        ~any_visitor() {delete content;}

        void swap(any_visitor& a)
        {
            std::swap(a.content,content);
        }

        any_visitor& operator=(const any_visitor& a)
                              {
            any_visitor b(a);
            swap(b);
            return *this;
        }

        void init(int dump, int save) { content->init(dump,save); }
        void begin(const Configuration& config, const Sampler& sample, double t) { content->begin(config,sample,t); }
        void visit(const Configuration& config, const Sampler& sample, double t) { content->visit(config,sample,t); }
        void end  (const Configuration& config, const Sampler& sample, double t) { content->end(config,sample,t); }

    private:
        detail::placeholder<Configuration,Sampler>* content;
    };


    template<typename Configuration, typename Sampler>
    class any_composite_visitor : public std::vector<any_visitor<Configuration,Sampler> >
    {
        typedef std::vector<any_visitor<Configuration,Sampler> > base;
    public:
        typedef typename base::iterator iterator;
        void init(int dump, int save) { for(iterator it=base::begin(); it!=base::end(); ++it) it->init(dump,save); }
        void begin(const Configuration& config, const Sampler& sample, double t) { for(iterator it=base::begin(); it!=base::end(); ++it) it->begin(config,sample,t); }
        void visit(const Configuration& config, const Sampler& sample, double t) { for(iterator it=base::begin(); it!=base::end(); ++it) it->visit(config,sample,t); }
        void end  (const Configuration& config, const Sampler& sample, double t) { for(iterator it=base::begin(); it!=base::end(); ++it) it->end(config,sample,t); }
    };


} // namespace simulated_annealing

#endif // ANY_VISITOR_HPP
