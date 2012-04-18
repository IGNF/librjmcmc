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
    params->template insert<double>("deccoef",'C',0.999999,"Decrease coefficient");
    params->template insert<int>("nbiter",'I',15000000,"Number of iterations");
//    params->template insert<double>("qtemp",'q',0.5,"Sampler (q) [0;1]");
    params->template insert<int>("nbdump",'d',10000,"Number of iterations between each result display");
//    params->template insert<bool>("dosave",'b',false, "Save intermediate results");
    params->template insert<int>("nbsave",'S',100000,"Number of iterations between each save");
    params->template insert<double>("poisson",'p',200, "Poisson processus parameter");
    params->template insert<double>("maxsize",'m',20, "Maximum rectangle size");
    params->template insert<double>("maxratio",'M',5, "Maximum rectangle aspect ratio");
    params->template insert<double>("pbirth",'B',0.1, "Birth probability");
    params->template insert<double>("pdeath",'D',0.1, "Death probability");
    params->template insert<double>("ponderation_surface",'s',10, "Intersection area weight");
    params->template insert<double>("energy",'e',250, "Cost of an object");
    params->template insert<double>("ponderation_grad",'\0',1, "Image gradient energy weight");
    params->template insert<double>("ponderation_mask",'\0',0, "Image mask energy weight");
    params->template insert<boost::filesystem::path>("dsm",'i',"../data/ZTerrain_c3.tif", "DSM image path");
    params->template insert<boost::filesystem::path>("mask",'\0',"", "mask image path");
    params->template insert<int>("xmin",'x',0, "Xmin");
    params->template insert<int>("ymin",'y',0, "Ymin");
    params->template insert<int>("xmax",'X',1000000, "Xmax");
    params->template insert<int>("ymax",'Y',1000000, "Ymax");
//    params->template insert<int>("subsampling",'u',1, "Subsampling");
//    params->template insert<double>("gaussian",'g',2, "Gaussian filter variance");
    params->template insert<double>("sigmaD",'G',1, "Kernel size for gradients computation");
}
//]

#endif // BUILDING_FOOTPRINT_RECTANGLE_PARAMETERS_INC_HPP
