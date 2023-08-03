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
/// \brief Defines the Exception class and macros for throwing exceptions and doing assertions


#ifndef __defined_libdai_exceptions_h
#define __defined_libdai_exceptions_h


#include <exception>
#include <stdexcept>
#include <string>
#include <iostream>


/// Used by DAI_THROW
#define DAI_QUOTE(x) #x

/// Used by DAI_THROW
#define DAI_TOSTRING(x) DAI_QUOTE(x)

/// Macro that simplifies throwing an exception with a useful default error message. 
/** The error message consists of a description of the exception, the source 
 *  code file and line number where the exception has been thrown.
 *  \param cod Corresponds to one of the enum values of dai::Exception::Code
 *
 *  \par Example:
 *  \code
 *  DAI_THROW(NOT_IMPLEMENTED);
 *  \endcode
 */
#define DAI_THROW(cod) throw dai::Exception(dai::Exception::cod, std::string(__FILE__ ", line " DAI_TOSTRING(__LINE__)))

/// Macro that simplifies thr