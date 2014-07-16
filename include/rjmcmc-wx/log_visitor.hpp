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

#ifndef WX_LOG_VISITOR_HPP
#define WX_LOG_VISITOR_HPP

#include "simulated_annealing/visitor/ostream_visitor.hpp"
#include <wx/log.h>
#include <sstream>
#include <wx/thread.h>


namespace simulated_annealing {
    namespace wx {

        class log_visitor
        {
        public:
            log_visitor() : m_out(""), m_visitor(m_out, false) {}

            void init(int dump, int save)
            {
                m_visitor.init(dump,save);
                log();
            }

            template<typename Configuration, typename Sampler>
            void begin(const Configuration& config, const Sampler& sampler, double t)
            {
                m_visitor.begin(config,sampler,t);
                log();
            }

            template<typename Configuration, typename Sampler>
            void end(const Configuration& config, const Sampler& sampler, double t)
            {
                m_visitor.end(config,sampler,t);
                log();
            }

            template<typename Configuration, typename Sampler>
            void visit(const Configuration& config, const Sampler& sampler, double t) {
                m_visitor.visit(config,sampler,t);
                log();
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
            simulated_annealing::ostream_visitor m_visitor;
        };

    } // namespace wx
} // namespace simulated_annealing

#endif
