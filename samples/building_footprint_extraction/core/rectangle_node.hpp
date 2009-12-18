#ifndef RECTANGLENODE_HPP_
#define RECTANGLENODE_HPP_

#include "geometry/geometry.h"

//////////////////////////////////////////////////////////

#include "core/bbox.hpp"

template<class K> inline typename K::FT intersection_area(const CGAL::Rectangle_2<K> &r, const CGAL::Circle_2<K> &c)
{
	return intersection_area(r,CGAL::Rectangle_2<K>(c.center(),Vector_2(c.radius(),0),1)); // todo : circle !!!
}

template<class K> inline typename K::FT intersection_area(const CGAL::Circle_2<K> &c, const CGAL::Rectangle_2<K> &r)
{
	return intersection_area(r,c);
}

class BoxIsValid
{
public:
	typedef bool result_type;
	BoxIsValid(const bbox_2 &box, double min_size, double max_ratio) : m_box(box), m_squared_min_size(min_size*min_size), m_max_ratio(max_ratio) {}
	bool operator()(const Rectangle_2 &n) const;
	bool operator()(const Circle_2 &n) const;
	const bbox_2& bbox() const { return m_box; }
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
	intersection_area_binary_energy(double d) : m_coefSurface(d) { }
	template<typename T, typename U> bool interact(const T &t, const U &u) const {
		return do_intersect_interior(t, u);
	}
	template<typename T, typename U> result_type operator()(const T &t, const U &u) const {
		return m_coefSurface * std::abs(CGAL::to_double(intersection_area(t,u)));
	}
private:
	double m_coefSurface;
};

////////////////////////////////////////////////////////////////////////////////

class unnamed_binary_energy
{
public:
	unnamed_binary_energy(double sigma = 10., double factor = 100) :
		m_sigma(sigma), m_factor(factor)
	{
		m_double_sigma_squared = 2. * sigma * sigma;
		m_min_squared_distance = 3. * m_sigma * m_sigma;
	}

	
	template<typename T> bool interact(const T &n1, const T &n2)
	{
		double dx = n1.x() - n2.x();
		double dy = n1.y() - n2.y();
		double d2 = dx * dx + dy * dy;
		return (d2 < m_min_squared_distance);
	}

	template<typename T> double operator()(const T &n1, const T &n2)
	{
		double dx = n1.x() - n2.x();
		double dy = n1.y() - n2.y();
		double d2 = dx * dx + dy * dy;
		if (d2 > m_min_squared_distance) return 0;
		return exp(-d2 / m_double_sigma_squared);
	}

private:
	double m_sigma;
	double m_factor;
	double m_double_sigma_squared;
	double m_min_squared_distance;
};

////////////////////////////////////////////////////////////////////////////////

class constant_unary_energy
{
public:
	constant_unary_energy(double val = -10.) : m_val(val) { }
	template<typename T> double operator()(const T &n) { return m_val; }
private:
	double m_val;
};

////////////////////////////////////////////////////////////////////////////////

class area_unary_energy
{
public:
	template<typename T> double operator()(const T &n) const {
		//	return -std::abs(CGAL::to_double(n.area()));
		return -::log(1. + std::abs(CGAL::to_double(n.area())));
	}
};

////////////////////////////////////////////////////////////////////////////////

class rectangle_points_binary_energy
{
public:
	rectangle_points_binary_energy(double coef_surface, double coef_distance, double distance_max) :
	m_coef_surface(coef_surface), m_coef_distance(coef_distance), m_distance_max(distance_max)
	{
		m_distance_max_squared = distance_max*distance_max;
	}
	template<typename T> bool interact(const T &n1, const T &n2) const;
	template<typename T> double operator()(const T &n1, const T &n2) const;

private:
	inline double distance_max() const { return m_distance_max; }
	inline double distance_max_squared() const { return m_distance_max_squared; }

	double m_coef_surface;
	double m_coef_distance;
	double m_distance_max, m_distance_max_squared;
};

template<typename T> bool rectangle_points_binary_energy::interact(const T &n1, const T &n2) const
{
	double distmin2 = (n1.center() - n2.center()).squared_length();
	if (distmin2 < distance_max_squared())
		return true;
	double distmin = ::sqrt(distmin2);
	distmin -= ::sqrt((1. + n1.ratio() * n1.ratio()) * n1.normal().squared_length());
	if (distmin < distance_max())
		return true;
	distmin -= ::sqrt((1. + n2.ratio() * n2.ratio()) * n2.normal().squared_length());
	return (distmin < distance_max_squared());
}

template<typename T> double rectangle_points_binary_energy::operator()(const T &n1, const T &n2) const
{
	double inter = std::abs(CGAL::to_double(intersection_area(n1, n2)));
	if (inter > 0)
		return m_coef_surface * inter;
	if (std::abs(m_coef_distance) < 1E-6)
		return 0.;
	double distmin2 = distance_max_squared();
	unsigned int imin = 5, jmin = 5;
	for (unsigned int i = 0; i < 4; ++i)
	{
		Point_2 p1 = n1.point(i);
		for (unsigned int j = 0; j < 4; ++j)
		{
			Point_2 p2 = n2.point(j);
			double dist2 = (p2 - p1).squared_length();
			if (dist2 < distmin2)
			{
				distmin2 = dist2;
				imin = i;
				jmin = j;
			}
		}
	}
	if (distmin2 >= distance_max_squared()) return 0;
	
	double energy = -m_coef_distance * ::sqrt(distance_max_squared() - distmin2);
	distmin2 = distance_max_squared();
	for (unsigned int i = 0; i < 4; ++i)
	{
		if (i == imin)
			continue;
		Point_2 p1 = n1.point(i);
		for (unsigned int j = 0; j < 4; ++j)
		{
			if (j == jmin)
				continue;
			Point_2 p2 = n2.point(j);
			double dist2 = (p2 - p1).squared_length();
			if (dist2 < distmin2)
			{
				distmin2 = dist2;
			}
		}
	}
	if (distmin2 < distance_max_squared())
		energy += -m_coef_distance * ::sqrt(distance_max_squared() - distmin2);
	return energy;
}

////////////////////////////////////////////////////////////////////////////////

#include "core/bbox.hpp"
#include <boost/shared_ptr.hpp>

class gil_image;
class image_gradient_unary_energy
{
public:
	typedef double result_type;

	image_gradient_unary_energy(double default_energy, const std::string& file, double sigmaD=1, unsigned int step=0);
	image_gradient_unary_energy(double default_energy, const std::string& file, const bbox_2& bbox, double sigmaD=1, unsigned int step=0);
	~image_gradient_unary_energy();
	result_type operator()(const Rectangle_2 &n) const;
	result_type operator()(const Circle_2 &c) const;
private:
	boost::shared_ptr<gil_image> m_image;
	double m_defaultEnergy;
};


#include <boost/gil/extension/matis/float_images.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>
#include <boost/gil/extension/io/tiff_dynamic_io.hpp>


using namespace boost::gil;

class gil_image
{
	typedef dev2n32F_image_t image_t; 
	image_t m_gradients;
	bbox_2 m_bbox;
public:
	double ComputeSegmentDataEnergy(const Point_2 &gridIn, const Point_2 &gridOut) const;
	void load(const std::string &file, double sigmaD=1, unsigned int step=0);
	void load(const std::string &file, const bbox_2& bbox, double sigmaD=1, unsigned int step=0); // subimage loading
	template<typename View> void init(const View& view, double sigmaD=1, unsigned int step=0);
};


#ifdef WIN32
const double M_PI = 4.0 * atan(1.0);
#endif // WIN32
template<typename Kernel1D>
void initKernelGaussian1D(Kernel1D& kernel, double sigma)
{
	// Gaussian smoothing
typedef	typename Kernel1D::value_type vt;
	const vt z = 1.0 / (std::sqrt(2 * M_PI) * sigma);
	const vt sigmasquared = sigma * sigma;
	vt x = -1.0 * kernel.center();
	vt sum =0.;
	for(typename Kernel1D::iterator i = kernel.begin();i!=kernel.end(); ++i, ++x)
	{
		*i = z * (std::exp(-0.5 * (x * x / sigmasquared)));
		sum += *i;
	}
	for (typename Kernel1D::iterator i = kernel.begin(); i!=kernel.end(); ++i)
	*i /= sum;
}

template<typename Kernel1D>
void initKernelGaussianDeriv1D(Kernel1D& kernel, double sigma)
{
	// Gaussian derivative smoothing
typedef	typename Kernel1D::value_type vt;
	const vt z = 1.0 / (std::sqrt(2 * M_PI) * sigma);
	const vt sigmasquared = sigma * sigma;
	vt x = -1.0 * kernel.center();
	typename Kernel1D::iterator i;
	vt sum = 0.;
	for (i = kernel.begin(); i!=kernel.end(); ++i, ++x)
	{
		*i = - (x / sigmasquared) * z * (std::exp(-0.5 * (x * x / sigmasquared)));
		sum += *i * x;
	}
	for (i=kernel.begin(); i!=kernel.end(); ++i)
	*i /= sum;
}

#endif /*RECTANGLENODE_HPP_*/
