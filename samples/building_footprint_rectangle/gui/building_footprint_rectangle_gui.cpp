#include <wx/app.h>

#include "param/wx_parameter_traits.hpp"
#include "geometry/wx/Rectangle_2_paint.h"
#include "visitor/wx/wx_visitor.hpp"
#include "visitor/wx/wx_controler.hpp"

#include "core/building_footprint_rectangle_parameters_inc.hpp"
#include "core/building_footprint_rectangle.hpp"

// includes for template instanciations
#include "energy/image_gradient_unary_energy_inc.hpp"
#include "image/gradient_image_inc.hpp"


class building_footprint_rectangle_gui : public wxApp
{
private:
    typedef wx_visitor<configuration,sampler> visitor_type;
    typedef wx_controler<configuration,sampler,temperature,end_test,visitor_type,any_view_type> controler_type;

    bool OnInit()
    {
        char **my_argv = new char*[argc];
        for (int i=0;i<argc;++i)
        {
            wxString mystr(argv[i]);
            my_argv[i] = new char[mystr.size()+1];
            strcpy(my_argv[i], mystr.To8BitData());
        }
        param *p = param::Instance();
        initialize_parameters(p);
        if(!p->parse(argc,my_argv)) return false;
        m_controler = new controler_type("rjmcmc building footprint rectangle");
        return true;
    }
    controler_type *m_controler;
};

IMPLEMENT_APP(building_footprint_rectangle_gui);
