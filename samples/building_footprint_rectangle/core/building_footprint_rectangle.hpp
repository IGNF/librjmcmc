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

//[building_footprint_rectangle_definition_kernels
#include "rjmcmc/kernel/transform.hpp"

struct rectangle_edge_translation_transform
{
    enum { dimension = 6 };

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
        FT f = exp(4.0*(s-0.5));
        FT g = 1-f;
        //   res = Rectangle_2(c+m*(1-f), n,f*r);
        *out++ = x-g*r*v;
        *out++ = y+g*r*u;
        *out++ = u;
        *out++ = v;
        *out++ = f*r;
        *out++ = 1.0-s;
        return res;
    }

    template<typename IteratorIn,typename IteratorOut>
    inline double inverse(IteratorIn in, IteratorOut out) const { return apply(in,out); }
};

struct rectangle_corner_translation_transform
{
    enum { dimension = 7 };

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

    template<typename IteratorIn,typename IteratorOut>
    inline double inverse(IteratorIn in, IteratorOut out) const { return apply(in,out); }
};



/*
struct rectangle_split_transform
{
    enum { dimension = 10 };

    template<typename Iterator>
    inline double abs_jacobian(Iterator it) const { return 1.; }

    template<typename IteratorIn,typename IteratorOut>
    inline double apply  (IteratorIn in, IteratorOut out) const {
        double res = abs_jacobian(in);
        typedef typename K::FT FT;
        FT x0 = *in++;
        FT y0 = *in++;
        FT u0 = *in++;
        FT v0 = *in++;
        FT r0 = *in++;
        FT x1 = *in++;
        FT y1 = *in++;
        FT u1 = *in++;
        FT v1 = *in++;
        FT r1 = *in++;
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

    template<typename IteratorIn,typename IteratorOut>
    inline double inverse(IteratorIn in, IteratorOut out) const {
        typedef typename K::FT FT;
        FT x = *in++;
        FT y = *in++;
        FT u = *in++;
        FT v = *in++;
        FT r = *in++;
        FT s = *in++;
        FT c0 = *in++;
        FT c1 = *in++;
        FT s = *in++;
        FT s = *in++;
        //   res = Rectangle_2(c+m*(1-f), n,f*r);
        *out++ = x-g*r*v;
        *out++ = y+g*r*u;
        *out++ = u;
        *out++ = v;
        *out++ = f*r;

        return res;
    }
};
*/
/*
  generated using boost::lambda ??

 _1+_6-_5*_7,
 _2+_7-_5*_6,
 _3+_6,
 _4+_7,
 _5,
-_6,
-_7

with some aliases :

 _x+_s-_r*_t,
 _y+_t+_r*_s,
 _u+_s,
 _v+_t,
 _r,
-_s,
-_t,

*/



#include "mpp/kernel/kernel.hpp"
typedef marked_point_process::result_of_make_uniform_birth_death_kernel<object>::type  birth_death_kernel;
typedef marked_point_process::result_of_make_uniform_modification_kernel<object,rectangle_edge_translation_transform>::type  modification_kernel;
//]

//[building_footprint_rectangle_definition_energies
#include "rjmcmc/energy/energy_operators.hpp"
#include "mpp/energy/image_gradient_unary_energy.hpp"
#include "image/gradient_functor.hpp"
#include "image/oriented.hpp"
typedef oriented<gradient_image_t>                          oriented_gradient_view;
typedef image_gradient_unary_energy<oriented_gradient_view> unary_energy;

#include "mpp/energy/intersection_area_binary_energy.hpp"
typedef intersection_area_binary_energy<>                   binary_energy;
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

#include "mpp/energy/image_center_unary_energy.hpp"
typedef oriented<boost::gil::gray16_image_t> mask_type;
#include "mpp/configuration/graph_configuration.hpp"
typedef marked_point_process::graph_configuration<
        object,
        plus_energy<multiplies_energy<constant_energy<>,image_center_unary_energy<mask_type> >,minus_energy<constant_energy<>,multiplies_energy<constant_energy<>,unary_energy> > >,
        multiplies_energy<constant_energy<>,binary_energy>
        > configuration;
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
#include "rjmcmc/sampler/sampler.hpp"


typedef marked_point_process::uniform_birth<object> uniform_birth;

#include "mpp/direct_sampler.hpp"
typedef marked_point_process::direct_sampler<distribution,uniform_birth> d_sampler;
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
typedef rjmcmc::sampler<d_sampler,acceptance
        ,birth_death_kernel
        ,modification_kernel
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
#include "image/conversion_functor.hpp"

void create_configuration(const param *p, const oriented_gradient_view& grad, configuration *&c) {
    // energies
    unary_energy e1( grad );
    binary_energy e2;

    std::string mask_file = p->get<boost::filesystem::path>("mask" ).string();
    Iso_rectangle_2 bbox = get_bbox(p);
    clip_bbox(bbox,mask_file);
    mask_type mask(mask_file , bbox, conversion_functor() );
    image_center_unary_energy<mask_type> ec(mask);
    // empty initial configuration
    c = new configuration( (p->get<double>("ponderation_mask")*ec)+(p->get<double>("energy") - ( p->get<double>("ponderation_grad") * e1)),
                            p->get<double>("ponderation_surface")*e2);
}
//]

//[building_footprint_rectangle_create_sampler
void create_sampler(const param *p, sampler *&s) {
    Iso_rectangle_2 r = get_bbox(p);
    K::Vector_2 v(p->get<double>("maxsize"),p->get<double>("maxsize"));

    uniform_birth birth(
            Rectangle_2(r.min(),-v,1/p->get<double>("maxratio")),
            Rectangle_2(r.max(), v,  p->get<double>("maxratio"))
            );

    //generator_kernel    birth(ss);
    distribution cs(p->get<double>("poisson"));

    acceptance a;
    //acceptance a(p->get<double>("qtemp"));

    d_sampler ds( cs, birth );
    s = new sampler( ds, a
                     , marked_point_process::make_uniform_birth_death_kernel(birth, p->get<double>("pbirth"), p->get<double>("pdeath") )
                     , marked_point_process::make_uniform_modification_kernel<object>(rectangle_edge_translation_transform(),1)
                     //                       , 0.5 * modif2

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
//]

//[building_footprint_rectangle_optimization
#include "simulated_annealing/simulated_annealing.hpp"
//]

#endif // __BUILDING_FOOTPRINT_RECTANGLE_HPP__
