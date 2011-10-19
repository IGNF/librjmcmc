#ifndef __ANY_VISITOR_HPP__
#define __ANY_VISITOR_HPP__

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

#endif // __ANY_VISITOR_HPP__
