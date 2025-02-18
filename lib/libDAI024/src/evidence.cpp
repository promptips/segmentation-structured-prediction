
/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  libDAI is licensed under the terms of the GNU General Public License version
 *  2, or (at your option) any later version. libDAI is distributed without any
 *  warranty. See the file COPYING for more details.
 *
 *  Copyright (C) 2009  Charles Vaske  [cvaske at soe dot ucsc dot edu]
 *  Copyright (C) 2009  University of California, Santa Cruz
 */


#include <sstream>
#include <string>
#include <cstdlib>

#include <dai/util.h>
#include <dai/evidence.h>
#include <boost/lexical_cast.hpp>


namespace dai {


void Evidence::addEvidenceTabFile( std::istream &is, FactorGraph &fg ) {
    std::map<std::string, Var> varMap;
    for( std::vector<Var>::const_iterator v = fg.vars().begin(); v != fg.vars().end(); ++v ) {
        std::stringstream s;
        s << v->label();
        varMap[s.str()] = *v;
    }

    addEvidenceTabFile( is, varMap );
}


void Evidence::addEvidenceTabFile( std::istream &is, std::map<std::string, Var> &varMap ) {
    std::string line;
    getline( is, line );
    size_t line_number = 0;

    // Parse header
    std::vector<std::string> header_fields;
    tokenizeString( line, header_fields );
    std::vector<std::string>::const_iterator p_field = header_fields.begin();
    if( p_field == header_fields.end() )
        DAI_THROWE(INVALID_EVIDENCE_FILE,"Empty header line");

    std::vector<Var> vars;
    for( ; p_field != header_fields.end(); ++p_field ) {
        std::map<std::string, Var>::iterator elem = varMap.find( *p_field );
        if( elem == varMap.end() )
            DAI_THROWE(INVALID_EVIDENCE_FILE,"Variable " + *p_field + " not known");
        vars.push_back( elem->second );
    }

    getline(is,line);
    if( is.fail() || line.size() > 0 )
        DAI_THROWE(INVALID_EVIDENCE_FILE,"Expecting empty line");

    // Read samples
    while( getline(is, line) ) {
        line_number++;

        std::vector<std::string> fields;
        tokenizeString( line, fields );
        if( fields.size() != vars.size() )
            DAI_THROWE(INVALID_EVIDENCE_FILE,"Invalid number of fields in line " + boost::lexical_cast<std::string>(line_number));

        Observation sample;
        for( size_t i = 0; i < vars.size(); ++i ) {
            if( fields[i].size() > 0 ) { // skip if missing observation
                if( fields[i].find_first_not_of("0123456789") != std::string::npos )
                    DAI_THROWE(INVALID_EVIDENCE_FILE,"Invalid state " + fields[i] + " in line " + boost::lexical_cast<std::string>(line_number));
                size_t state = atoi( fields[i].c_str() );
                if( state >= vars[i].states() )
                    DAI_THROWE(INVALID_EVIDENCE_FILE,"State " + fields[i] + " too large in line " + boost::lexical_cast<std::string>(line_number));
                sample[vars[i]] = state;
            }
        }
        _samples.push_back( sample );
    } // finished sample line
}


} // end of namespace dai