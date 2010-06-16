#ifndef __RJMCMC_OSTREAM_VISITOR_HPP__
#define __RJMCMC_OSTREAM_VISITOR_HPP__

#include <ostream>
#include <iomanip>
#include <time.h>
#include "rjmcmc/sampler.hpp" // pour kernel_traits<>

namespace rjmcmc {

class ostream_visitor {
private:
	unsigned int *m_proposed;
	unsigned int *m_accepted;
	clock_t m_clock_begin, m_clock;
	unsigned int m_dump;
	unsigned int m_save;
	int w;
	std::ostream& m_out;

public:
	ostream_visitor(std::ostream& out=std::cout) : m_proposed(NULL), m_accepted(NULL), m_out(out) {}
	~ostream_visitor() {
		if(m_accepted) delete m_accepted;
		if(m_proposed) delete m_proposed;
	}

	void init(int dump, int save) {
		m_dump = dump;
		m_save = save;
	}

	template<typename Configuration, typename Sampler>
        void begin(const Configuration& config, const Sampler& sampler, double t)
	{
		enum { kernel_size =  kernel_traits<typename Sampler::Kernels>::size };
		if(m_accepted) delete m_accepted;
		if(m_proposed) delete m_proposed;
		m_accepted = new unsigned int[kernel_size];
		m_proposed = new unsigned int[kernel_size];
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
	template<typename Configuration, typename Sampler>
        void end(const Configuration& config, const Sampler&, double)
	{
		clock_t clock_end = clock();
		m_out << "Iterations finished" << std::endl;
		m_out << "Total elapsed time (s) :  " << double(clock_end - m_clock_begin) / CLOCKS_PER_SEC << std::endl;
		m_out << "Graph Data energy integrity : " << config.audit_unary_energy() - config.unary_energy() << std::endl;
		m_out << "Graph Prior energy integrity: " << config.audit_binary_energy() - config.binary_energy()<< std::endl;
		m_out << "Graph Structure integrity : " << config.audit_structure() << std::endl;
		m_out << config;
		m_out << std::endl << std::flush;
	}

	template<typename Configuration, typename Sampler>
	bool iterate(const Configuration& config, const Sampler& sampler, double t, unsigned int i) {
		enum { kernel_size =  kernel_traits<typename Sampler::Kernels>::size };

		m_proposed[sampler.kernel_id()]++;
		if( sampler.accepted() ) m_accepted[sampler.kernel_id()]++;

		if (m_dump && (i % m_dump == 0))
		{
			m_out << std::setw(w) << i;
			m_out << std::setw(w) << config.size();
			
			unsigned int total_accepted =0;
			for(unsigned int k=0; k<kernel_size; ++k)
			{
				m_out << std::setw(w) << 100.* m_proposed[k] / m_dump;
				m_out << std::setw(w) << (m_proposed[k]?(100.* m_accepted[k]) / m_proposed[k]:100.);
				total_accepted += m_accepted[k];
				m_accepted[k] = m_proposed[k] = 0;
			}
			m_out << std::setw(w) << (100.*total_accepted) / m_dump;
			clock_t clock_temp = clock();
			m_out << std::setw(w) << ((clock_temp - m_clock)*1000.)/ m_dump;
			m_clock = clock_temp;
			m_out << std::setw(w) << t;
			m_out << std::setw(w) << config.unary_energy();
			m_out << std::setw(w) << config.binary_energy();
			m_out << std::setw(w) << config.energy();
			m_out << std::endl << std::flush;
		}
		return true;
	}
};

}; // namespace rjmcmc

#endif
