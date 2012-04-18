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

//[building_footprint_rectangle_gui_parameters
#include "simulated_annealing/visitor/wx/wx_parameter.hpp"
typedef parameters< wx_parameter > param;
#include "core/building_footprint_rectangle_parameters_inc.hpp"
//]

//[building_footprint_rectangle_gui_core_include
#include "core/building_footprint_rectangle.hpp"
//]

//[building_footprint_rectangle_gui_visitors
#include "simulated_annealing/visitor/composite_visitor.hpp"
#include "simulated_annealing/visitor/wx/all.hpp"
#include "simulated_annealing/visitor/any_visitor.hpp"
#include "rjmcmc/sampler/any_sampler.hpp"
//]

#include "simulated_annealing/salamon_initial_schedule.hpp"

//[building_footprint_rectangle_gui_specific
#include <boost/thread.hpp>
#include <boost/filesystem/operations.hpp>
#include <wx/app.h>
#include <wx/gdicmn.h>
//]

//[building_footprint_rectangle_gui_gilviewer_and_resources
#include <GilViewer/gui/basic_viewer_frame.hpp>
#include <GilViewer/gui/define_id.hpp>
#include <GilViewer/gui/panel_viewer.hpp>
#include <GilViewer/gui/panel_manager.hpp>
#include <GilViewer/gui/layer_control.hpp>
#include <GilViewer/io/gilviewer_io_factory.hpp>
#include <GilViewer/io/gilviewer_io_factory.hpp>
#include "gui/resources/IGN.xpm"
//]

//[building_footprint_rectangle_gui_app_declaration
class building_footprint_rectangle_gui : public wxApp, public simulated_annealing::Controler
{
    //]
    //[building_footprint_rectangle_gui_app_typedef_visitor

    typedef simulated_annealing::composite_visitor<
            simulated_annealing::wx::log_visitor*,
            simulated_annealing::wx::configuration_visitor*,
            simulated_annealing::wx::parameters_visitor*,
            simulated_annealing::wx::chart_visitor*,
            simulated_annealing::wx::controler_visitor*
            > visitor;
    //] //_typedef_visitor

private:
    //[building_footprint_rectangle_gui_app_oninit
    bool OnInit()
    {
        /*<< Gilviewer IO factory initialization >>*/
        //register_all_file_formats();
        /*< Parameters processing >*/
        char **my_argv = new char*[argc];
        for (int i=0;i<argc;++i)
        {
            wxString mystr(argv[i]);
            my_argv[i] = new char[mystr.size()+1];
            strcpy(my_argv[i], mystr.To8BitData());
        }
        m_param = new param;
        initialize_parameters(m_param);
        if(!m_param->parse(argc,my_argv)) return false;
        /*< Various gui stuffs >*/
        wxSize frame_size;
        frame_size.x = std::min(800, wxGetClientDisplayRect().GetWidth());
        frame_size.y = std::min(600, wxGetClientDisplayRect().GetHeight());
        m_frame = new basic_viewer_frame((wxFrame *)NULL, wxID_ANY, _("librjmcmc: rectangular building footprint extraction"), wxDefaultPosition, frame_size);
        m_frame->SetIcon(wxICON(IGN));
        wxAuiManager * manager = m_frame->dockManager();
        panel_viewer::Register(m_frame,manager);
        m_panelviewer = panel_manager::instance()->create_object("PanelViewer");
        wxAuiPaneInfo paneInfoDrawPane;
        paneInfoDrawPane.Name( wxT("Viewer panel name") );
        paneInfoDrawPane.Caption( wxT("Viewer panel") );
        paneInfoDrawPane.TopDockable();
        paneInfoDrawPane.Center();
        paneInfoDrawPane.CloseButton(false);
        paneInfoDrawPane.CaptionVisible(false);
        manager->AddPane( m_panelviewer, paneInfoDrawPane );

        //create toolbars
        m_panelviewer->main_toolbar(m_frame,manager); // TODO : buttons do nothing
        m_panelviewer->mode_and_geometry_toolbar(m_frame,manager); // TODO : buttons do nothing

        m_frame->SetMenuBar( m_panelviewer->menubar() );
        m_frame->SetStatusText(wxT("GilViewer - Adrien Chauve & Olivier Tournaire"));

        m_confg_visitor = new simulated_annealing::wx::configuration_visitor(m_panelviewer);
        /*< Visitors initialization >*/

        m_param_visitor = new simulated_annealing::wx::parameters_visitor(m_panelviewer,m_param,m_frame);
        m_chart_visitor = new simulated_annealing::wx::chart_visitor(m_frame);
        m_contr_visitor = new simulated_annealing::wx::controler_visitor(this,m_frame);
        m_visitor = new visitor(&m_log_visitor,m_confg_visitor,m_param_visitor,m_chart_visitor,m_contr_visitor);


        m_contr_visitor->add_pane(manager);
        manager->Update();
        m_frame->Show();
        return true;
    }
    //]

public:

    void load_image(const std::string& file)
    {
        if(file=="") return;
        layer_control *lc = m_confg_visitor->panelviewer()->layercontrol();
        layer_control::const_iterator it  = lc->begin();
        layer_control::const_iterator end = lc->end();
        for(;it!=end;++it)
        {
            using namespace boost::filesystem;
            if((*it)->filename()==path(system_complete(file)).string())
            {
                return; // it;
            }
        }
        // return
                lc->add_layer_from_file(wxString(file.c_str(), *wxConvCurrent));
    }

    //[building_footprint_rectangle_gui_go
    virtual void go()
    {
        release();
        update_values(m_param);

        Iso_rectangle_2 bbox = get_bbox(m_param);
        std::string  dsm_file = m_param->get<boost::filesystem::path>("dsm").string();
        clip_bbox(bbox,dsm_file );

        gradient_functor gf(m_param->get<double>("sigmaD"));
        oriented_gradient_view grad_view(dsm_file,  bbox, gf);
        m_grad = grad_view.img();

        std::string  mask_file = m_param->get<boost::filesystem::path>("mask").string();

        load_image(dsm_file );
        load_image(mask_file);

        set_bbox(m_param,bbox);
        wxPoint p0(wxCoord(bbox.min().x()),wxCoord(bbox.min().y()));
        wxPoint p1(wxCoord(bbox.max().x()),wxCoord(bbox.max().y()));
        m_confg_visitor->set_bbox(wxRect(p0,p1));

        init_visitor        (m_param,*m_visitor);
        create_configuration(m_param,grad_view,m_config);
        //<-
        //    estimate_initial_temperature(p,100,*m_config);
        //->
        create_sampler      (m_param,m_sampler);
        create_schedule     (m_param,m_schedule);
        create_end_test     (m_param,m_end_test);

      //  std::cout << "Salamon initial schedule : " << salamon_initial_schedule(m_sampler->density(),*m_config,1000) << std::endl;
        m_config->clear();

        typedef rjmcmc::any_sampler<configuration> any_sampler;
        typedef simulated_annealing::any_visitor<configuration,any_sampler> any_visitor;

        any_sampler *sampler = new any_sampler(*m_sampler);
        any_visitor *visitor = new any_visitor(*m_visitor);

        m_thread = new boost::thread(
                simulated_annealing::optimize<configuration,any_sampler,schedule,end_test,any_visitor>,
                boost::ref(*m_config), boost::ref(*sampler),
                boost::ref(*m_schedule),   boost::ref(*m_end_test),
                boost::ref(*visitor) );
    }
    //]

    //[building_footprint_rectangle_gui_controller_overload
    virtual void stop() { m_end_test->stop(); }

    virtual void param_visibility(bool b) { m_param_visitor->Show(b); }
    virtual void chart_visibility(bool b) { m_chart_visitor->Show(b); }
    virtual bool param_visibility() const { return m_param_visitor->IsShown(); }
    virtual bool chart_visibility() const { return m_chart_visitor->IsShown(); }
    //]

    //[building_footprint_rectangle_gui_cons_des
    building_footprint_rectangle_gui() :
            m_config(NULL), m_sampler(NULL), m_schedule(NULL), m_end_test(NULL),
            m_visitor(NULL), m_thread(NULL),
            m_confg_visitor(NULL), m_param_visitor(NULL), m_chart_visitor(NULL), m_param(NULL)
    {
    }

    virtual ~building_footprint_rectangle_gui() {
        release();
        //<-
        /*
        if(m_confg_visitor) { m_confg_visitor->Close(); }
        if(m_param_visitor) { m_param_visitor->Close(); }
        if(m_chart_visitor) { m_chart_visitor->Close(); }
        */
        //->
        if(m_visitor    ) { delete m_visitor; }
        if(m_param      ) { delete m_param; }
    }
    //]
private:
    //[building_footprint_rectangle_gui_release
    void release()
    {
        if(m_end_test){ m_end_test->stop(); }
        if(m_thread)  { m_thread->join(); delete m_thread; m_thread=NULL; }
        if(m_end_test){ delete m_end_test;m_end_test=NULL; }
        if(m_config)  { delete m_config;  m_config  =NULL; }
        if(m_schedule){ delete m_schedule;m_schedule=NULL; }
        if(m_sampler) { delete m_sampler; m_sampler =NULL; }
    }
    //]

    //[building_footprint_rectangle_gui_members
    configuration *m_config;
    sampler       *m_sampler;
    schedule      *m_schedule;
    end_test      *m_end_test;
    visitor       *m_visitor;
    boost::thread *m_thread;
    basic_viewer_frame *m_frame;
    panel_viewer *m_panelviewer;
    param        *m_param;

    simulated_annealing::wx::configuration_visitor *m_confg_visitor;
    simulated_annealing::wx::parameters_visitor    *m_param_visitor;
    simulated_annealing::wx::chart_visitor         *m_chart_visitor;
    simulated_annealing::wx::log_visitor            m_log_visitor;
    simulated_annealing::wx::controler_visitor     *m_contr_visitor;
    boost::shared_ptr<gradient_image_t>     m_grad;

    DECLARE_GILVIEWER_METHODS_FOR_EVENTS_TABLE();
    DECLARE_EVENT_TABLE();
};
//]

BEGIN_EVENT_TABLE(building_footprint_rectangle_gui,wxApp)
ADD_GILVIEWER_EVENTS_TO_TABLE(building_footprint_rectangle_gui)
END_EVENT_TABLE()

IMPLEMENTS_GILVIEWER_METHODS_FOR_EVENTS_TABLE(building_footprint_rectangle_gui,m_panelviewer);


//[building_footprint_rectangle_gui_implement
IMPLEMENT_APP(building_footprint_rectangle_gui);
//]
