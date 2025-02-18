
/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  libDAI is licensed under the terms of the GNU General Public License version
 *  2, or (at your option) any later version. libDAI is distributed without any
 *  warranty. See the file COPYING for more details.
 *
 *  Copyright (C) 2010  Joris Mooij  [joris dot mooij at libdai dot org]
 */


/// \file
/// \brief Defines class TRWBP, which implements Tree-Reweighted Belief Propagation


#ifndef __defined_libdai_trwbp_h
#define __defined_libdai_trwbp_h


#include <string>
#include <dai/daialg.h>
#include <dai/factorgraph.h>
#include <dai/properties.h>
#include <dai/enum.h>
#include <dai/bp.h>


namespace dai {


/// Approximate inference algorithm "Tree-Reweighted Belief Propagation" [\ref WJW03]
/** The Tree-Reweighted Belief Propagation algorithm is like Belief
 *  Propagation, but associates each factor with a scale parameter.
 *  which controls the divergence measure being minimized.
 *
 *  The messages \f$m_{I\to i}(x_i)\f$ are passed from factors \f$I\f$ to variables \f$i\f$. 
 *  The update equation is given by:
 *    \f[ m_{I\to i}(x_i) \propto \sum_{x_{N_I\setminus\{i\}}} f_I(x_I)^{1/c_I} \prod_{j\in N_I\setminus\{i\}} m_{I\to j}^{c_I-1} \prod_{J\in N_j\setminus\{I\}} m_{J\to j}^{c_J} \f]
 *  After convergence, the variable beliefs are calculated by:
 *    \f[ b_i(x_i) \propto \prod_{I\in N_i} m_{I\to i}^{c_I} \f]
 *  and the factor beliefs are calculated by:
 *    \f[ b_I(x_I) \propto f_I(x_I)^{1/c_I} \prod_{j \in N_I} m_{I\to j}^{c_I-1} \prod_{J\in N_j\setminus\{I\}} m_{J\to j}^{c_J} \f]
 *  The logarithm of the partition sum is approximated by:
 *    \f[ \log Z = \sum_{I} \sum_{x_I} b_I(x_I) \big( \log f_I(x_I) - c_I \log b_I(x_I) \big) + \sum_{i} (c_i - 1) \sum_{x_i} b_i(x_i) \log b_i(x_i) \f]
 *  where the variable weights are defined as
 *    \f[ c_i := \sum_{I \in N_i} c_I \f]
 *
 *  \note TRWBP is actually equivalent to FBP
 *  \todo Add nice way to set weights
 *  \todo Merge code of FBP and TRWBP
 */
class TRWBP : public BP {
    protected:
        /// "Edge weights" (indexed by factor ID)
        /** In [\ref WJW03], only unary or pairwise factors are considered.
         *  Here we are more general by having a weight for each factor in the
         *  factor graph. If unary factors have weight 1, and higher-order factors
         *  are absent, then we have the special case considered in [\ref WJW03].
         */
        std::vector<Real> _weight;

    public:
        /// Name of this inference algorithm
        static const char *Name;

    public:
    /// \name Constructors/destructors
    //@{
        /// Default constructor
        TRWBP() : BP(), _weight() {}

        /// Construct from FactorGraph \a fg and PropertySet \a opts
        /** \param opts Parameters @see BP::Properties
         */
        TRWBP( const FactorGraph &fg, const PropertySet &opts ) : BP(fg, opts), _weight() {
            setProperties( opts );
            construct();
        }
    //@}

    /// \name General InfAlg interface
    //@{
        virtual TRWBP* clone() const { return new TRWBP(*this); }
        virtual std::string identify() const;
        virtual Real logZ() const;
    //@}

    /// \name TRWBP accessors/mutators for scale parameters
    //@{
        /// Returns weight corresponding to the \a I 'th factor
        Real Weight( size_t I ) const { return _weight[I]; }

        /// Returns constant reference to vector of all weights
        const std::vector<Real>& Weights() const { return _weight; }

        /// Sets the weight of the \a I 'th factor to \a c
        void setWeight( size_t I, Real c ) { _weight[I] = c; }

        /// Sets the weights of all factors simultaenously
        /** \note Faster than calling setWeight(size_t,Real) for each factor
         */
        void setWeights( const std::vector<Real> &c ) { _weight = c; }

    protected:
        /// Calculate the product of factor \a I and the incoming messages
        /** If \a without_i == \c true, the message coming from variable \a i is omitted from the product
         *  \note This function is used by calcNewMessage() and calcBeliefF()
         */
        virtual Prob calcIncomingMessageProduct( size_t I, bool without_i, size_t i ) const;

        /// Calculates unnormalized belief of variable \a i
        virtual void calcBeliefV( size_t i, Prob &p ) const;

        // Calculates unnormalized belief of factor \a I
        virtual void calcBeliefF( size_t I, Prob &p ) const {
            p = calcIncomingMessageProduct( I, false, 0 );
        }

        // Helper function for constructors
        virtual void construct();
};


} // end of namespace dai


#endif