#ifndef BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_INC_HPP
#define BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_INC_HPP

#include "param/parameters_inc.hpp"

template<typename T>
void initialize_parameters(T* params)
{
    params->caption("Building footprint extraction parameters");
    params->template insert<double>("temp",'t',150,"Temperature initiale");
    params->template insert<int>("nbiter",'I',15000000,"Nombre d'iterations");
    params->template insert<int>("nbdump",'d',10000,"Nombre d'iterations entre chaque affichage");
    params->template insert<int>("nbsave",'S',10000,"Nombre d'iterations entre chaque sauvegarde");
    params->template insert<bool>("dosave",'b',false, "Sauvegarde des resultats intermediaires");
    params->template insert<double>("deccoef",'C',0.999999,"Coefficient de decroissance");
    params->template insert<double>("pbirth",'B',0.1, "Probabilite de naissance");
    params->template insert<double>("pdeath",'D',0.1, "Probabilite de mort");
    params->template insert<boost::filesystem::path>("dem",'i',"../data/ZTerrain_c3.tif", "Image DEM");
    params->template insert<boost::filesystem::path>("ndvi",'n',"../data/ZTerrain_c3.tif", "Image NDVI");
    params->template insert<int>("xmin",'x',0, "Xmin)");
    params->template insert<int>("ymin",'y',0, "Ymin)");
    params->template insert<int>("xmax",'X',1000000, "Xmax");
    params->template insert<int>("ymax",'Y',1000000, "Ymax");
    params->template insert<int>("subsampling",'u',1, "Sous-échantillonnage");
    params->template insert<double>("gaussian",'g',2, "Variance du filtre gaussien en entree");
    params->template insert<double>("sigmaD",'G',1, "Taille du noyau de flou pour le calcul des gradients");
    params->template insert<double>("minsize",'m',5, "Taille minimale d'un rectangle");
    params->template insert<double>("maxratio",'M',5, "Rapport longueur/largeur maximal d'un rectangle");
    params->template insert<double>("surface",'s',10, "Ponderation de la surface d'intersection");
    params->template insert<double>("ponderation_gradient",'\0',10, "Ponderation de l'energie DEM");
    params->template insert<double>("ponderation_ndvi",'\0',0.5, "Ponderation de l'energie NDVI");

    params->template insert<double>("energy",'e',250, "Energie d'existence d'un objet");
    params->template insert<double>("poisson",'p',200, "Parametre du processus de Poisson");
}

#endif // BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_INC_HPP
