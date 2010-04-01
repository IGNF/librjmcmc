#ifndef __WX_LOG_VISITOR_HPP__
#define __WX_LOG_VISITOR_HPP__

#include "visitor/ostream_visitor.hpp"
#include <wx/log.h>

template<typename Sampler>
class wx_log_visitor
{
public:
	wx_log_visitor() : m_out(""), m_visitor(m_out) {}

	void init(int dump, int save) {}

	template<typename Configuration>
        void begin(const Configuration& config, const Sampler& sampler, double t)
	{
		m_visitor.begin(config,sampler,t);
		log();
	}
	template<typename Configuration>
        void end(const Configuration& config, const Sampler& sampler, double t)
	{
		m_visitor.end(config,sampler,t);
		log();
	}

	template<typename Configuration>
	bool iterate(const Configuration& config, const Sampler& sampler, double t, unsigned int i) {
		bool b = m_visitor.iterate(config,sampler,t,i);
		log();
		return b;
	}

private:
	void log() {
		if(m_out.str().empty()) return;
		wxMutexGuiEnter();
		wxLogMessage( wxString( m_out.str().c_str(),*wxConvCurrent ).GetData());
		wxMutexGuiLeave();
		m_out.str("");
	}
	std::ostringstream m_out;
	rjmcmc::ostream_visitor<Sampler> m_visitor;
};

#endif
