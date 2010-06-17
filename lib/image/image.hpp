#ifndef RJMCMC_IMAGE_HPP_
#define RJMCMC_IMAGE_HPP_

#include <boost/gil/image.hpp>
#include <boost/shared_ptr.hpp>

template<typename Image>
class oriented {
public:
  typedef typename Image::view_t view_t;
  typedef Image image_t;
  
  oriented(const boost::shared_ptr<Image>& img, const view_t& view, int x0=0, int y0=0)
    : m_img(img), m_view(view), m_x0(x0), m_y0(y0) {}
  oriented(const boost::shared_ptr<Image>& img, int x0=0, int y0=0)
    : m_img(img), m_x0(x0), m_y0(y0) { if(img) m_view = view(*img); }
  oriented() : m_img(), m_view(), m_x0(0), m_y0(0) {}

  inline int x0() const { return m_x0; }
  inline int y0() const { return m_y0; }
  inline const view_t& view() const { return m_view; }
  inline const boost::shared_ptr<Image>& img() const { return m_img; }

  template<typename ImageIn, typename IsoRectangle>
    oriented(const oriented<ImageIn>& img, const IsoRectangle& bbox);
  
  template<typename ImageIn, typename IsoRectangle, typename Fun>
    oriented(const oriented<ImageIn>& img, const IsoRectangle& bbox, const Fun& f);
  
  template<typename Types, typename IsoRectangle, typename Fun>
    oriented(const oriented<boost::gil::any_image<Types> >& img, const IsoRectangle& bbox, const Fun& fun);
    
  template<typename IsoRectangle>
    oriented(const std::string &file, const IsoRectangle& bbox);
  
  template<typename IsoRectangle, typename Fun>
    oriented(const std::string &file, const IsoRectangle& bbox, const Fun& f);
  
private:
  boost::shared_ptr<Image> m_img;
  view_t m_view;
  int m_x0, m_y0;
};

template<typename IsoRectangle>
void clip_bbox(IsoRectangle& bbox, const std::string &file);

template<typename IsoRectangle, typename Image>
void clip_bbox(IsoRectangle& bbox, const oriented<Image>& img);

struct static_cast_color_converter
{
  template<class Src, class Dst>
    void operator()(const Src &src, Dst &dst) const
  {
    typedef typename boost::gil::channel_type<Dst>::type type;
    boost::gil::at_c<0>(dst) = static_cast<type>(boost::gil::at_c<0>(src));
  }
};


#endif // RJMCMC_IMAGE_HPP_
