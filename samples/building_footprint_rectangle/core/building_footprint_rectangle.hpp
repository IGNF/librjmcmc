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
#include "rjmcmc/kernel/transform_kernels.hpp"
typedef rjmcmc::generator<searchspace> generator_kernel;

/*
struct rectangle_edge_translation_transform
{
    enum { size = 6 };
    typedef Rectangle_2 input_type;
    typedef Rectangle_2 output_type;

    template<typename Iterator>
    inline double abs_jacobian(Iterator it) const { return 1.; }

    template<typename IteratorIn,typename IteratorOut>
    inline double apply  (IteratorIn in, IteratorOut out) const {
        double res = abs_jacobian(in);
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
        return res;
    }
};

struct rectangle_corner_translation_transform
{
    enum { size = 7 };
    typedef Rectangle_2 input_type;
    typedef Rectangle_2 output_type;

    template<typename Iterator>
    inline double abs_jacobian(Iterator it) const { return 1.; }

    template<typename IteratorIn,typename IteratorOut>
    inline double apply  (IteratorIn in, IteratorOut out) const {
        double res = abs_jacobian(in);
        typedef typename K::FT FT;
        FT x = *in++;
        FT y = *in++;
        FT u = *in++;
        FT v = *in++;
        FT r = *in++;
        FT s = *in++;
        FT t = *in;
        //   res = Rectangle_2(c+v+u, n+v,r);
        *out++ = x+s-r*t;
        *out++ = y+t+r*s;
        *out++ = u+s;
        *out++ = v+t;
        *out++ = r;
        *out++ =-s;
        *out++ =-t;
        return res;
    }
};

typedef rjmcmc::modifier<rectangle_edge_translation_transform  > modifier_kernel1;
typedef rjmcmc::modifier<rectangle_corner_translation_transform> modifier_kernel2;
*/
#include "geometry/kernels/rectangle_scaled_edge_kernel.hpp"
#include "geometry/kernels/rectangle_rotation_scaled_corner_kernel.hpp"
#include "geometry/kernels/rectangle_split_merge_kernel.hpp"
typedef geometry::rectangle_scaled_edge_kernel<K> modifier_kernel1;
typedef geometry::rectangle_rotation_scaled_corner_kernel<K> modifier_kernel2;

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

//[building_footprint_rectangle_definition_distribution
#include "rjmcmc/distribution/poisson_distribution.hpp"
typedef rjmcmc::poisson_distribution                           distribution;
//]

//<-
//#include "rjmcmc/distribution/uniform_distribution.hpp"
//typedef rjmcmc::uniform_distribution                           distribution;
//->

//[building_footprint_rectangle_definition_sampler
/*< /Birth/ and /death kernels/ are required. They are encapsulated in a `binary_kernel` >*/
#include "rjmcmc/sampler/sampler.hpp"
#include "mpp/kernel/kernel.hpp"
typedef mpp::uniform_birth_kernel<generator_kernel>           birth_kernel;
typedef mpp::uniform_death_kernel                             death_kernel;
typedef rjmcmc::binary_kernel<birth_kernel,death_kernel>      birth_death_kernel;
/*< Optionnaly, we can specify a /modification kernel/ to modify the objects templated over the `rjmcmc::modifier` >*/
typedef mpp::modification_kernel<modifier_kernel1>            modification_kernel1;
typedef mpp::modification_kernel<modifier_kernel2>            modification_kernel2;
#include "mpp/direct_sampler.hpp"
typedef marked_point_process::direct_sampler<distribution,generator_kernel> d_sampler;
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
typedef rjmcmc::sampler<d_sampler,acceptance,birth_death_kernel
        ,modification_kernel1
        ,modification_kernel2
        > sampler;

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
    ss.min(Rectangle_2(r.min(),-v,1/p->get<double>("maxratio")));
    ss.max(Rectangle_2(r.max(), v, p->get<double>("maxratio")));

    generator_kernel    birth(ss);
    distribution cs(p->get<double>("poisson"));

    acceptance a;
    //acceptance a(p->get<double>("qtemp"));

    birth_death_kernel kbirthdeath = mpp::make_uniform_birth_death_kernel(
            birth,
            p->get<double>("pbirth"),
            p->get<double>("pdeath")
            );

    modifier_kernel1     modif1;
    modifier_kernel2     modif2;

    d_sampler ds( cs, birth );
    s = new sampler( ds, a,
                       kbirthdeath
                       , 0.5 * modif1
                       , 0.5 * modif2
                     );
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
