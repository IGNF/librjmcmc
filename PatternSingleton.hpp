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
	static boost::shared_ptr<T> Instance ()
	{
		if ( m_singleton == boost::shared_ptr<T>() )
		{
			// Double-Checked Locking Pattern !
			boost::mutex::scoped_lock lock(m_mutex);
			if ( m_singleton == boost::shared_ptr<T>() )
				m_singleton = boost::shared_ptr<T> (new T);
		}
		return (static_cast< boost::shared_ptr<T> > (m_singleton));
	}

private:
	/// Le pointeur sur l'instance unique de l'objet
	static boost::shared_ptr<T> m_singleton;
	/// Un mutex pour le <i>Double-Checked Locking Pattern</i> !
	static boost::mutex m_mutex;
};

// Initialisation des variables statiques
template <typename T> boost::shared_ptr<T> PatternSingleton<T>::m_singleton;
template <typename T> boost::mutex PatternSingleton<T>::m_mutex;

#endif // __PATTERN_SINGLETON_HPP__
