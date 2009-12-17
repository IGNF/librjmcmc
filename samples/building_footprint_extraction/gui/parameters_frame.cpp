/*
 * ParametersFrame.cpp
 *
 *  Created on: 4 déc. 2008
 *      Author: olivier
 */

#include <wx/sizer.h>
#include "core/building_footprint_extraction_parameters.hpp"
#include "parameters_frame.hpp"

#define ID_CHECK_SAVE	1

parameters_frame::parameters_frame(wxWindow *parent, wxWindowID id, const wxString& title, long style, const wxPoint& pos, const wxSize& size) :
	wxDialog(parent, id, title, pos, size, style)
{
	wxBoxSizer *test_sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* inner_sizer = new wxBoxSizer(wxVERTICAL);

	wxScrolledWindow* wnd = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("scroll"));
	wnd->SetScrollbars(20, 20, 50, 50);

	std::map<std::string, std::string> options;
	building_footprint_extraction_parameters::Instance()->store_string_map(options);
	std::map<std::string, std::string>::const_iterator it;
	for (it = options.begin(); it != options.end(); ++it)
	{
		// Handle particular bool values cases
		if ( building_footprint_extraction_parameters::Instance()->long_name_from_description( it->first ) == "dosave" )
			continue;
			
		wxBoxSizer *sizerH = new wxBoxSizer(wxHORIZONTAL);
				
		m_parameters.push_back(std::make_pair(new wxStaticText(wnd, -1, wxString(it->first.c_str(), *wxConvCurrent)), new wxTextCtrl(wnd, -1, wxString(it->second.c_str(), *wxConvCurrent))));
		if ( building_footprint_extraction_parameters::Instance()->long_name_from_description( it->first ) == "nbsave" )
		{
			wxCheckBox *check_do_save = new wxCheckBox(wnd, ID_CHECK_SAVE, wxT("") /*wxString(it->first.c_str(), *wxConvCurrent)*/ );
			check_do_save->SetValue( building_footprint_extraction_parameters::Instance()->m_do_save );
			sizerH->Add(check_do_save, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
			check_do_save->Connect(ID_CHECK_SAVE, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(parameters_frame::on_check_save), NULL, this);
		}
		/*
		else
		{
		}
		* */
		sizerH->Add(m_parameters.back().first, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
		sizerH->Add(m_parameters.back().second, 1, wxALL, 5);
		
		// Test if the option needs a browse button
		if ( building_footprint_extraction_parameters::Instance()->long_name_from_description( it->first ) == "input" )
		{
			wxButton *browse_input_button = new wxButton(wnd,wxID_OPEN,wxT("Browse input"));
			sizerH->Add(browse_input_button, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
			browse_input_button->Connect(wxID_OPEN, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(parameters_frame::on_browse_input_button), NULL, this);
		}
		
		inner_sizer->Add(sizerH, 0, wxEXPAND);		
		sizerH->Layout();
	}

	wnd->SetSizer(inner_sizer);
	wnd->SetAutoLayout(true);
	wnd->Layout();

	test_sizer->Add(wnd, 1, wxEXPAND | wxALL, 5);
	SetSizer(test_sizer);
}

void parameters_frame::on_browse_input_button(wxCommandEvent& event)
{
	wxString wildcard;
	wildcard << _("Image files ");
	wildcard << wxT("(*.tif;*.tiff;*.png;*.jpg;*.jpeg)|*.tif;*.tiff;*.TIF;*.TIFF;*.png;*.PNG;*.jpg;*.jpeg;*.JPG;*.JPEG|");
	wildcard << wxT("TIFF (*.tif;*.tiff;*.TIF;*.TIFF)|*.tif;*.tiff;*.TIF;*.TIFF|");
	wildcard << wxT("PNG (*.png;*.PNG)|*.png;*.PNG|");
	wildcard << wxT("JPEG (*.jpg;*.jpeg;*.JPG;*.JPEG)|*.jpg;*.jpeg;*.JPG;*.JPEG|");
	wxString str;
	wxFileDialog *fileDialog = new wxFileDialog(this, _("Choose DEM"), wxT(""), wxT(""), wildcard, wxFD_OPEN|wxFD_CHANGE_DIR );
	if (fileDialog->ShowModal() == wxID_OK)
	{
		std::string full_path( fileDialog->GetPath().To8BitData() );
		building_footprint_extraction_parameters::Instance()->m_input_data_file_path = full_path;
		// Update text control
		parameters_const_iterator itb = m_parameters.begin(), ite = m_parameters.end();
		for (;itb!=ite;++itb)
		{
			if ( building_footprint_extraction_parameters::Instance()->long_name_from_description( std::string(itb->first->GetLabel().To8BitData()) ) == "input" )
			{
				itb->second->SetValue( wxString(full_path.c_str(), *wxConvCurrent) );
			}
		}
	}
}

void parameters_frame::on_check_save(wxCommandEvent& event)
{
	building_footprint_extraction_parameters::Instance()->m_do_save = event.IsChecked();
}
