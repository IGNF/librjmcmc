/*
#ifndef WX_CONTROLER_HPP
#define WX_CONTROLER_HPP

#include "rjmcmc/simulated_annealing.hpp"
#include <boost/thread.hpp>

template<typename Param, typename Configuration>
void create_configuration(Param *p, Configuration *&c);
template<typename Param, typename Sampler>
void create_sampler(Param *p, Sampler *&s);
template<typename Param, typename Temperature>
void create_temperature(Param *p, Temperature *&t);
template<typename Param, typename EndTest>
void create_end_test(Param *p, EndTest *&e);
template<typename Param, typename Visitor>
void init_visitor(Param *p, Visitor& v);

template<typename Configuration, typename Sampler, typename Temperature, typename EndTest, typename Visitor>
class wx_controler : public Controler
{
public:
    virtual void go() {
        release();
        param *p = param::Instance();

        create_configuration(p,m_config);
        create_sampler      (p,m_sampler);
        create_temperature  (p,m_temp);
        create_end_test     (p,m_end_test);
        init_visitor(p,m_visitor);

        m_thread = new boost::thread(
                simulated_annealing<Configuration,Sampler,Temperature,EndTest,Visitor>,
                boost::ref(*m_config), boost::ref(*m_sampler),
                boost::ref(*m_temp),   boost::ref(*m_end_test),
                boost::ref( m_visitor) );
    }

    virtual void toggle_param_visibility() { m_visitor.toggle_param_visibility(); }
    virtual void toggle_chart_visibility() { m_visitor.toggle_chart_visibility(); }
    virtual void stop() { m_end_test->stop(); }

    wx_controler(const std::string& title) :
        m_config(NULL), m_sampler(NULL), m_temp(NULL), m_end_test(NULL), 
        m_visitor(title),  m_thread(NULL)
    {
        m_visitor.controler(this);
    }

    virtual ~wx_controler() {
        release();
    }
private:
    void release()
    {
        if(m_end_test){ m_end_test->stop(); }
        if(m_thread)  { m_thread->join(); delete m_thread; m_thread=NULL; }
        if(m_end_test){ delete m_end_test;m_end_test=NULL; }
        if(m_config)  { delete m_config;  m_config =NULL; }
        if(m_temp)    { delete m_temp;    m_temp   =NULL; }
        if(m_sampler) { delete m_sampler; m_sampler=NULL; }
    }
    Configuration *m_config;
    Sampler *m_sampler;
    Temperature *m_temp;
    EndTest *m_end_test;
    Visitor m_visitor;
    boost::thread *m_thread;
};




/*


    param::Instance()->update_values();

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
        m_controler->go(iclayer->View()->value); // todo
    }
    catch( const exception &e )
    {
        wxString message( e.what() , *wxConvCurrent );
        ::wxMessageBox( message , _("Exception!") , wxICON_ERROR );
    }


#endif
*/
