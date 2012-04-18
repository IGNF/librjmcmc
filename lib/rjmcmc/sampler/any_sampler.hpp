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

#ifndef ANY_SAMPLER_HPP
#define ANY_SAMPLER_HPP

namespace rjmcmc {

    namespace detail {

        template<typename Configuration>
        class sampler_placeholder
        {
        public:
            virtual ~sampler_placeholder() {}
            virtual sampler_placeholder* clone() const=0;

            virtual const sampler_base* base() const=0;
            virtual void operator()(Configuration &c, double temp) = 0;
            virtual const char * kernel_name(unsigned int i) const = 0;
            virtual unsigned int kernel_id  () const = 0;
            virtual unsigned int kernel_size() const = 0;
        };

        template<typename Configuration, typename T>
        class sampler_holder : public sampler_placeholder<Configuration>
        {
        public:
            sampler_holder(T const & value) : held(value) {}
            virtual sampler_holder<Configuration,T>* clone() const {return new sampler_holder<Configuration,T>(held);}

            virtual const sampler_base* base() const { return &held; }
            virtual void operator()(Configuration &c, double temp)  { held(c,temp); }
            virtual const char * kernel_name(unsigned int i) const  { return held.kernel_name(i); }
            virtual unsigned int kernel_id  () const  { return held.kernel_id(); }
            virtual unsigned int kernel_size() const { return held.kernel_size(); }

        private:
            T held;
        };

    } // namespace detail

    template<typename Configuration>
    class any_sampler
    {
    public:
        any_sampler() : content(0) {}

        template<typename T>
        any_sampler(T const & t) : content(new detail::sampler_holder<Configuration,T>(t)) {}

        any_sampler(any_sampler const & other) :
                content(other.content ? other.content->clone() : 0) {}

        ~any_sampler() {delete content;}

        void swap(any_sampler& a)
        {
            std::swap(a.content,content);
        }

        any_sampler& operator=(const any_sampler& a)
                              {
            any_sampler b(a);
            swap(b);
            return *this;
        }

        // sampling step
        void operator()(Configuration &c, double temp) { (*content)(c,temp); }
        // statistics accessors
        inline const char * kernel_name(unsigned int i) const { return content->kernel_name(i); }
        inline unsigned int kernel_id  () const { return content->kernel_id(); }
        inline unsigned int kernel_size() const { return content->kernel_size(); }

        // base statistics accessors
        inline double acceptance_probability() const { return content->base()->acceptance_probability(); }
        inline double temperature() const { return content->base()->temperature(); }
        inline double delta() const { return content->base()->delta(); }
        inline double green_ratio() const { return content->base()->green_ratio(); }
        inline bool accepted() const { return content->base()->accepted(); }
        inline double kernel_ratio() const { return content->base()->kernel_ratio(); }
        inline double ref_pdf_ratio() const { return content->base()->ref_pdf_ratio(); }

    private:
        detail::sampler_placeholder<Configuration>* content;
    };


} // namespace rjmcmc

#endif // ANY_SAMPLER_HPP
