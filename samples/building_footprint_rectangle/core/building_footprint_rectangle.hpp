#ifndef __BUILDING_FOOTPRINT_RECTANGLE_HPP__
#define __BUILDING_FOOTPRINT_RECTANGLE_HPP__

//<-
/************** application-specific types ****************/
//->

//[building_footprint_rectangle_definition_geometry
#include "core/geometry.h"
typedef Rectangle_2 object;
//]

//<-
//#include <boost/variant.hpp>
//typedef boost::variant<Rectangle_2> object;
//->

//[building_footprint_rectangle_definition_energies
#include "mpp/energy/image_gradient_unary_energy.hpp"
#include "image/gradient_functor.hpp"
#include "image/oriented.hpp"
typedef oriented<gradient_image_t>                          oriented_gradient_view;
typedef image_gradient_unary_energy<oriented_gradient_view> unary_energy;

#include "mpp/energy/intersection_area_binary_energy.hpp"
typedef intersection_area_binary_energy                     binary_energy;

#include "mpp/rectilinear_searchspace.hpp"
typedef rectilinear_searchspace<object> searchspace;
//]

//[building_footprint_rectangle_definition_kernels
#include "rjmcmc/kernel/kernels.hpp"
typedef rjmcmc::generator<searchspace> generator_kernel;



struct modifier_transform
{
    enum { size = 5 };

    template<typename Iterator>
    inline double abs_jacobian(Iterator it) const { return 1.; }

    template<typename IteratorIn,typename IteratorOut>
    inline void apply  (IteratorIn in, IteratorOut out) const {
        typedef typename K::FT FT;
        FT x = *in++;
        FT y = *in++;
        FT u = *in++;
        FT v = *in++;
        FT r = *in++;
        FT s = 1-(*in);
        FT f = exp(10*(s-0.5));
        FT g = 1-f;
        //   res = Rectangle_2(c+m*(1-f), n,f*r);
        *out++ = x-g*r*v;
        *out++ = y+g*r*u;
        *out++ = u;
        *out++ = v;
        *out++ = f*r;
        *out++ = s;
    }
};

typedef rjmcmc::modifier<modifier_transform,Rectangle_2,Rectangle_2> modifier_kernel;
//]


//<-
/************** rjmcmc library types ****************/
//->

//[building_footprint_rectangle_definition_simulated_annealing
/*< Choice of the schedule (/include/ and /typedef/) >*/
#include "simulated_annealing/schedule/geometric_schedule.hpp"
typedef simulated_annealing::geometric_schedule<double> schedule;
/*< Choice of the end_test (/include/ and /typedef/) >*/
#include "simulated_annealing/end_test/max_iteration_end_test.hpp"
typedef simulated_annealing::max_iteration_end_test     end_test;
//]

//<-
//#include "rjmcmc/step_schedule.hpp"
//typedef simulated_annealing::step_schedule<rjmcmc::logarithmic_schedule<double> > schedule;

//#include "simulated_annealing/schedule/inverse_linear_schedule.hpp"
//typedef simulated_annealing::inverse_linear_schedule<double>                 schedule;

//#include "simulated_annealing/end_test/delta_energy_end_test.hpp"
//typedef simulated_annealing::delta_energy_end_test                           end_test;

//#include "mpp/configuration/vector_configuration.hpp"
//typedef marked_point_process::vector_configuration<object, unary_energy, binary_energy> configuration;
//->

//[building_footprint_rectangle_definition_configuration
#include "mpp/configuration/graph_configuration.hpp"
typedef marked_point_process::graph_configuration<object, unary_energy, binary_energy> configuration;
//]

//[building_footprint_rectangle_definition_density
#include "mpp/density/poisson_density.hpp"
typedef marked_point_process::poisson_density                           density;
//]

//<-
//#include "mpp/density/uniform_density.hpp"
//typedef marked_point_process::uniform_density                           density;
//->

//[building_footprint_rectangle_definition_sampler
/*< /Birth/ and /death kernels/ are required. They are encapsulated in a `binary_kernel` >*/
#include "rjmcmc/sampler/sampler.hpp"
typedef rjmcmc::uniform_birth_kernel<generator_kernel>          birth_kernel;
typedef rjmcmc::uniform_death_kernel                            death_kernel;
typedef rjmcmc::binary_kernel<birth_kernel,death_kernel>        birth_death_kernel;
/*< Optionnaly, we can specify a /modification kernel/ to modify the objects templated over the `rjmcmc::modifier` >*/
typedef rjmcmc::modification_kernel<modifier_kernel>            modification_kernel;
#include "mpp/direct_sampler.hpp"
typedef marked_point_process::direct_sampler<density,generator_kernel> d_sampler;
/*< The /RJMCMC/ `rjmcmc::sampler` then encapsulates all the kernels through its template parameters to enable the sampling of the Marked Point Process relative to the poisson reference process >*/

#include "rjmcmc/acceptance/metropolis_acceptance.hpp"
typedef rjmcmc::metropolis_acceptance acceptance;

//<-
//#include "rjmcmc/acceptance/dueck_scheuer_sampler.hpp"
//typedef rjmcmc::dueck_scheuer_acceptance acceptance;

//#include "rjmcmc/acceptance/franz_hoffmann_sampler.hpp"
//typedef rjmcmc::franz_hoffmann_acceptance acceptance;
//->


//typedef rjmcmc::sampler<d_sampler,acceptance,birth_death_kernel> sampler;
typedef rjmcmc::sampler<d_sampler,acceptance,birth_death_kernel,modification_kernel> sampler;

//<-
//#include "rjmcmc/sampler/rejection_sampler.hpp"
//typedef rejection_sampler<d_sampler,null_binary_energy_predicate> sampler;
//->
//]


//[building_footprint_rectangle_bbox_accessors
Iso_rectangle_2 get_bbox(const param *p) {
    int x0 = p->get<int>("xmin");
    int x1 = p->get<int>("xmax");
    int y0 = p->get<int>("ymin");
    int y1 = p->get<int>("ymax");
    if(x0>x1) std::swap(x0,x1);
    if(y0>y1) std::swap(y0,y1);
    return Iso_rectangle_2(x0,y0,x1,y1);
}

void set_bbox(param *p, const Iso_rectangle_2& r) {
    p->set("xmin",(int) r.min().x());
    p->set("ymin",(int) r.min().y());
    p->set("xmax",(int) r.max().x());
    p->set("ymax",(int) r.max().y());
}
//]

//[building_footprint_rectangle_create_configuration
void create_configuration(const param *p, const oriented_gradient_view& grad, configuration *&c) {
    // energies
    unary_energy e1( grad,
                     p->get<double>("energy"),
                     p->get<double>("ponderation_grad")
                     );

    binary_energy e2(
            p->get<double>("surface")
            );

    // empty initial configuration
    c = new configuration(e1,e2);
}
//]

//[building_footprint_rectangle_create_sampler
void create_sampler(const param *p, sampler *&s) {
    Iso_rectangle_2 r = get_bbox(p);
    K::Vector_2 v((r.max()-r.min())*0.05);

    searchspace ss;
    ss.min(Rectangle_2(r.min(),K::Vector_2(0,0),1/p->get<double>("maxratio")));
    ss.max(Rectangle_2(r.max(),v,p->get<double>("maxratio")));

    generator_kernel    birth(ss);
    density cs(p->get<double>("poisson"));

    acceptance a;
    //acceptance a(p->get<double>("qtemp"));

    birth_death_kernel kbirthdeath = rjmcmc::make_uniform_birth_death_kernel(
            birth,
            p->get<double>("pbirth"),
            p->get<double>("pdeath")
            );

    modifier_transform mt;
    modifier_kernel     modif(mt);
    modification_kernel kmodif(modif);

    d_sampler ds( cs, birth );
    s = new sampler( ds, a, kbirthdeath, kmodif );
    //s = new sampler( ds, a, kbirthdeath);
    //s = new sampler( ds );
}
//]

//[building_footprint_rectangle_create_schedule
void create_schedule(const param *p, schedule *&t)
{
    t = new schedule(
            p->get<double>("temp"),
            p->get<double>("deccoef")
            );
    //  t = new schedule( 1000, rjmcmc::logarithmic_schedule(p->get<double>("temp")) );
}
//]

//[building_footprint_rectangle_create_end_test
void create_end_test(const param *p, end_test *&e)
{
    e = new end_test(
            p->get<int>("nbiter")
            );
}
//]

//[building_footprint_rectangle_init_visitor
template<typename Visitor>
void init_visitor(const param *p, Visitor& v)
{
    v.init(
            p->get<int>("nbdump"),
            p->get<int>("nbsave")
            );
}
//]

/*
// Salamon
#include "rjmcmc/sampler/direct_poisson_sampler.hpp"
double estimate_initial_temperature(param *p, unsigned int n, configuration& c)
{
  is_valid valid(
                  get_bbox(p),
                  p->get<double>("minsize"),
                  p->get<double>("maxratio")
                );
  generator_kernel generator(valid);
  direct_poisson_sampler<generator_kernel> dps(p->get<double>("poisson"),generator);
  double e1 = 0;
  double e2 = 0;
  double invn = 1./n;
  for(unsigned int i=0; i<n; ++i) {
    dps(c);
    double e = c.energy();
    double invne = invn*e;
    e1 += invne;
    e2 += invne*e;
  }
  double std_dev = sqrt(e2-e1*e1);
  p->set("temp",2*std_dev);
}
*/

//[building_footprint_rectangle_image_include_tpl_instanciations
#include "image/image_types.hpp"
#include "image/oriented_inc.hpp"
#include "image/gradient_functor_inc.hpp"
#include "mpp/energy/image_gradient_unary_energy_inc.hpp"
//]

//[building_footprint_rectangle_optimization
#include "simulated_annealing/simulated_annealing.hpp"
//]

#endif // __BUILDING_FOOTPRINT_RECTANGLE_HPP__
