#ifndef __WX_LOG_VISITOR_HPP__
#define __WX_LOG_VISITOR_HPP__

#include "simulated_annealing/visitor/ostream_visitor.hpp"
#include <wx/log.h>


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
