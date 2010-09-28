#ifndef ORIENTED_INC_HPP
#define ORIENTED_INC_HPP

#include "image/oriented.hpp"

namespace internal {

    template<typename T, typename U> T min(const T& t, const U& u) { return std::min(t,T(u)); }
    template<typename T, typename U> T max(const T& t, const U& u) { return std::max(t,T(u)); }

    template<typename IsoRectangle, typename T, typename U>
    void clip_bbox(IsoRectangle& bbox, T x0, T y0, U x1, U y1)
    {
        if(bbox.is_degenerate ()) {
            bbox = IsoRectangle(x0,y0,x1,y1);
        } else {
            bbox = IsoRectangle(
                    max(bbox.min().x(),x0),
                    max(bbox.min().y(),y0),
                    min(bbox.max().x(),x1),
                    min(bbox.max().y(),y1)
                    );
        }
    }

}

template<typename IsoRectangle, typename Image>
void clip_bbox(IsoRectangle& bbox, const oriented<Image>& v)
{
    internal::clip_bbox(v.x0(), v.y0(), v.x0()+v.view().width(), v.y0()+v.view().height());
}

#include <boost/gil/extension/io_new/tiff_read.hpp>
template<typename IsoRectangle>
void clip_bbox(IsoRectangle& bbox, const std::string &file)
{
    using namespace boost::gil;
    image_read_info<tiff_tag> info = read_image_info(file, tiff_tag());
    internal::clip_bbox(bbox,0,0,info._width,info._height);
}


template<typename Image>
template<typename ImageIn, typename IsoRectangle>
oriented<Image>::oriented(const oriented<ImageIn>& img, const IsoRectangle& bbox)
    : m_img(img.m_img)
{
    using namespace boost::gil;
    int x0 = (int) bbox.min().x();
    int y0 = (int) bbox.min().y();
    int x1 = (int) bbox.max().x();
    int y1 = (int) bbox.max().y();
    if(m_img) m_view = subimage_view(img.view(),x0-img.x0(),y0-img.y0(),x1-x0,y1-y0);
    m_x0 = x0;
    m_y0 = y0;
}

template<typename Image>
template<typename ImageIn, typename IsoRectangle, typename Fun>
oriented<Image>::oriented(const oriented<ImageIn>& img, const IsoRectangle& bbox, const Fun& fun)
{
    oriented<ImageIn> crop(img,bbox);
    if(crop.img())
    {
        m_img.reset(new Image);
        fun( *m_img, crop.view() );
        m_view = boost::gil::view(*m_img);
    }
    m_x0   = crop.x0();
    m_y0   = crop.y0();
}

template<typename Image>
template<typename Types, typename IsoRectangle, typename Fun>
oriented<Image>::oriented(const oriented<boost::gil::any_image<Types> >& img, const IsoRectangle& bbox, const Fun& fun)
{
    oriented<boost::gil::any_image<Types> > crop(img,bbox);
    if(crop.img())
    {
        m_img.reset(new Image);
        boost::gil::apply_operation( crop.view(), boost::bind(fun,boost::ref(*m_img),_1) );
        m_view = boost::gil::view(*m_img);
    }
    m_x0   = crop.x0();
    m_y0   = crop.y0();
}

template<typename Image>
template<typename IsoRectangle>
oriented<Image>::oriented(const std::string &file, const IsoRectangle& bbox)
{
    using namespace boost::gil;
    int x0 = (int) bbox.min().x();
    int y0 = (int) bbox.min().y();
    int x1 = (int) bbox.max().x();
    int y1 = (int) bbox.max().y();
    try
    {
        m_img.reset(new Image);
        image_read_settings<tiff_tag> irs( point_t(x0,y0), point_t(x1-x0,y1-y0) );
        read_image( file, *m_img, irs );
        m_view = boost::gil::view(*m_img);
    }
    catch(const std::ios_base::failure&) {
        m_img.reset();
    }
    m_x0 = x0;
    m_y0 = y0;
}

template<typename Image>
template<typename IsoRectangle, typename Fun>
oriented<Image>::oriented(const std::string &file, const IsoRectangle& bbox, const Fun& fun)
{
    oriented<rjmcmc::any_image_t> crop(file,bbox);
    if(crop.img())
    {
        m_img.reset(new Image);
        boost::gil::apply_operation( crop.view(), boost::bind(fun,boost::ref(*m_img),_1) );
        m_view = boost::gil::view(*m_img);
    }
    m_x0   = crop.x0();
    m_y0   = crop.y0();
}

#endif // ORIENTED_INC_HPP
