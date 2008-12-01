#ifndef __PATTERN_SINGLETON_HPP__
#define __PATTERN_SINGLETON_HPP__

#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

/**
* @brief Classe de <a href="http://home.earthlink.net/~huston2/dp/singleton.html">Singleton</a>.
*
* Un singleton permet de n'avoir qu'une seule et unique instance d'un objet. Le constructeur de la classe est alors prive,
* et pour obtenir cette l'instance existante de l'objet, on fait appel a la methode Instance().<br>
* L'utilisation est assez simple. Par exemple, on peut deriver la classe MySingleton :<br>
* @code
* class SingletonObject : public PatternSingleton<SingletonObject>
* {
*	friend class PatternSingleton<SingletonObject>;
*	// ...
* }
* @endcode
* Il est important que le constructeur soit cache. L'utilisateur ne peut ainsi pas l'appeler et doit obligatoirement
* passer par la methode PatternSingleton::Instance(). Avec les shared_ptr de boost, cela donne :<br>
* @code
* boost::shared_ptr<SingletonObject> unique_object = SingletonObject::Instance();
* @endcode
* @author OTO
* @date Sat Sep 08 16:29:00 2007
*/
template <typename T> class PatternSingleton
{
protected:
	/// Constructeur vide 
	PatternSingleton () {;}
	/// Constructeur par recopie : non implemente
	PatternSingleton (const PatternSingleton& ) ;
	/// Destructeur
	~PatternSingleton () {;}

public:
	/// Cette methode permet d'obtenir ou de creer l'instance unique de l'objet
	static T * Instance ()
	{
		static T myT;

		return &myT; 
	}
};

#endif // __PATTERN_SINGLETON_HPP__
