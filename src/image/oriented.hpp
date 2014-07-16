/***********************************************************************
This file is part of the librjmcmc project source files.

Copyright : Institut Geographique National (2008-2012)
Contributors : Mathieu Brédif, Olivier Tournaire, Didier Boldo
email : librjmcmc@ign.fr

This software is a generic C++ library for stochastic optimization.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software. You can use,
modify and/or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty and the software's author, the holder of the
economic rights, and the successive licensors have only limited liability.

In this respect, the user's attention is drawn to the risks associated
with loading, using, modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean that it is complicated to manipulate, and that also
therefore means that it is reserved for developers and experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and, more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

***********************************************************************/

#ifndef ORIENTED_HPP
#define ORIENTED_HPP

#include <boost/gil/image.hpp>
#include <boost/gil/extension/dynamic_image/any_image.hpp>

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


#endif // ORIENTED_HPP
