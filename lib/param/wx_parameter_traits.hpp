#ifndef WX_PARAMETER_TRAITS
#define WX_PARAMETER_TRAITS

#include <wx/textctrl.h>
#include <wx/checkbox.h>

#include "param/parameters.hpp"

struct wx_parameter_traits { 

  template<typename T> struct control { 
    typedef wxTextCtrl type;
    static inline void set(type *ctrl, const T& t) {
	std::ostringstream oss;
	oss << t;
	ctrl->SetValue(wxString(oss.str().c_str(), *wxConvCurrent));
    }
    static inline void get(type *ctrl, T&t) {
        wxString ws(ctrl->GetLineText(0), *wxConvCurrent);
        std::string s(ws.mb_str());
        std::istringstream iss(s.c_str());
	iss >> t;
//	set(ctrl,t);
    }
  };
};

template<> struct wx_parameter_traits::control<bool> { 
  typedef wxCheckBox type;
  static inline void set(type *ctrl, bool  b) { ctrl->SetValue(b); }
  static inline void get(type *ctrl, bool& b) { b = ctrl->GetValue(); }
};

typedef parameters<wx_parameter_traits> param;

#endif
