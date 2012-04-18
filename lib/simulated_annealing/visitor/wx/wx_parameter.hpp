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

#ifndef WX_PARAMETER_TRAITS
#define WX_PARAMETER_TRAITS

#ifdef WIN32
	#pragma warning(disable : 4251)
	#pragma warning(disable : 4275)
#endif

#include <sstream>

#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/choice.h>

#include "param/parameter.hpp"

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
            oss.precision(std::numeric_limits<double>::digits10+1);
            oss << t;
            ctrl->SetValue(wxString(oss.str().c_str(), *wxConvCurrent));
	}

        template<typename T> void operator()(bool& t, const wxCheckBox *ctrl) const {
            if(ctrl) t = ctrl->GetValue();
        }

        template<typename T> void operator()(wxCheckBox *ctrl, bool t) const {
            if(ctrl) ctrl->SetValue(t);
        }

        template<typename T> void operator()(T& t, const wxChoice *ctrl) const {
            if(ctrl==NULL) return;
            wxString ws(ctrl->GetStringSelection());
            std::string s(ws.mb_str());
            std::istringstream iss(s.c_str());
            iss >> t;
        }

        template<typename T> void operator()(wxChoice *ctrl, const T& t) const {
            if(ctrl==NULL) return;
            std::ostringstream oss;
            oss.precision(std::numeric_limits<double>::digits10+1);
            oss << t;
            wxString s(oss.str().c_str(), *wxConvCurrent);
            if(ctrl->FindString(s) == wxNOT_FOUND)
                ctrl->Append(s);
            ctrl->SetStringSelection(s);
        }
    };

public:
    typedef boost::variant<wxCheckBox*,wxTextCtrl*,wxChoice*> control_type;

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
