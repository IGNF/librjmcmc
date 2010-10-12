#ifndef BUILDING_FOOTPRINT_RECTANGLE_PARAMETERS_INC_HPP
#define BUILDING_FOOTPRINT_RECTANGLE_PARAMETERS_INC_HPP

//[building_footprint_rectangle_parameters_lib_include
#include "param/parameters_inc.hpp"
//]

//[building_footprint_rectangle_initialize_parameters
template<typename T>
void initialize_parameters(T* params)
{
    params->caption("Building footprint extraction parameters");
    params->template insert<double>("temp",'t',150,"Initial Temperature");
    params->template insert<double>("qtemp",'q',0.5,"Sampler (q) [0;1]");
    params->template insert<int>("nbiter",'I',15000000,"Number of iterations");
    params->template insert<int>("nbdump",'d',10000,"Number of iterations between each result display");
    params->template insert<bool>("dosave",'b',false, "Save intermediate results");
    params->template insert<int>("nbsave",'S',10000,"Number of iterations between each save");
    params->template insert<double>("deccoef",'C',0.999999,"Decrease coefficient");
    params->template insert<double>("pbirth",'B',0.1, "Birth probability");
    params->template insert<double>("pdeath",'D',0.1, "Death probability");
    params->template insert<boost::filesystem::path>("dsm",'i',"../data/ZTerrain_c3.tif", "DSM image path");
    params->template insert<int>("xmin",'x',0, "Xmin");
    params->template insert<int>("ymin",'y',0, "Ymin");
    params->template insert<int>("xmax",'X',1000000, "Xmax");
    params->template insert<int>("ymax",'Y',1000000, "Ymax");
    params->template insert<int>("subsampling",'u',1, "Subsampling");
    params->template insert<double>("gaussian",'g',2, "Gaussian filter variance");
    params->template insert<double>("sigmaD",'G',1, "Taille du noyau de flou pour le calcul des gradients");
    params->template insert<double>("minsize",'m',5, "Minimum rectangle size");
    params->template insert<double>("maxratio",'M',5, "Maximum rectangle aspect ratio");
    params->template insert<double>("surface",'s',10, "Intersection area weight");
    params->template insert<double>("ponderation_grad",'\0',1, "Image gradient energy weight");
    params->template insert<double>("energy",'e',250, "Cost of an object");
    params->template insert<double>("poisson",'p',200, "Poisson processus parameter");
}
//]

#endif // BUILDING_FOOTPRINT_RECTANGLE_PARAMETERS_INC_HPP
