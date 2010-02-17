#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/tiff_dynamic_io.hpp>

typedef boost::gil::rgb16_image_t  image_rgb_t;
typedef boost::gil::gray16_image_t image_pir_t;
typedef boost::gil::gray16_image_t image_z_t;
typedef boost::gil::gray16s_image_t image_ndvi_t;
//typedef boost::gil::rgb16_image_t image_zndvi_t;


typedef image_rgb_t::const_view_t image_rgb_const_view_t;
typedef image_pir_t::const_view_t image_pir_const_view_t;
typedef image_z_t::const_view_t   image_z_const_view_t;
typedef image_ndvi_t::view_t image_ndvi_view_t;
//typedef image_zndvi_t::view_t image_zndvi_view_t;

void ndvi(const image_rgb_const_view_t& rgb, const image_pir_const_view_t& pir, const image_z_const_view_t& z, const image_ndvi_view_t& dst) {
    for (int y=0; y<dst.height(); ++y)
        for (int x=0; x<dst.width(); ++x) {
	double r = boost::gil::at_c<0>(rgb(x,y));
	double i = (((pir(x,y))*26.5263)+1952.3755);
	if((r+i)==0) {
		dst(x,y) = 0;
	} else {
		dst(x,y) = (unsigned short) (85+((170*(i-r))/(i+r)));
	}

	/*
	if(z(x,y)==32768) {
		boost::gil::at_c<0>(dst(x,y)) = 0;
	} else {
		boost::gil::at_c<0>(dst(x,y)) = z(x,y);
	}
	if(r+i==0) {
		boost::gil::at_c<1>(dst(x,y)) = 0;
	} else {
		boost::gil::at_c<1>(dst(x,y)) = 1000+((2000*(i-r))/(i+r));
	}
	//boost::gil::at_c<2>(dst(x,y))=0;
boost::gil::at_c<0>(dst(x,y))=boost::gil::at_c<1>(dst(x,y));
boost::gil::at_c<2>(dst(x,y))=boost::gil::at_c<1>(dst(x,y));
*/

	}
}



int main(int argc, char **argv) {
	std::string file_rgb(argv[1]);
	std::string file_pir(argv[2]);
	std::string file_z  (argv[3]);
	std::string file_out(argv[4]);
	
	image_rgb_t  img_rgb;
	image_pir_t  img_pir;
	image_z_t  img_z;
	boost::gil::tiff_read_image(file_rgb, img_rgb);
	boost::gil::tiff_read_image(file_pir, img_pir);
	boost::gil::tiff_read_image(file_z  , img_z  );
	image_ndvi_t img_out(img_rgb.dimensions());

	ndvi(const_view(img_rgb),const_view(img_pir),const_view(img_z),view(img_out));
	boost::gil::tiff_write_view(file_out,const_view(img_out));
}
