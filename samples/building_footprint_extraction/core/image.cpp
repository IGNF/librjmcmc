#include "core/image.hpp"

#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>

void rjmcmc::image::load(const std::string &file)
{
	tiff_read_image(file, m_image);
}

double rjmcmc::image::error(const Rectangle_2 &s) const {
	return 0;
}

double rjmcmc::image::error(const Circle_2  &c) const {
	return 0;
}
