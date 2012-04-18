#ifndef __BUILDING_FOOTPRINT_EXTRACTION_HPP__
#define __BUILDING_FOOTPRINT_EXTRACTION_HPP__

/************** application-specific types ****************/

#include "core/geometry.hpp"
#include <boost/variant.hpp>
typedef boost::variant<Rectangle_2,Circle_2> object;
//typedef Rectangle_2 object;

#include "core/global_reconstruction_unary_energy.hpp"
#include "image/gradient_functor.hpp"
#include "image/oriented.hpp"
typedef oriented<gradient_image_t> oriented_gradient_view;

//typedef boost::gil::gray16s_image_t ndvi_image_t;
typedef rjmcmc::any_image_t ndvi_image_t;
typedef oriented<ndvi_image_t>     oriented_ndvi_view;
typedef global_reconstruction_unary_energy<oriented_gradient_view,oriented_ndvi_view> unary_energy;

#include "mpp/energy/intersection_area_binary_energy.hpp"
typedef intersection_area_binary_energy<>      binary_energy;

#include "mpp/rectilinear_searchspace.hpp"
typedef rectilinear_searchspace<object> searchspace;

#include "rjmcmc/kernel/transform_kernels.hpp"
typedef rjmcmc::generator<searchspace>          generator_kernel;

#include "geometry/kernels/rectangle_scaled_edge_kernel.hpp"
#include "geometry/kernels/rectangle_rotation_scaled_corner_kernel.hpp"
#include "geometry/kernels/circle_translation_kernel.hpp"
typedef geometry::rectangle_scaled_edge_kernel<K> modifier_kernel1;
typedef geometry::rectangle_rotation_scaled_corner_kernel<K> modifier_kernel2;
typedef geometry::circle_translation_kernel<K> modifier_kernel3;


/************** rjmcmc library types ****************/

//#include "simulated_annealing/schedule/logarithmic_schedule.hpp"
//#include "simulated_annealing/schedule/step_schedule.hpp"
//typedef simulated_annealing::step_schedule<rjmcmc::logarithmic_schedule<double> > schedule;
#include "simulated_annealing/schedule/geometric_schedule.hpp"
typedef simulated_annealing::geometric_schedule<double> schedule;

#include "simulated_annealing/end_test/max_iteration_end_test.hpp"
typedef simulated_annealing::max_iteration_end_test                           end_test;

//#include "mpp/configuration/vector_configuration.hpp"
//typedef marked_point_process::vector_configuration<object, unary_energy, binary_energy> configuration;
#include "mpp/configuration/graph_configuration.hpp"
typedef marked_point_process::graph_configuration<object, unary_energy, binary_energy> configuration;

#include "rjmcmc/distribution/poisson_distribution.hpp"
typedef rjmcmc::poisson_distribution                           distribution;
//#include "mpp/distribution/uniform_distribution.hpp"
//typedef marked_point_process::uniform_distribution                           distribution;

#include "rjmcmc/sampler/sampler.hpp"
#include "mpp/kernel/kernel.hpp"
typedef mpp::uniform_birth_kernel<generator_kernel>           birth_kernel;
typedef mpp::uniform_death_kernel                             death_kernel;
typedef rjmcmc::binary_kernel<birth_kernel,death_kernel>      birth_death_kernel;

typedef mpp::modification_kernel<modifier_kernel1>            modification_kernel1;
typedef mpp::modification_kernel<modifier_kernel2>            modification_kernel2;
typedef mpp::modification_kernel<modifier_kernel3>            modification_kernel3;

//#include "rjmcmc/sampler/dueck_scheuer_sampler.hpp"
//typedef rjmcmc::dueck_scheuer_acceptance acceptance;
//#include "rjmcmc/sampler/franz_hoffmann_sampler.hpp"
//typedef rjmcmc::franz_hoffmann_acceptance acceptance;
#include "rjmcmc/acceptance/metropolis_acceptance.hpp"
typedef rjmcmc::metropolis_acceptance acceptance;

#include "mpp/direct_sampler.hpp"
typedef marked_point_process::direct_sampler<distribution,generator_kernel> d_sampler;

typedef rjmcmc::sampler<d_sampler,acceptance,birth_death_kernel
        ,modification_kernel1
        ,modification_kernel2
        ,modification_kernel3
        > sampler;


/************** main ****************/

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

void create_configuration(param *p, const oriented_gradient_view& grad, const oriented_ndvi_view& ndvi, configuration *&c) {
    // energies
    unary_energy e1( grad, ndvi,
                     p->get<double>("energy"),
                     p->get<double>("ponderation_grad"),
                     p->get<double>("ponderation_ndvi")
                     );

    binary_energy e2(
            p->get<double>("surface")
            );

    // empty initial configuration
    c = new configuration(e1,e2);
}

void create_sampler(param *p, sampler *&s) {
    Iso_rectangle_2 r = get_bbox(p);

    searchspace valid;
    valid.min(Rectangle_2(r.min(),K::Vector_2(0,0) ,1/p->get<double>("maxrectangleratio")));
    valid.max(Rectangle_2(r.max(),(r.max()-r.min()),  p->get<double>("maxrectangleratio")));
    valid.min(Circle_2(r.min(),0));
    valid.max(Circle_2(r.max(),p->get<double>("maxcircleradius")));

    generator_kernel birth(valid);

    birth_kernel        kbirth(birth);
    death_kernel        kdeath;

    birth_death_kernel kbirthdeath(
            kbirth, kdeath,
            p->get<double>("pbirth"),
            p->get<double>("pdeath")
            );

    modifier_kernel1     modif1;
    modifier_kernel2     modif2;
    modifier_kernel3     modif3;

    distribution cs(p->get<double>("poisson"));

    acceptance a;
    d_sampler ds( cs, birth );
    s = new sampler( ds, a, kbirthdeath
                     , mpp::make_modification_kernel(modif1,0.2)
                     , mpp::make_modification_kernel(modif2,0.2)
                     , mpp::make_modification_kernel(modif3,0.2)
                     );
    //	s = new sampler( cs, birth );
}

void create_schedule(param *p, schedule *&t)
{
    t = new schedule(
            p->get<double>("temp"),
            p->get<double>("deccoef")
            );
    //  t = new schedule( 1000, rjmcmc::logarithmic_schedule(p->get<double>("temp")) );
}

void create_end_test(param *p, end_test *&e)
{
    e = new end_test(
            p->get<int>("nbiter")
            );
}

template<typename Visitor>
void init_visitor(param *p, Visitor& v)
{
    v.init(
            p->get<int>("nbdump"),
            p->get<int>("nbsave")
            );
}

#endif // __BUILDING_FOOTPRINT_EXTRACTION_HPP__
