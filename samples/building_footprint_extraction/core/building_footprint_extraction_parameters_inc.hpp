#ifndef BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_INC_HPP
#define BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_INC_HPP

#include "param/parameters_inc.hpp"

template<typename T>
parameters<T>::parameters()
{
	caption("Building footprint extraction parameters");
	insert<double>("temp",'t',150,"Temperature initiale");
	insert<int>("nbiter",'I',15000000,"Nombre d'iterations");
	insert<int>("nbdump",'d',10000,"Nombre d'iterations entre chaque affichage");
	insert<int>("nbsave",'S',10000,"Nombre d'iterations entre chaque sauvegarde");
	insert<bool>("dosave",'b',false, "Sauvegarde des resultats intermediaires");
	insert<bool>("test",'b',true, "test");
	insert<double>("deccoef",'C',0.999999,"Coefficient de decroissance");
	insert<double>("pbirth",'B',0.1, "Probabilite de naissance");
	insert<double>("pdeath",'D',0.1, "Probabilite de mort");
	insert<boost::filesystem::path>("dem",'i',"../data/ZTerrain_c3.tif", "Image DEM");
	insert<boost::filesystem::path>("ndvi",'v',"../data/ZTerrain_c3.tif", "Image NDVI");
	insert<int>("xmin",'x',0, "Xmin)");
	insert<int>("ymin",'y',0, "Ymin)");
	insert<int>("xmax",'X',1000000, "Xmax");
	insert<int>("ymax",'Y',1000000, "Ymax");
	insert<int>("subsampling",'u',1, "Sous-échantillonnage");
	insert<double>("gaussian",'g',2, "Variance du filtre gaussien en entree");
	insert<double>("sigmaD",'G',1, "Taille du noyau de flou pour le calcul des gradients");
	insert<double>("minsize",'m',5, "Taille minimale d'un rectangle");
	insert<double>("maxratio",'M',5, "Rapport longueur/largeur maximal d'un rectangle");
	insert<double>("surface",'s',10, "Ponderation de la surface d'intersection");
	insert<double>("ponderation_gradient",'\0',10, "Ponderation de l'energie DEM");
	insert<double>("ponderation_ndvi",'\0',0.5, "Ponderation de l'energie NDVI");
	insert<double>("energy",'e',250, "Energie d'existence d'un objet");
	insert<double>("poisson",'p',200, "Parametre du processus de Poisson");
}

#endif // BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_INC_HPP
