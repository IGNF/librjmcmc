#ifndef WX_CONTROLER_HPP
#define WX_CONTROLER_HPP

#include "rjmcmc/simulated_annealing.hpp"
#include <boost/thread.hpp>

template<typename Param, typename View, typename Configuration>
void create_configuration(Param *p, const View& view, Configuration *&c);
template<typename Param, typename Sampler>
void create_sampler(Param *p, Sampler *&s);
template<typename Param, typename Temperature>
void create_temperature(Param *p, Temperature *&t);
template<typename Param, typename EndTest>
void create_end_test(Param *p, EndTest *&e);
template<typename Param, typename Visitor>
void init_visitor(Param *p, Visitor& v);

template<typename Configuration, typename Sampler, typename Temperature, typename EndTest, typename Visitor, typename View>
class wx_controler : public Controler<View>
{
public:
    virtual void go(const View& view) {
        release();
        param *p = param::Instance();
	create_configuration(p,view,m_config);
	create_sampler      (p,m_sampler);
	create_temperature  (p,m_temp);
	create_end_test     (p,m_end_test);
	init_visitor(p,m_visitor);
        m_thread = new boost::thread(
                simulated_annealing<Configuration,Sampler,Temperature,EndTest,Visitor>,
                boost::ref(*m_config), boost::ref(*m_sampler),
		boost::ref(*m_temp),   boost::ref(*m_end_test),
		boost::ref(m_visitor) );
    }

    virtual void toggle_param_visibility() { m_visitor.toggle_param_visibility(); }
    virtual void toggle_chart_visibility() { m_visitor.toggle_chart_visibility(); }
    virtual void stop() { m_end_test->stop(); }

    wx_controler(const std::string& title) :
	m_config(NULL), m_sampler(NULL), m_temp(NULL), m_end_test(NULL), 
	m_visitor(title),  m_thread(NULL)
    {
        m_visitor.controler(this);
    }

    virtual ~wx_controler() {
        release();
    }
private:
    void release()
    {
        if(m_end_test){ m_end_test->stop(); }
        if(m_thread)  { m_thread->join(); delete m_thread; m_thread=NULL; }
        if(m_end_test){ delete m_end_test;m_end_test=NULL; }
        if(m_config)  { delete m_config;  m_config =NULL; }
        if(m_temp)    { delete m_temp;    m_temp   =NULL; }
        if(m_sampler) { delete m_sampler; m_sampler=NULL; }
    }
    Configuration *m_config;
    Sampler *m_sampler;
    Temperature *m_temp;
    EndTest *m_end_test;
    Visitor m_visitor;
    boost::thread *m_thread;
};

#endif
