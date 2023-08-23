/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  libDAI is licensed under the terms of the GNU General Public License version
 *  2, or (at your option) any later version. libDAI is distributed without any
 *  warranty. See the file COPYING for more details.
 *
 *  Copyright (C) 2006-2009  Joris Mooij  [joris dot mooij at libdai dot org]
 *  Copyright (C) 2006-2007  Radboud University Nijmegen, The Netherlands
 */


/// \file
/// \brief Defines class LC, which implements loop corrections for approximate inference


#ifndef __defined_libdai_lc_h
#define __defined_libdai_lc_h


#include <string>
#include <dai/daialg.h>
#include <dai/enum.h>
#include <dai/factorgraph.h>
#include <dai/properties.h>
#include <dai/exceptions.h>


namespace dai {


/// Approximate inference algorithm "Loop Corrected Belief Propagation" [\ref MoK07]
class LC : public DAIAlgFG {
    private:
        /// Stores for each variable the approximate cavity distribution multiplied with the omitted factors
        std::vector<Factor> _pancakes;
        /// Stores for each variable the approximate cavity distribution
        std::vector<Factor> _cavitydists;
        /// _phis[i][_I] corresponds to \f$ \phi^{\setminus i}_I(x_{I \setminus i}) \f$ in the paper
        std::vector<std::vector<Factor> > _phis;
        /// Single variable beliefs
        std::vector<Factor> _beliefs;
        /// Maximum difference encountered so far
        Real _maxdiff;
        /// Number of iterations needed
        size_t _iters;

    public:
        /// Parameters for LC
        struct Properties {
            /// Enumeration of possible ways to initialize the cavities
            /** The following initialization methods are defined:
             *  - FULL calculates the marginal using calcMarginal()
             *  - PAIR calculates only second order interactions using calcPairBeliefs() with \a accurate == \c false
             *  - PAIR2 calculates only second order interactions using calcPairBeliefs() with \a accurate == \c true
             *  - UNIFORM uses a uniform distribution
             */
            DAI_ENUM(CavityType,FULL,PAIR,PAIR2,UNIFORM);

            /// Enumeration of different update schedules
            /** The following update schedules are defined:
             *  - SEQFIX sequential fixed schedule
             *  - SEQRND sequential random schedule
             */
            DAI_ENUM(UpdateType,SEQFIX,SEQRND);

            /// Verbosity (amount of output sent to stderr)
            size_t verbose;

            /// Maximum number of iterations
            size_t maxiter;

            /// Tolerance for convergence test
            Real tol;

            /// Complete or partial reinitialization of cavity graphs?
            bool reinit;

            /// Damping constant (0.0 means no damping, 1.0 is maximum damping)
            Real damping;

            /// How to initialize the cavities
            CavityType cavity;

            /// What update schedule to use
            UpdateType updates;

            /// Name of the algorithm used to initialize the cavi