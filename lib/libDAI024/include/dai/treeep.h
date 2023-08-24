
/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  libDAI is licensed under the terms of the GNU General Public License version
 *  2, or (at your option) any later version. libDAI is distributed without any
 *  warranty. See the file COPYING for more details.
 *
 *  Copyright (C) 2006-2010  Joris Mooij  [joris dot mooij at libdai dot org]
 *  Copyright (C) 2006-2007  Radboud University Nijmegen, The Netherlands
 */


/// \file
/// \brief Defines class TreeEP, which implements Tree Expectation Propagation
/// \todo Clean up the TreeEP code (exploiting that a large part of the code
/// is just a special case of JTree).


#ifndef __defined_libdai_treeep_h
#define __defined_libdai_treeep_h


#include <vector>
#include <string>
#include <dai/daialg.h>
#include <dai/varset.h>
#include <dai/regiongraph.h>
#include <dai/factorgraph.h>
#include <dai/clustergraph.h>
#include <dai/weightedgraph.h>
#include <dai/jtree.h>
#include <dai/properties.h>
#include <dai/enum.h>


namespace dai {


/// Approximate inference algorithm "Tree Expectation Propagation" [\ref MiQ04]
class TreeEP : public JTree {
    private:
        /// Maximum difference encountered so far
        Real                  _maxdiff;
        /// Number of iterations needed
        size_t                _iters;

    public:
        /// Parameters for TreeEP
        struct Properties {
            /// Enumeration of possible choices for the tree
            /** The two possibilities are:
             *  - \c ORG: take the maximum spanning tree where the weights are crude
             *            estimates of the mutual information between the nodes;
             *  - \c ALT: take the maximum spanning tree where the weights are upper
             *            bounds on the effective interaction strengths between pairs of nodes.
             */
            DAI_ENUM(TypeType,ORG,ALT);

            /// Verbosity (amount of output sent to stderr)
            size_t verbose;

            /// Maximum number of iterations
            size_t maxiter;

            /// Tolerance for convergence test
            Real tol;

            /// How to choose the tree
            TypeType type;
        } props;

        /// Name of this inference method
        static const char *Name;

    private:
        /// Stores the data structures needed to efficiently update the approximation of an off-tree factor.
        /** The TreeEP object stores a TreeEPSubTree object for each off-tree factor.
         *  It stores the approximation of that off-tree factor, which is represented 
         *  as a distribution on a subtree of the main tree.
         */
        class TreeEPSubTree {
            private:
                /// Outer region pseudomarginals (corresponding with the \f$\tilde f_i(x_j,x_k)\f$ in [\ref MiQ04])
                std::vector<Factor>  _Qa;
                /// Inner region pseudomarginals (corresponding with the \f$\tilde f_i(x_s)\f$ in [\ref MiQ04])
                std::vector<Factor>  _Qb;
                /// The junction tree (stored as a rooted tree)
                RootedTree           _RTree;
                /// Index conversion table for outer region indices (_Qa[alpha] corresponds with Qa[_a[alpha]] of the supertree)
                std::vector<size_t>  _a;        
                /// Index conversion table for inner region indices (_Qb[beta] corresponds with Qb[_b[beta]] of the supertree)
                std::vector<size_t>  _b;
                /// Pointer to off-tree factor
                const Factor *       _I;
                /// Variables in off-tree factor
                VarSet               _ns;
                /// Variables in off-tree factor which are not in the root of this subtree
                VarSet               _nsrem;
                /// Used for calculating the free energy
                Real                 _logZ;

            public:
            /// \name Constructors/destructors
            //@{
                /// Default constructor
                TreeEPSubTree() : _Qa(), _Qb(), _RTree(), _a(), _b(), _I(NULL), _ns(), _nsrem(), _logZ(0.0) {}

                /// Copy constructor
                TreeEPSubTree( const TreeEPSubTree &x ) : _Qa(x._Qa), _Qb(x._Qb), _RTree(x._RTree), _a(x._a), _b(x._b), _I(x._I), _ns(x._ns), _nsrem(x._nsrem), _logZ(x._logZ) {}

                /// Assignment operator
                TreeEPSubTree & operator=( const TreeEPSubTree& x ) {
                    if( this != &x ) {
                        _Qa         = x._Qa;
                        _Qb         = x._Qb;
                        _RTree      = x._RTree;
                        _a          = x._a;
                        _b          = x._b;
                        _I          = x._I;
                        _ns         = x._ns;