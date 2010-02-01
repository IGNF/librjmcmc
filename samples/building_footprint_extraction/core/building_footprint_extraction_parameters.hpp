#ifndef __BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_HPP__
#define __BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_HPP__

#include <cstdlib>
#include <vector>
#include <sstream>
#include <boost/program_options.hpp>

#include "rjmcmc/pattern_singleton.hpp"

/**
 * @brief Class for specific "Building footprint extraction" application parameters handling
 * @author O. Tournaire
 */
struct building_footprint_extraction_parameters: public PatternSingleton<building_footprint_extraction_parameters>
{
	friend class PatternSingleton<building_footprint_extraction_parameters>;

	void store_string_map(std::map<std::string, std::string> &map_options) const;
	void parse_string_map(const std::map<std::string, std::string> &string_map);
	void parse_config_file(const char *filename);
	bool parse_command_line(int argc, char **argv);
	
	std::string long_name_from_description( const std::string &description );
	std::string description_from_long_name( const std::string &long_name );

	double m_initial_temperature;
	unsigned int m_nb_iterations;
	unsigned int m_nb_iterations_dump;
	unsigned int m_nb_iterations_save;
	bool m_do_save;
	unsigned int m_subsampling;
	double m_decrease_coefficient;
	double m_birth_probability;
	double m_death_probability;
	std::string m_input_data_file_path;
	unsigned int m_running_min_x;
	unsigned int m_running_min_y;
	unsigned int m_running_max_x;
	unsigned int m_running_max_y;
	double m_variance_gaussian_filter;
	double m_rectangle_minimal_size;
	double m_rectangle_maximal_ratio;
	double m_ponderation_surface_intersection;
	double m_individual_energy;
	double m_sigma_d;
	double m_poisson;

private:
	boost::program_options::options_description m_desc;
	boost::program_options::variables_map m_vm;

	building_footprint_extraction_parameters();
	building_footprint_extraction_parameters(const building_footprint_extraction_parameters &);
};

#endif /* __BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_HPP__ */
