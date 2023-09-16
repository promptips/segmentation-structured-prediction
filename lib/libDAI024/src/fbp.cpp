/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  libDAI is licensed under the terms of the GNU General Public License version
 *  2, or (at your option) any later version. libDAI is distributed without any
 *  warranty. See the file COPYING for more details.
 *
 *  Copyright (C) 2009  Frederik Eaton  [frederik at ofb dot net]
 *  Copyright (C) 2009-2010  Joris Mooij  [joris dot mooij at libdai dot org]
 */


#include <dai/fbp.h>


#define DAI_FBP_FAST 1


namespace dai {


using namespace std;


const char *FBP::Name = "FBP";


string FBP::identify() const {
    return string(Name) + printProperties();
}


// This code has been copied from bp.cpp, except where comments indicate FBP-specific behaviour
Real FBP::logZ() const {
    Real sum = 0.0;
    for( size_t I = 0; I < nrFactors(); I++ ) {
        sum += (beliefF(I) * factor(I).log(true)).sum();  // FBP
        sum += Weight(I) * beliefF(I).entropy();  // FBP
    }
    for( size_t i = 0; i < nrVars(); ++i ) {
        Real c_i = 0.0;
        daiforeach( const Neighbor &I, nbV(i) )
            c_i += Weight(I);
        sum += (1.0 - c_i) * beliefV(i).entropy();  // FBP
    }
    return sum;
}


// This code has been copied from bp.cpp, except where comments indicate FBP-specific behaviour
Prob FBP::calcIncomingMessageProduct( size_t I, bool without_i, size_t i ) const {
    Real c_I = Weight(I); // FBP: c_I

    Factor Fprod( factor(I) );
    Prob &prod = Fprod.p();

    if( props.logdomain ) {
        prod.takeLog();
        prod /= c_I; // FBP
    } else
        prod ^= (1.0 / c_I); // FBP

    // Calculate pr