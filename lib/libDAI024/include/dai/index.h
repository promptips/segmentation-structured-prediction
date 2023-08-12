/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  libDAI is licensed under the terms of the GNU General Public License version
 *  2, or (at your option) any later version. libDAI is distributed without any
 *  warranty. See the file COPYING for more details.
 *
 *  Copyright (C) 2002       Martijn Leisink  [martijn@mbfys.kun.nl]
 *  Copyright (C) 2006-2009  Joris Mooij      [joris dot mooij at libdai dot org]
 *  Copyright (C) 2002-2007  Radboud University Nijmegen, The Netherlands
 */


/// \file
/// \brief Defines the IndexFor, multifor, Permute and State classes, which all deal with indexing multi-dimensional arrays


#ifndef __defined_libdai_index_h
#define __defined_libdai_index_h


#include <vector>
#include <algorithm>
#include <map>
#include <dai/varset.h>


namespace dai {


/// Tool for looping over the states of several variables.
/** The class IndexFor is an important tool for indexing Factor entries.
 *  Its usage can best be explained by an example.
 *  Assume \a indexVars, \a forVars are both VarSet 's.
 *  Then the following code:
 *  \code
 *      IndexFor i( indexVars, forVars );
 *      size_t iter = 0;
 *      for( ; i.valid(); i++, iter++ ) {
 *          cout << "State of forVars: " << calcState( forVars, iter ) << "; ";
 *          cout << "state of indexVars: " << calcState( indexVars, long(i) ) << endl;
 *      }
 *  \endcode
 *  loops over all joint states of the variables in \a forVars,
 *  and <tt>(long)i</tt> equals the linear index of the corresponding
 *  state of \a indexVars, where the variables in \a indexVars that are
 *  not in \a forVars assume their zero'th value.
 *  \idea Optimize all indices as follows: keep a cache of all (or only
 *  relatively small) indices that have been computed (use a hash). Then,
 *  instead of computing on the fly, use the precomputed ones. Here the
 *  labels of the variables don't matter, but the ranges of the variables do.
 */
class IndexFor {
    private:
        /// The current linear index corresponding to the state of indexVars
        long                _index;

        /// For each variable in forVars, the amount of change in _index
        std::vector<long>   _sum;

        /// For each variable in forVars, the current state
        std::vector<size_t> _state;

        /// For each variable in forVars, its number of possible values
        std::vector<size_t> _ranges;

    public:
        /// Default constructor
        IndexFor() : _index(-1) {}

        /// Construct IndexFor object from \a indexVars and \a forVars
        IndexFor( const VarSet& indexVars, const VarSet& forVars ) : _state( forVars.size(), 0 ) {
            long sum = 1;

            _ranges.reserve( forVars.size() );
            _sum.reserve( forVars.size() );

            VarSet::const_iterator j = forVars.begin();
            for( VarSet::const_iterator i = indexVars.begin(); i != indexVars.end(); ++i ) {
                for( ; j != forVars.end() && *j <= *i; ++j ) {
                    _ranges.push_back( j->states() );
                    _sum.push_back( (*i == *j) ? sum : 0 );
                }
                sum *= i->states();
          