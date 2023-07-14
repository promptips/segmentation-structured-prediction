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
/// \brief Defines the general interface for inference methods in libDAI (classes InfAlg, DaiAlg<>, DaiAlgFG and DaiAlgRG).


#ifndef __defined_libdai_daialg_h
#define __defined_libdai_daialg_h


#include <string>
#include <iostream>
#include <vector>
#include <dai/factorgraph.h>
#include <dai/regiongraph.h>
#include <dai/properties.h>


namespace dai {


/// InfAlg is an abstract base class, defining the common interface of all inference algorithms in libDAI.
/** \idea General marginalization functions like calcMarginal() now copy a complete InfAlg object. Instead,
 *  it would make more sense that they construct a new object without copying the FactorGraph or RegionGraph.
 *  Or they can simply be made methods of the general InfAlg class.
 *  \idea Use a PropertySet as output of an InfAlg, instead of functions like maxDiff() and Iterations().
 */
class InfAlg {
    public:
    /// \name Constructors/destructors
    //@{
        /// Virtual destruc