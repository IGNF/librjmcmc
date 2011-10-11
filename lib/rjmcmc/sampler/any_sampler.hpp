#ifndef __ANY_SAMPLER_HPP__
#define __ANY_SAMPLER_HPP__

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

    private:
        detail::sampler_placeholder<Configuration>* content;
    };


} // namespace rjmcmc

#endif // __ANY_SAMPLER_HPP__
