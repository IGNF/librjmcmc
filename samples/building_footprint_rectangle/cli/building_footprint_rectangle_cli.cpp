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

//[building_footprint_rectangle_cli_parameters
#include "param/parameter.hpp"
typedef parameters< parameter > param;
#include "core/building_footprint_rectangle_parameters_inc.hpp"
//]

//[building_footprint_rectangle_cli_core_include
#include "core/building_footprint_rectangle.hpp"
//]

//[building_footprint_rectangle_cli_visitors
#include "rjmcmc/sampler/any_sampler.hpp"
#include "simulated_annealing/visitor/any_visitor.hpp"
#include "simulated_annealing/visitor/ostream_visitor.hpp"
#ifdef USE_SHP
# include "simulated_annealing/visitor/shp/shp_visitor.hpp"
#endif
//]

//[building_footprint_rectangle_cli_main
int main(int argc , char** argv)
{
    /*< Retrieve the singleton instance of the parameters object... initialize the parameters object with the default values provided... parse the command line to eventually change the values >*/
    param *p = new param;
    initialize_parameters(p);
    if (!p->parse(argc, argv)) return -1;


    /*< Input data is an image. We first retrieve from the parameters the region to process... clip the image to fit this region... and then compute the gradient and build the attached view>*/
    Iso_rectangle_2 bbox = get_bbox(p);
    std::string  dsm_file = p->get<boost::filesystem::path>("dsm").string();
    clip_bbox(bbox, dsm_file);

    gradient_functor gf(p->get<double>("sigmaD"));
    oriented_gradient_view grad_view(dsm_file, bbox, gf);

    set_bbox(p,bbox);

    /*< Before launching the optimization process, we create all the required stuffs: a configuration, a sampler, a schedule scheme and an end test >*/
    configuration *conf; create_configuration(p,grad_view,conf);
    sampler       *samp; create_sampler      (p,samp);
    schedule      *sch ; create_schedule     (p,sch);
    end_test      *end ; create_end_test     (p,end);

    // test avec les any_*
    typedef rjmcmc::any_sampler<configuration> any_sampler;
    any_sampler sampler(*samp);

    /*< Build and initialize simple visitor which prints some data on the standard output >*/
    typedef simulated_annealing::any_composite_visitor<configuration,any_sampler> any_visitor;
    any_visitor visitor;
    visitor.push_back(simulated_annealing::ostream_visitor());
#ifdef USE_SHP
    visitor.push_back(simulated_annealing::shp::shp_visitor(argv[0]));
#endif
    init_visitor(p,visitor);

    /*< This is the way to launch the optimization process. Here, the magic happen... >*/
    simulated_annealing::optimize(*conf,sampler,*sch,*end,visitor);

    /*< Finally release all dynamically allocated resources >*/
    if(conf) {delete conf; conf=NULL;}
    if(samp) {delete samp; samp=NULL;}
    if(sch)  {delete sch;  sch=NULL;}
    if(end)  {delete end;  end=NULL;}

    return 0;
}
//]

//  estimate_initial_temperature(p,100,*conf);
