#ifndef WX_PARAMETER_TRAITS
#define WX_PARAMETER_TRAITS

#include <wx/textctrl.h>
#include <wx/checkbox.h>

#include "param/parameters.hpp"

/*
	LayerControl::const_iterator it  = m_panel->GetLayerControl()->begin();
    LayerControl::const_iterator end = m_panel->GetLayerControl()->end();
    Layer::ptrLayerType ilayer;
    for(;it!=end && !ilayer;++it) {
         if(boost::dynamic_pointer_cast<ImageLayer>(*it)) ilayer=*it; //use dem tag??
    }
    if(!ilayer) {
        boost::filesystem::path file;
        param::Instance()->get("dem",file);
        ilayer = ImageLayer::CreateImageLayer(file.string());
        if ( !ilayer )
        {
          std::ostringstream oss;
          oss << "File " << file << " does not exist !";
          wxString message( oss.str().c_str() , *wxConvCurrent );
          ::wxMessageBox( message , _("Error !") , wxICON_ERROR );
          return;
        }
    }

    wxPoint p0,p1;
    boost::shared_ptr<VectorLayerGhost> ghost = m_panel->GetVectorLayerGhost();
    if(ghost->m_drawRectangleSelection) {
        wxRect rect = ghost->GetRectangle();
        p0 = ilayer->ToLocal(rect.GetTopLeft    ());
        p1 = ilayer->ToLocal(rect.GetBottomRight());
    } else {
        p0.x = param::Instance()->get<int>("xmin");
        p0.y = param::Instance()->get<int>("ymin");
        p1.x = param::Instance()->get<int>("xmax");
        p1.y = param::Instance()->get<int>("ymax");
        if(p0.x>p1.x) std::swap(p0.x,p1.x);
        if(p0.y>p1.y) std::swap(p0.y,p1.y);
    }
    try
    {
       boost::shared_ptr<Layer> vlayer = boost::shared_ptr<Layer>(new simple_vector_layer("Extracted elements"));
        Layer::ptrLayerType clayer = ilayer->crop(p0.x,p0.y,p1.x,p1.y);
        if(!clayer) {
            std::ostringstream oss;
            oss << "Cropping outside the bounds of " << ilayer->Filename() << " !";
            wxString message( oss.str().c_str() , *wxConvCurrent );
            ::wxMessageBox( message , _("Error !") , wxICON_ERROR );
            return;
        }
        m_panel->AddLayer(clayer);
        m_panel->AddLayer(vlayer);
        vlayer->TranslationX(p0.x+ilayer->TranslationX());
        vlayer->TranslationY(p0.y+ilayer->TranslationY());
        vlayer->ZoomFactor  (     ilayer->ZoomFactor  ());
        clayer->TranslationX(p0.x+ilayer->TranslationX());
        clayer->TranslationY(p0.y+ilayer->TranslationY());
        clayer->ZoomFactor  (     ilayer->ZoomFactor  ());
	
	vlayer->set_line_color(*wxRED);
        vlayer->set_line_style(wxSOLID);
        vlayer->set_line_width(3);
        vlayer->set_polygon_border_color(*wxBLUE);
        vlayer->set_polygon_border_style(wxSOLID);
        vlayer->set_polygon_border_width(3);
        vlayer->set_polygon_inner_color(*wxRED);
        vlayer->set_polygon_inner_style(wxTRANSPARENT);
        vlayer->text_visibility(false);
        
        boost::filesystem::path file(clayer->Filename());
        param::Instance()->set("dem",file);
        param::Instance()->set("xmin",0);
        param::Instance()->set("ymin",0);
        param::Instance()->set("xmax",p1.x-p0.x);
        param::Instance()->set("ymax",p1.y-p0.y);
        
        m_vlayer = vlayer;
        boost::shared_ptr<ImageLayer> iclayer = boost::dynamic_pointer_cast<ImageLayer>(clayer);
		return iclayer->View()->value; // todo
    }
    catch( const exception &e )
    {
        wxString message( e.what() , *wxConvCurrent );
        ::wxMessageBox( message , _("Exception!") , wxICON_ERROR );
    }
*/

struct wx_parameter : public parameter {

    struct updater {
        typedef void result_type;
        template<typename C, typename T> void operator()(C *, const T& t) const {}
        template<typename T, typename C> void operator()(T& t, const C*) const {}

        template<typename T> void operator()(T& t, const wxTextCtrl *ctrl) const {
            if(ctrl==NULL) return;
            wxString ws(ctrl->GetLineText(0), *wxConvCurrent);
            std::string s(ws.mb_str());
            std::istringstream iss(s.c_str());
            iss >> t;
	}

        template<typename T> void operator()(wxTextCtrl *ctrl, const T& t) const {
            if(ctrl==NULL) return;
            std::ostringstream oss;
            oss << t;
            ctrl->SetValue(wxString(oss.str().c_str(), *wxConvCurrent));
	}

        template<typename T> void operator()(bool& t, const wxCheckBox *ctrl) const {
            if(ctrl) t = ctrl->GetValue();
	}

        template<typename T> void operator()(wxCheckBox *ctrl, bool t) const {
            if(ctrl) ctrl->SetValue(t);
	}

    };

public:
    typedef boost::variant<wxCheckBox*,wxTextCtrl*> control_type;

    template<typename T> inline void set(const T& t) { parameter::value() = t; update_control(); }

    inline void update_control() { boost::apply_visitor(updater(),m_control,parameter::value());}
    inline void update_value  () { boost::apply_visitor(updater(),parameter::value(),m_control);}

    template<typename T>
    wx_parameter(const std::string& n, char c, const T& v, const std::string& d)
        : parameter(n,c,v,d) {}

    template<typename T> inline void control(T *c)    { m_control = c; }
    inline       control_type    control()        { return m_control; }
private:
    control_type m_control;
};

template<typename T>
void update_values(parameters<T> *p) {
    for(typename parameters<T>::iterator it = p->begin(); it!=p->end(); ++it)
        it->update_value();
}

template<typename T>
void update_controls(parameters<T> *p) {
    for(typename parameters<T>::iterator it = p->begin(); it!=p->end(); ++it)
        it->update_control();
}

#endif
