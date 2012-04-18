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

#ifndef SALAMON_INITIAL_SCHEDULE_HPP
#define SALAMON_INITIAL_SCHEDULE_HPP

template<typename DirectSampler, typename Configuration>
double salamon_initial_schedule(const DirectSampler& sampler, Configuration& c, unsigned int iterations)
{
  double e1 = 0;
  double e2 = 0;
  double inv = 1./iterations;
  for(unsigned int i=0; i<iterations; ++i) {
    sampler(c);
    double e = c.energy();
    double inv_e = inv*e;
    e1 += inv_e;
    e2 += inv_e*e;
  }
  double std_dev = sqrt(e2-e1*e1);
  return 2*std_dev;
}

#endif // SALAMON_INITIAL_SCHEDULE_HPP
