#ifndef COMPOSITE_VISITOR_HPP
#define COMPOSITE_VISITOR_HPP

#include <boost/tuple/tuple.hpp>

namespace internal {

inline void init(const boost::tuples::null_type&, int dump, int save) {}
template <class H, class T>
inline void init(boost::tuples::cons<H, T>& v, int dump, int save) {
	v.get_head().init(dump,save);
	internal::init(v.get_tail(),dump,save);
}
template <class H, class T>
inline void init(boost::tuples::cons<H*, T>& v, int dump, int save) {
	v.get_head()->init(dump,save);
	internal::init(v.get_tail(),dump,save);
}

template<typename Configuration, typename Sampler>
inline bool iterate(const boost::tuples::null_type&, const Configuration& config, const Sampler& sample, double t, unsigned int i) {
    return true;
}
template <typename Configuration, typename Sampler, typename H, typename T>
inline bool iterate(boost::tuples::cons<H, T>& v, const Configuration& config, const Sampler& sample, double t, unsigned int i) {
	bool b = v.get_head().iterate(config,sample,t,i);
	return internal::iterate(v.get_tail(),config,sample,t,i) && b;
}
template <typename Configuration, typename Sampler, typename H, typename T>
inline bool iterate(boost::tuples::cons<H*, T>& v, const Configuration& config, const Sampler& sample, double t, unsigned int i) {
	bool b = v.get_head()->iterate(config,sample,t,i);
	return internal::iterate(v.get_tail(),config,sample,t,i) && b;
}

template<typename Configuration, typename Sampler>
inline void begin(const boost::tuples::null_type&, const Configuration& config, const Sampler& sample, double t) {}
template <typename Configuration, typename Sampler, typename H, typename T>
inline void begin(boost::tuples::cons<H, T>& v, const Configuration& config, const Sampler& sample, double t) {
	v.get_head().begin(config,sample,t);
	internal::begin(v.get_tail(),config,sample,t);
}
template <typename Configuration, typename Sampler, typename H, typename T>
inline void begin(boost::tuples::cons<H*, T>& v, const Configuration& config, const Sampler& sample, double t) {
	v.get_head()->begin(config,sample,t);
        internal::begin(v.get_tail(),config,sample,t);
}


template<typename Configuration, typename Sampler>
inline void end(const boost::tuples::null_type&, const Configuration& config, const Sampler& sample, double t) {}
template <typename Configuration, typename Sampler, typename H, typename T>
inline void end(boost::tuples::cons<H, T>& v, const Configuration& config, const Sampler& sample, double t) {
	v.get_head().end(config,sample,t);
	internal::end(v.get_tail(),config,sample,t);
}
template <typename Configuration, typename Sampler, typename H, typename T>
inline void end(boost::tuples::cons<H*, T>& v, const Configuration& config, const Sampler& sample, double t) {
	v.get_head()->end(config,sample,t);
	return internal::end(v.get_tail(),config,sample,t);
}


} // namespace internal


#if USE_VARIADIC_TEMPLATES
template<typename... V>
class composite_visitor
{
public:
	typedef std::tuple<V...> Visitors;
	composite_visitor(const V&... v) : m_visitors(v...) {}

#else

#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>
#ifndef RJMCMC_COMPOSITE_VISITOR_LIMIT_TYPES
	#define RJMCMC_COMPOSITE_VISITOR_LIMIT_TYPES 5
#endif
#define RJMCMC_COMPOSITE_VISITOR_ENUM_PARAMS(x) \
	BOOST_PP_ENUM_PARAMS(RJMCMC_COMPOSITE_VISITOR_LIMIT_TYPES,x)
#define RJMCMC_COMPOSITE_VISITOR_ENUM_BINARY_PARAMS(x,y) \
	BOOST_PP_ENUM_BINARY_PARAMS(RJMCMC_COMPOSITE_VISITOR_LIMIT_TYPES,x,y)
#define RJMCMC_COMPOSITE_VISITOR_ENUM_PARAMS_WITH_A_DEFAULT(x,y) \
	BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(RJMCMC_COMPOSITE_VISITOR_LIMIT_TYPES,x,y)
#define RJMCMC_COMPOSITE_VISITOR_ENUM(x) \
	BOOST_PP_ENUM(RJMCMC_COMPOSITE_VISITOR_LIMIT_TYPES,x,nil)
#define RJMCMC_COMPOSITE_VISITOR_ARG(z,n,_) const V##n& v##n = V##n()

template<RJMCMC_COMPOSITE_VISITOR_ENUM_PARAMS_WITH_A_DEFAULT(typename V,boost::tuples::null_type)>
class composite_visitor
{
public:
	typedef boost::tuple<RJMCMC_COMPOSITE_VISITOR_ENUM_PARAMS(V)> Visitors;
	composite_visitor(RJMCMC_COMPOSITE_VISITOR_ENUM(RJMCMC_COMPOSITE_VISITOR_ARG)) :
		m_visitors(RJMCMC_COMPOSITE_VISITOR_ENUM_PARAMS(v)) {}

#endif

    void init(int dump, int save) { internal::init(m_visitors,dump,save); }

    template<typename Configuration, typename Sampler>
	void begin(const Configuration& config, const Sampler& sample, double t)
    {
       internal::begin(m_visitors,config,sample,t);
    }

    template<typename Configuration, typename Sampler>
    bool iterate(const Configuration& config, const Sampler& sample, double t, unsigned int i)
    {
        return internal::iterate(m_visitors,config,sample,t,i);
    }

    template<typename Configuration, typename Sampler>
    void end(const Configuration& config, const Sampler& sample, double t)
    {
        internal::end(m_visitors,config,sample,t);
    }
private:
	Visitors m_visitors;
};

#endif // COMPOSITE_VISITOR_HPP
