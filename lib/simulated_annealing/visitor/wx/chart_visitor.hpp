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

#ifndef WX_CHART_VISITOR_HPP
#define WX_CHART_VISITOR_HPP

class VectorDataset;

#include <boost/shared_ptr.hpp>


#ifdef WIN32
	#pragma warning(disable : 4251)
	#pragma warning(disable : 4275)
    #include <wx/msw/winundef.h>
#endif
#include <wx/window.h>
#include <wx/toplevel.h>

namespace simulated_annealing {
    namespace wx {

        class chart_frame;

        class chart_visitor
        {
        public:
            chart_visitor(
                    wxWindow *parent = (wxWindow *) NULL,
                    wxWindowID id = wxID_ANY,
                    const wxString& title = _("librjmcmc charts"),
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

            void init(int dump, int)
            {
                m_dump = dump;
                m_iter = 0;
            }

            template<typename Configuration, typename Sampler>
            void begin(const Configuration& config, const Sampler& sampler, double t)
            {
                clear();
                add(config.energy(),t);
            }
            template<typename Configuration, typename Sampler>
            void end(const Configuration& config, const Sampler& sampler, double t)
            {
                add(config.energy(),t);
            }

            template<typename Configuration, typename Sampler>
            void visit(const Configuration& config, const Sampler& sampler, double t) {
                if(++m_iter==m_dump)
                {
                    m_iter = 0;
                    add(config.energy(),t);
                }
            }
            void Show(bool b = true);
            bool IsShown() const;

        private:
            boost::shared_ptr<chart_frame> m_frame;
            VectorDataset *m_dataset[2];
            unsigned int m_dump, m_iter;

            void clear();
            void add(double e, double t);
        };

    } // namespace wx
} // namespace simulated_annealing

#endif // WX_CHART_VISITOR_HPP
