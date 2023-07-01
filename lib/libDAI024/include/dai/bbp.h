/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  libDAI is licensed under the terms of the GNU General Public License version
 *  2, or (at your option) any later version. libDAI is distributed without any
 *  warranty. See the file COPYING for more details.
 *
 *  Copyright (C) 2009       Frederik Eaton [frederik at ofb dot net]
 *  Copyright (C) 2009-2010  Joris Mooij [joris dot mooij at libdai dot org]
 */


/// \file
/// \brief Defines class BBP, which implements Back-Belief-Propagation
/// \todo Clean up code


#ifndef ___defined_libdai_bbp_h
#define ___defined_libdai_bbp_h


#include <vector>
#include <utility>

#include <dai/prob.h>
#include <dai/daialg.h>
#include <dai/factorgraph.h>
#include <dai/enum.h>
#include <dai/bp_dual.h>


namespace dai {


/// Enumeration of several cost functions that can be used with BBP
/** \note This class is meant as a base class for BBPCostFunction, which provides additional functionality.
 */
DAI_ENUM(BBPCostFunctionBase,CFN_GIBBS_B,CFN_GIBBS_B2,CFN_GIBBS_EXP,CFN_GIBBS_B_FACTOR,CFN_GIBBS_B2_FACTOR,CFN_GIBBS_EXP_FACTOR,CFN_VAR_ENT,CFN_FACTOR_ENT,CFN_BETHE_ENT);


/// Predefined cost functions that can be used with BBP
class BBPCostFunction : public BBPCostFunctionBase {
    public:
        /// Default constructor
        BBPCostFunction() : BBPCostFunctionBase() {}

        /// Construct from BBPCostFunctionBase \a x
        BBPCostFunction( const BBPCostFunctionBase &x ) : BBPCostFunctionBase(x) {}

        /// Returns whether this cost function depends on having a Gibbs state
        bool needGibbsState() const;

        /// Evaluates cost function in state \a stateP using the information in inference algorithm \a ia
        Real evaluate( const InfAlg &ia, const std::vector<size_t> *stateP ) const;

        /// Assignment operator
        BBPCostFunction& operator=( const BBPCostFunctionBase &x ) {
            if( this != &x ) {
                (BBPCostFunctionBase)*this = x;
            }
            return *this;
        }
};


/// Implements BBP (Back-Belief-Propagation) [\ref EaG09]
/** \author Frederik Eaton
 */
class BBP {
    private:
    /// \name Input variables
    //@{
        /// Stores a BP_dual helper object
        BP_dual _bp_dual;
        /// Pointer to the factor graph
        const FactorGraph *_fg;
        /// Pointer to the approximate inference algorithm (currently, only BP objects are supported)
        const InfAlg *_ia;
    //@}

    /// \name Output variables
    //@{
        /// Variable factor adjoints
        std::vector<Prob> _adj_psi_V;
        /// Factor adjoints
        std::vector<Prob> _adj_psi_F;
        /// Variable->factor message adjoints (indexed [i][_I])
        std::vector<std::vector<Prob> > _adj_n;
        /// Factor->variable message adjoints (indexed [i][_I])
        std::vector<std::vector<Prob> > _adj_m;
        /// Normalized variable belief adjoints
        std::vector<Prob> _adj_b_V;
        /// Normalized factor belief adjoints
        std::vector<Prob> _adj_b_F;
    //@}

    /// \name Internal state variables
    //@{
        /// Initial variable factor adjoints
        std::vector<Prob> _init_adj_psi_V;
        /// Initial factor adjoints
        std::vector<Prob> _init_adj_psi_F;

        /// Unnormalized variable->factor message adjoint (indexed [i][_I])
        std::vector<std::vector<Prob> > _adj_n_unnorm;
        /// Unnormalized factor->variable message adjoint (indexed [i][_I])
        std::vector<std::vector<Prob> > _adj_m_unnorm;
        /// Updated normalized variable->factor message adjoint (indexed [i][_I])
        std::vector<std::vector<Prob> > _new_adj_n;
        /// Updated normalized factor->variable message adjoint (indexed [i][_I])
        std::vector<std::vector<Prob> > _new_adj_m;
        /// Unnormalized variable belief adjoints
        std::vector<Prob> _adj_b_V_unnorm;
        /// Unnormalized factor belief adjoints
        std::vector<Prob> _adj_b_F_unnorm;

        /// _Tmsg[i][_I] (see eqn. (41) in [\ref EaG09])
        std::vector<std::vector<Prob > > _Tmsg;
        /// _Umsg[I][_i] (see eqn. (42) in [\ref EaG09])
        std::vector<std::vector<Prob > > _Umsg;
        /// _Smsg[i][_I][_j] (see eqn. (43) in [\ref EaG09])
        std::vector<std::vector<std::vector<Prob > > > _Smsg;
        /// _Rmsg[I][_i][_J] (see eqn. (44) in [\ref EaG09])
        std::vector<std::vector<std::vector<Prob > > > _Rmsg;

        /// Number of iterations done
        size_t _iters;
    //@}

    /// \name Index cache management (for performance)
    //@{
        /// Index type
        typedef std::vector<size_t>  _ind_t;
        /// Cached indices (indexed [i][_I])
        std::vector<std::vector<_ind_t> >  _indices;
        /// Prepares index cache _indices
        /** \see bp.cpp
         */
        void RegenerateInds();
        /// Returns an index from the cache
        const _ind_t& _index(size_t i, size_t _I) const { return _indices[i][_I]; }
    //@}

    /// \name Initialization helper functions
    //@{
        /// Calculate T values; see eqn. (41) in [\ref EaG09]
        void RegenerateT();
        /// Calculate U values; see eqn. (42) in [\ref EaG09]
        void RegenerateU();
        /// Calculate S values; see eqn. (43) in [\ref EaG09]
        void RegenerateS();
        /// Calculate R values; see eqn. (44) in [\ref EaG09]
        void RegenerateR();
        /// Calculate _adj_b_V_unnorm and _adj_b_F_unnorm from _adj_b_V and _adj_b_F
        void RegenerateInputs();
        /// Initialise members for factor adjoints
        /** \pre RegenerateInputs() should be called first
         */
        void RegeneratePsiAdjoints();
        /// Initialise members for message adjoints for parallel algorithm
        /** \pre RegenerateInputs() should be called first
         */
        void RegenerateParMessageAdjoints();
        /// Initialise members for message adjoints for sequential algorithm
        /** Same as RegenerateMessageAdjoints, but calls sendSeqMsgN rather
         *  than updating _adj_n (and friends) which are unused in the sequential algorithm.
         *  \pre RegenerateInputs() should be called first
         */
        void RegenerateSeqMessageAdjoints();
        /// Called by \a init, recalculates intermediate values
        void Regenerate();
    //@}

    /// \name Accessors/mutators
    //@{
        /// Returns reference to T value; see eqn. (41) in [\ref EaG09]
        Prob & T(size_t i, size_t _I) { return _Tmsg[i][_I]; }
        /// Returns constant reference to T value; see eqn. (41)