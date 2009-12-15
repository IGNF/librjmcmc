#ifndef __PATTERN_SINGLETON_HPP__
#define __PATTERN_SINGLETON_HPP__

#include <iostream>

#include <boost/thread/mutex.hpp>

template<typename T> class PatternSingleton
{
protected:
	/// Constructeur vide
	PatternSingleton()
	{
	}
	/// Constructeur par recopie
	PatternSingleton(const PatternSingleton&)
	{
	}
	/// Destructeur
	~PatternSingleton()
	{
	}

public:
	static T* Instance()
	{
		if (!m_singleton)
		{
			// Double-Checked Locking Pattern !
			boost::mutex::scoped_lock lock(m_mutex);
			if (!m_singleton)
				m_singleton = new T;
		}
		return m_singleton;
	}

	//private:
	//    /// Le pointeur sur l'instance unique de l'objet
	static T* m_singleton;
	//    /// Un mutex pour le <i>Double-Checked Locking Pattern</i> !
	static boost::mutex m_mutex;
};

// Initialisation des variables statiques
template<typename T> T* PatternSingleton<T>::m_singleton = 0;
template<typename T> boost::mutex PatternSingleton<T>::m_mutex;

#endif // __PATTERN_SINGLETON_HPP__
