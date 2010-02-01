#ifndef __RJMCMC_OSTREAM_VISITOR_HPP__
#define __RJMCMC_OSTREAM_VISITOR_HPP__

#include <ostream>
#include <iomanip>
#include <time.h>
#include "rjmcmc_sampler.hpp" // pour kernel_traits<>

namespace rjmcmc {

template<typename Sampler> class sampler_ostream_visitor {
private:
	enum { kernel_size =  kernel_traits<typename Sampler::Kernels>::size };
	unsigned int m_proposed[kernel_size];
	unsigned int m_accepted[kernel_size];
	clock_t m_clock_begin, m_clock;
	unsigned int m_dump;
	unsigned int m_save;
	int w;
	std::ostream& m_out;

public:
	sampler_ostream_visitor(std::ostream& out) : m_out(out) {}

	void begin(unsigned int dump, unsigned int save) {
		m_dump = dump;
		m_save = save;
		for (unsigned int i=0; i<kernel_size; ++i) m_accepted[i] = m_proposed[i] = 0;
	
		m_out.fill(' ');
		w = 10;
		m_out << std::setw(w)<<"Iteration"<< std::setw(w)<< "Objects";
		m_out << std::setw(w)<<"BirthP"<< std::setw(w)<<"BirthA"; // todo get labels from kernels...
		m_out << std::setw(w)<<"DeathP"<< std::setw(w)<<"DeathA";
		m_out << std::setw(w)<<"ModifP"<< std::setw(w)<<"ModifA";
		m_out << std::setw(w)<<"Accept"<< std::setw(w)<<"Time(ms)";
		m_out << std::setw(w)<<"Temp"<< std::setw(w)<<"E_data";
		m_out << std::setw(w)<<"E_priori"<< std::setw(w)<<"E_total";
		m_out << std::endl << std::flush;
	
		m_clock_begin = m_clock = clock();
	}

	template <typename Configuration> bool iterate(unsigned int i, double t,
			const Configuration& configuration, const Sampler& sampler) {
		m_proposed[sampler.kernel_id()]++;
		if( sampler.accepted() ) m_accepted[sampler.kernel_id()]++;

		if (i % m_dump == 0 && m_dump!=0)
		{
			m_out << std::setw(w) << i;
			m_out << std::setw(w) << configuration.size();
			
			unsigned int total_accepted =0;
			for(unsigned int i=0; i<kernel_size; ++i)
			{
				m_out << std::setw(w) << 100.* m_proposed[i] / m_dump;
				m_out << std::setw(w) << (m_proposed[i]?(100.* m_accepted[i]) / m_proposed[i]:100.);
				total_accepted += m_accepted[i];
				m_accepted[i] = m_proposed[i] = 0;
			}
			m_out << std::setw(w) << (100.*total_accepted) / m_dump;
			clock_t clock_temp = clock();
			m_out << std::setw(w) << ((clock_temp - m_clock)*1000.)/ m_dump;
			m_clock = clock_temp;
			m_out << std::setw(w) << t;
			m_out << std::setw(w) << configuration.unary_energy();
			m_out << std::setw(w) << configuration.binary_energy();
			m_out << std::setw(w) << configuration.unary_energy() + configuration.binary_energy();
			m_out << std::endl << std::flush;
		}
		return true;
	}

	template <typename Configuration> void end(const Configuration& configuration) {
		clock_t clock_end = clock();
		m_out << "Iterations finished" << std::endl;
		m_out << "Total elapsed time (s) :  " << double(clock_end - m_clock_begin) / CLOCKS_PER_SEC << std::endl;
		m_out << "Graph Data energy integrity : " << configuration.audit_unary_energy() - configuration.unary_energy() << std::endl;
		m_out << "Graph Prior energy integrity: " << configuration.audit_binary_energy() - configuration.binary_energy()<< std::endl;
		m_out << "Graph Structure integrity : " << configuration.audit_structure() << std::endl;
		m_out << configuration;
		m_out << std::endl << std::flush;
	}

};

}; // namespace rjmcmc

#endif