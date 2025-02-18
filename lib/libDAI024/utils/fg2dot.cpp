
/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  libDAI is licensed under the terms of the GNU General Public License version
 *  2, or (at your option) any later version. libDAI is distributed without any
 *  warranty. See the file COPYING for more details.
 *
 *  Copyright (C) 2006-2010  Joris Mooij  [joris dot mooij at libdai dot org]
 *  Copyright (C) 2006-2007  Radboud University Nijmegen, The Netherlands
 */


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <dai/factorgraph.h>


using namespace dai;
using namespace std;


int main( int argc, char *argv[] ) {
    if( argc != 3 ) {
        // Display help message if number of command line arguments is incorrect
        cout << "This program is part of libDAI - http://www.libdai.org/" << endl << endl;
        cout << "Usage: ./fg2dot <in.fg> <out.dot>" << endl << endl;
        cout << "Converts a libDAI factor graph file to a GraphViz .dot file for visualization." << endl;
        cout << "The .dot file can be converted to .ps (PostScript) by" << endl;
        cout << "'neato -T ps out.dot > out.ps' or by 'dot -T ps out.dot > out.ps'" << endl << endl;
        return 1;
    } else {
        // Read factorgraph
        FactorGraph fg;
        char *infile = argv[1];
        fg.ReadFromFile( infile );

        // Open output file for writing (except if filename equals "-")
        ostream *os = &cout;
        ofstream outfile;
        if( string( argv[2] ) != "-" ) {
            outfile.open( argv[2] );
            if( !outfile.is_open() ) {
                cerr << "Cannot open " << argv[2] << " for writing" << endl;
                return 1;
            }
            os = &outfile;
        } // else, write to cout

        // Write the .dot file
        fg.printDot( *os );

        return 0;
    }
}