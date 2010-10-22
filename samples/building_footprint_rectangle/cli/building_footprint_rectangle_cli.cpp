//[building_footprint_rectangle_cli_parameters
#include "param/parameter.hpp"
typedef parameters< parameter > param;
#include "core/building_footprint_rectangle_parameters_inc.hpp"
//]

//[building_footprint_rectangle_cli_core_include
#include "core/building_footprint_rectangle.hpp"
//]

//[building_footprint_rectangle_cli_visitors
#include "simulated_annealing/visitor/ostream_visitor.hpp"
//]

//[building_footprint_rectangle_cli_main
int main(int argc , char** argv)
{
    /*< Retrieve the singleton instance of the parameters object... initialize the parameters object with the default values provided... parse the command line to eventually change the values >*/
    param *p = param::instance();
    initialize_parameters(p);
    if (!p->parse(argc, argv)) return -1;

    /*< Build and initialize simple visitor which prints some data on the standard output >*/
    simulated_annealing::ostream_visitor visitor;
    init_visitor(p,visitor);

    /*< Input data is an image. We first retrieve from the parameters the region to process... clip the image to fit this region... and then compute the gradient and build the attached view>*/
    Iso_rectangle_2 bbox = get_bbox(p);
    std::string  dsm_file = p->get<boost::filesystem::path>("dsm").string();
    clip_bbox(bbox, dsm_file);
    gradient_functor gf(p->get<double>("sigmaD"));
    oriented_gradient_view grad_view(dsm_file, bbox, gf);

    /*< Before launching the optimization process, we create all the required stuffs: a configuration, a sampler, a schedule scheme and an end test >*/
    configuration *conf; create_configuration(p,grad_view,conf);
    sampler       *samp; create_sampler      (p,samp);
    schedule      *sch ; create_schedule     (p,sch);
    end_test      *end ; create_end_test     (p,end);

    /*< This is the way to launch the optimization process. Here, the magic happen... >*/
    simulated_annealing::optimize(*conf,*samp,*sch,*end,visitor);

    /*< Finally release all dynamically allocated resources >*/
    if(conf) {delete conf; conf=NULL;}
    if(samp) {delete samp; samp=NULL;}
    if(sch)  {delete sch;  sch=NULL;}
    if(end)  {delete end;  end=NULL;}

    return 0;
}
//]

//  estimate_initial_temperature(p,100,*conf);
