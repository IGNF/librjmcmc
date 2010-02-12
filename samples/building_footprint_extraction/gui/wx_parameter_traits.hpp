#ifndef WX_PARAMETER_TRAITS
#define WX_PARAMETER_TRAITS

#include <wx/textctrl.h>
#include <wx/checkbox.h>

struct wx_parameter_traits { 

  template<typename T> struct control { 
    typedef wxTextCtrl type;
    static inline void set(type *ctrl, const T& t) {
	std::ostringstream oss;
	oss << t;
	ctrl->SetValue(wxString(oss.str().c_str(), *wxConvCurrent));
    }
    static inline void get(type *ctrl, T&t) {
	std::istringstream iss(ctrl->GetLineText(0).fn_str());//.data());
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

#endif
