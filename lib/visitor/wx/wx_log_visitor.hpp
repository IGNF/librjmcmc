#ifndef __WX_LOG_VISITOR_HPP__
#define __WX_LOG_VISITOR_HPP__

#include "visitor/ostream_visitor.hpp"

template<typename Sampler>
class wx_log_visitor
{
public:
	wx_log_visitor() : m_out(""), m_visitor(m_out) {}
	template<typename Config>
	void begin(unsigned int dump, unsigned int save, double t, const Config& config) {
		m_visitor.begin(dump,save,t,config);
		log();
	}

	template<typename Config>
	bool iterate(unsigned int i, double t, const Config& config, const Sampler& sampler) {
		bool b = m_visitor.iterate(i,t,config,sampler);
		log();
		return b;
	}

	template<typename Config>
	void end(const Config& config) { 
		m_visitor.end(config);
		log();
	}

private:
	void log() {
		if(m_out.str().empty()) return;
		wxMutexGuiEnter();
		{
			wxLogMessage( wxString( m_out.str().c_str(),*wxConvCurrent ).GetData());
		}
		wxMutexGuiLeave();
		m_out.str("");
	}
	std::ostringstream m_out;
	rjmcmc::ostream_visitor<Sampler> m_visitor;
};

#endif
