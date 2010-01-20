#ifndef RECTANGLENODE_HPP_
#define RECTANGLENODE_HPP_

#include "geometry/geometry.h"

//////////////////////////////////////////////////////////

#include "core/bbox.hpp"

class BoxIsValid
{
public:
	typedef bool result_type;
	result_type operator()(const Rectangle_2 &n) const;
	result_type operator()(const Circle_2 &n) const;

	BoxIsValid(const bbox_2 &box, double min_size, double max_ratio) : m_box(box), m_squared_min_size(min_size*min_size), m_max_ratio(max_ratio) {}
	inline const bbox_2& bbox() const { return m_box; }
private:
	bbox_2 m_box;
	double m_squared_min_size;
	double m_max_ratio;
};

//////////////////////////////////////////////////////////

class intersection_area_binary_energy
{
public:
	typedef double result_type;

	template<typename T, typename U>
	inline bool interact(const T &t, const U &u) const {
		return do_intersect_interior(t, u);
	}

	template<typename T, typename U>
	inline result_type operator()(const T &t, const U &u) const {
		return m_coefSurface * std::abs(CGAL::to_double(intersection_area(t,u)));
	}

	intersection_area_binary_energy(double d) : m_coefSurface(d) { }
private:
	double m_coefSurface;
};


////////////////////////////////////////////////////////////////////////////////

#include <boost/shared_ptr.hpp>
class gil_image;
class image_gradient_unary_energy
{
public:
	typedef double result_type;
	result_type operator()(const Rectangle_2 &n) const;
	result_type operator()(const Circle_2 &c) const;

	image_gradient_unary_energy(double default_energy, const std::string& file, double sigmaD=1, unsigned int step=0);
	image_gradient_unary_energy(double default_energy, const std::string& file, const bbox_2& bbox, double sigmaD=1, unsigned int step=0);

private:
	boost::shared_ptr<gil_image> m_image;
	double m_defaultEnergy;
};


#include <boost/gil/extension/io/tiff_dynamic_io.hpp>
#include <boost/gil/extension/matis/float_images.hpp>

class gil_image
{
public:
	typedef boost::gil::dev2n32F_image_t image_t; 

	double integrated_flux(const Segment_2 &s) const;
	double integrated_flux(const Circle_2  &c) const;

	void load(const std::string &file, double sigmaD=1, unsigned int step=0);
	void load(const std::string &file, const bbox_2& bbox, double sigmaD=1, unsigned int step=0); // subimage loading

private:
	image_t m_gradients;
	bbox_2 m_bbox;
};

#endif /*RECTANGLENODE_HPP_*/
