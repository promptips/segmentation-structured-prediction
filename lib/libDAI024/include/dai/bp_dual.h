/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  libDAI is licensed under the terms of the GNU General Public License version
 *  2, or (at your option) any later version. libDAI is distributed without any
 *  warranty. See the file COPYING for more details.
 *
 *  Copyright (C) 2009  Frederik Eaton [frederik at ofb dot net]
 */


/// \file
/// \brief Defines class BP_dual, which is used primarily by BBP.
/// \idea BP_dual replicates a large part of the functionality of BP; would it not be more efficient to adapt BP instead?
/// \author Frederik Eaton


#ifndef __defined_libdai_bp_dual_h
#define __defined_libdai_bp_dual_h


#include <dai/daialg.h>
#include <dai/factorgraph.h>
#include <dai/enum.h>


namespace dai {


/// Calculates both types of BP messages and their normalizers from an InfAlg.
/** BP_dual calculates "dual" versions of BP messages (both messages from factors
 *  to variables and messages from variables to factors), and normalizers, given an InfAlg.
 *  These are computed from the variable and factor beliefs of the InfAlg.
 *  This class is used primarily by BBP.
 *
 *  \author Frederik Eaton
 */
class BP_dual {
    protected:
        /// Convenience label for storing edge properties
        template<class T>
        struct _edges_t : public std::vector<std::vector<T> > {};

        /// Groups together the data structures for storing the two types of messages and their normalizers
        struct messages {
            /// Unnormalized variable->factor messages
            _edges_t<Prob> n;
            /// Normalizers of variable->factor messages
            _edges_t<Real> Zn;
            /// Unnormalized Factor->variable messages
            _edges_t<Prob> m;
            /// Normalizers of factor->variable messages
            _edges_t<Real> Zm;
        };
        /// Stores all messages
        messages _msgs;

        /// Groups together the data structures for storing the two types of beliefs and their normalizers
        struct beliefs {
            /// Unnormalized variable beliefs
            std::vector<Prob> b1;
            /// Normalizers of variable beliefs
            std::vector<Real> Zb1;
            /// Unnormalized factor beliefs
            std::vector<Prob> b2;
            /// Normalizers of factor beliefs
            std::vector<Real> Zb2;
        };
        /// Stores all beliefs
        beliefs _beliefs;

        /// Pointer to the InfAlg object
        const InfAlg *_ia;

        /// Does all necessary preprocessing
        void init();
        /// Allocates space for \a _msgs
        void regenerateMessages();
        /// Allocates space for \a _beliefs
        void regenerateBeliefs();

        /// Calculate all messages from InfAlg beliefs
        void calcMessages();
        /// Update factor->variable message (\a i -> \a I)
        void calcNewM(size_t i, size_t _I);
        /// Update variable->factor message (\a I -> \a i)
        void calcNewN(size_t i, size_t _I);

        /// Calculate all variable and factor beliefs from messages
        void calcBeliefs();
        /// Calculate belief of variable \a i
        void calcBeliefV(size_t i);
        /// Calculate belief of factor \a I
        void calcBeliefF(size_t I);

    public:
        /// Construct BP_dual object from (converged) InfAlg object's beliefs and factors.
        /** \warning A pointer to the the InfAlg object is stored,
         *  so the object must not be destroyed before the BP_dual is destroyed.
         */
        BP_dual( const InfAlg *ia ) : _ia(ia) { init(); }

        /// Returns the underlying FactorGraph
        const FactorGraph& fg() const { return _ia->fg(); }

        /// Returns reference to factor->variable message (\a I -> \a i)
        Prob & 