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
encouraged to load and test thesoftware's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and, more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

***********************************************************************/

#ifndef MPP_UNIFORM_BIRTH_DEATH_KERNEL_HPP
#define MPP_UNIFORM_BIRTH_DEATH_KERNEL_HPP

#include "uniform_view.hpp"
#include "rjmcmc/kernel/null_view.hpp"
#include "rjmcmc/kernel/null_variate.hpp"
#include "rjmcmc/kernel/kernel.hpp"

namespace marked_point_process {
    template <typename birth_type>
            struct uniform_birth_death_kernel
    {
        typedef typename birth_type::value_type     value_type;
        typedef typename birth_type::transform_type transform_type;
        typedef rjmcmc::null_view                   view0_type;
        typedef uniform_view<value_type,1>          view1_type;
        typedef typename birth_type::variate_type   variate0_type;
        typedef rjmcmc::null_variate                variate1_type;
        typedef rjmcmc::kernel<view0_type,view1_type,variate0_type,variate1_type,transform_type> type;
    };

    template <typename birth_type>
            typename uniform_birth_death_kernel<birth_type>::type
            make_uniform_birth_death_kernel(const birth_type& b, double p, double q = 0.5)
    {
        typedef uniform_birth_death_kernel<birth_type> res;
        typename res::view0_type view0;
        typename res::view1_type view1;
        typename res::variate1_type variate1;
        return typename res::type(view0,view1,b.variate(),variate1,b.transform(), p, q);
    }

}; // namespace marked_point_process

#endif // MPP_UNIFORM_BIRTH_DEATH_KERNEL_HPP
