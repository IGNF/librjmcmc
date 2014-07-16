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

#ifdef WIN32
	#pragma warning(disable : 4251)
	#pragma warning(disable : 4275)
#endif

#include <GilViewer/layers/vector_layer_ghost.hpp>
#include <GilViewer/layers/simple_vector_layer.hpp>
#include <GilViewer/gui/panel_viewer.hpp>
#include <GilViewer/gui/panel_manager.hpp>

#include "configuration_visitor.hpp"
using namespace std;

namespace simulated_annealing {
    namespace wx {

        configuration_visitor::configuration_visitor(panel_viewer *panel)
            : m_panel(panel)
        {
        }
        wxAboutDialogInfo configuration_visitor::about_info() const
        {
            wxAboutDialogInfo info;
            info.AddDeveloper(_("Olivier Tournaire"));
            info.AddDeveloper(_("Mathieu Brédif"));
            info.AddDeveloper(_("Didier Boldo"));
            info.SetName(_("librjmcmc buildings detection sample"));
            info.SetVersion(_("0.1.0"));
            info.SetWebSite(_("http://librjmcmc.ign.fr"), _("Home page") );
            info.SetDescription(_("Marked point process for buildings detection from DEM"));
            info.SetCopyright(_T("olivier.tournaire@ign.fr\nmathieu.bredif@ign.fr"));
            return info;
        }

        void configuration_visitor::init(int dump, int save)
        {
            m_iter = 0;
            m_dump = dump;
            m_save = save;

            m_vlayer = layer::ptrLayerType(new simple_vector_layer("Extracted elements"));
            m_panel->add_layer(m_vlayer);

            boost::shared_ptr<vector_layer_ghost> ghost = m_panel->vectorlayerghost();

            m_vlayer->transform() = ghost->transform();

            m_vlayer->line_color(*wxRED);
            m_vlayer->line_style(wxSOLID);
            m_vlayer->line_width(3);
            m_vlayer->polygon_border_color(*wxBLUE);
            m_vlayer->polygon_border_style(wxSOLID);
            m_vlayer->polygon_border_width(3);
            m_vlayer->polygon_inner_color(*wxRED);
            m_vlayer->polygon_inner_style(wxTRANSPARENT);
            m_vlayer->text_visibility(false);
        }

        wxRect configuration_visitor::get_bbox() const {
            boost::shared_ptr<vector_layer_ghost> ghost = m_panel->vectorlayerghost();
            const vector_layer_ghost::Rectangle *r = ghost->get<vector_layer_ghost::Rectangle>();
            if(!r) return wxRect();
            wxPoint p0(ghost->transform().from_local_int(r->first ));
            wxPoint p1(ghost->transform().from_local_int(r->second));
            if(p0.x>p1.x) std::swap(p0.x,p1.x);
            if(p0.y>p1.y) std::swap(p0.y,p1.y);

            return wxRect(p0,p1);
        }

        void configuration_visitor::set_bbox(const wxRect& r) {
            boost::shared_ptr<vector_layer_ghost> ghost = m_panel->vectorlayerghost();
            if(!ghost) return;
            ghost->reset<vector_layer_ghost::Rectangle>();
            ghost->add_point(wxRealPoint(r.GetLeft (),r.GetTop   ()));
            ghost->add_point(wxRealPoint(r.GetRight(),r.GetBottom()));
            ghost->m_penRectangle = wxPen(*wxRED, 2, wxDOT);
            ghost->m_brushRectangle = wxBrush(*wxRED, wxTRANSPARENT);
        }

        panel_viewer* configuration_visitor::panelviewer() const { return m_panel; }

    } // namespace wx
} // namespace simulated_annealing

