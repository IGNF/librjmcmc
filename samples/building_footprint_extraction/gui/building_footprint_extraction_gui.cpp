#include "core/building_footprint_extraction_parameters.hpp"
#include "../rjmcmc_building_footprint_extraction_thread.hpp"

#include <wx/app.h>

class building_footprint_extraction_gui : public wxApp
{
public:
	rjmcmc_building_footprint_extraction_frame *m_frame;

private:
	bool OnInit()
	{
		char **my_argv = new char*[argc];
		for (int i=0;i<argc;++i)
		{
			wxString mystr(argv[i]);
			my_argv[i] = new char[mystr.size()+1];
			strcpy(my_argv[i], mystr.To8BitData());
		}
		building_footprint_extraction_parameters::Instance()->parse_command_line(argc,my_argv);
		
		m_frame = new rjmcmc_building_footprint_extraction_frame((wxFrame *)NULL, wxID_ANY, _("RJMCMC building footprint extraction"), wxPoint(50,50), wxSize(800,600) );
		m_frame->Show();
		return true;
	}
};

IMPLEMENT_APP(building_footprint_extraction_gui);
