#ifndef __PATTERN_SINGLETON_HPP__
#define __PATTERN_SINGLETON_HPP__

#include <iostream>

#include <boost/thread/mutex.hpp>
/**
* @brief <a href="http://home.earthlink.net/~huston2/dp/singleton.html">Singleton</a> pattern.
*
* @code
* // Declaration of a class which requires to have a unique instance
* class my_singleton_object : public PatternSingleton<my_singleton_object>
* {
*	friend class PatternSingleton<my_singleton_object>;
*	// ...
* };
* @endcode
* @code
* // Use of the class
* boost::shared_ptr<my_singleton_object> unique_object = my_singleton_object::Instance();
* @endcode
* @author O. Tournaire
*/
template<typename T> class PatternSingleton
{
protected:
    /// Empty constructor
    PatternSingleton() {}
    /// Copy constructor
    PatternSingleton(const PatternSingleton&) {}
    /// Destructor
    ~PatternSingleton() {}

public:
    /// This is tha main method
    static T* instance()
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
    /// A pointer on the unique instance
    static T* m_singleton;
    /// Mutex used in the <i>Double-Checked Locking Pattern</i>
    static boost::mutex m_mutex;
};

// Initialisation des variables statiques
template<typename T> T* PatternSingleton<T>::m_singleton = 0;
template<typename T> boost::mutex PatternSingleton<T>::m_mutex;

#endif // __PATTERN_SINGLETON_HPP__
