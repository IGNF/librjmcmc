#ifndef KERNEL_TRAITS_HPP
#define KERNEL_TRAITS_HPP

namespace rjmcmc {

#if USE_VARIADIC_TEMPLATES
template<typename T> struct kernel_traits {
  enum { size = 0 };
};
template <class H, class... T> struct kernel_traits < std::tuple<H, T...> > {
  enum { size = H::size + kernel_traits<std::tuple<T...> >::size };
};

#else

namespace internal {

template<typename T>
  struct kernel_traits_impl {
    enum { size = 0 };
  };

template <class H, class T>
  struct kernel_traits_impl < boost::tuples::cons<H, T> > {
    enum { size = H::size + kernel_traits_impl<T>::size };
  };

} //  namespace internal

template<typename T> struct kernel_traits {
  enum { size = internal::kernel_traits_impl<typename T::inherited>::size };
};

#endif

}; //namespace rjmcmc

#endif // KERNEL_TRAITS_HPP
