
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
/// \brief Defines TProb<> and Prob classes which represent (probability) vectors (e.g., probability distributions of discrete random variables)


#ifndef __defined_libdai_prob_h
#define __defined_libdai_prob_h


#include <cmath>
#include <vector>
#include <ostream>
#include <algorithm>
#include <numeric>
#include <functional>
#include <dai/util.h>
#include <dai/exceptions.h>


namespace dai {


/// Function object that returns the value itself
template<typename T> struct fo_id : public std::unary_function<T, T> {
    /// Returns \a x
    T operator()( const T &x ) const {
        return x;
    }
};


/// Function object that takes the absolute value
template<typename T> struct fo_abs : public std::unary_function<T, T> {
    /// Returns abs(\a x)
    T operator()( const T &x ) const {
        if( x < (T)0 )
            return -x;
        else
            return x;
    }
};


/// Function object that takes the exponent
template<typename T> struct fo_exp : public std::unary_function<T, T> {
    /// Returns exp(\a x)
    T operator()( const T &x ) const {
        return exp( x );
    }
};


/// Function object that takes the logarithm
template<typename T> struct fo_log : public std::unary_function<T, T> {
    /// Returns log(\a x)
    T operator()( const T &x ) const {
        return log( x );
    }
};


/// Function object that takes the logarithm, except that log(0) is defined to be 0
template<typename T> struct fo_log0 : public std::unary_function<T, T> {
    /// Returns (\a x == 0 ? 0 : log(\a x))
    T operator()( const T &x ) const {
        if( x )
            return log( x );
        else
            return 0;
    }
};


/// Function object that takes the inverse
template<typename T> struct fo_inv : public std::unary_function<T, T> {
    /// Returns 1 / \a x
    T operator()( const T &x ) const {
        return 1 / x;
    }
};


/// Function object that takes the inverse, except that 1/0 is defined to be 0
template<typename T> struct fo_inv0 : public std::unary_function<T, T> {
    /// Returns (\a x == 0 ? 0 : (1 / \a x))
    T operator()( const T &x ) const {
        if( x )
            return 1 / x;
        else
            return 0;
    }
};


/// Function object that returns p*log0(p)
template<typename T> struct fo_plog0p : public std::unary_function<T, T> {
    /// Returns \a p * log0(\a p)
    T operator()( const T &p ) const {
        return p * dai::log0(p);
    }
};


/// Function object similar to std::divides(), but different in that dividing by zero results in zero
template<typename T> struct fo_divides0 : public std::binary_function<T, T, T> {
    /// Returns (\a y == 0 ? 0 : (\a x / \a y))
    T operator()( const T &x, const T &y ) const {
        if( y == (T)0 )
            return (T)0;
        else
            return x / y;
    }
};


/// Function object useful for calculating the KL distance
template<typename T> struct fo_KL : public std::binary_function<T, T, T> {
    /// Returns (\a p == 0 ? 0 : (\a p * (log(\a p) - log(\a q))))
    T operator()( const T &p, const T &q ) const {
        if( p == (T)0 )
            return (T)0;
        else
            return p * (log(p) - log(q));
    }
};


/// Function object useful for calculating the Hellinger distance
template<typename T> struct fo_Hellinger : public std::binary_function<T, T, T> {
    /// Returns (sqrt(\a p) - sqrt(\a q))^2
    T operator()( const T &p, const T &q ) const {
        T x = sqrt(p) - sqrt(q);
        return x * x;
    }
};


/// Function object that returns x to the power y
template<typename T> struct fo_pow : public std::binary_function<T, T, T> {
    /// Returns (\a x ^ \a y)
    T operator()( const T &x, const T &y ) const {
        if( y != 1 )
            return std::pow( x, y );
        else
            return x;
    }
};


/// Function object that returns the maximum of two values
template<typename T> struct fo_max : public std::binary_function<T, T, T> {
    /// Returns (\a x > y ? x : y)
    T operator()( const T &x, const T &y ) const {
        return (x > y) ? x : y;
    }
};


/// Function object that returns the minimum of two values
template<typename T> struct fo_min : public std::binary_function<T, T, T> {
    /// Returns (\a x > y ? y : x)
    T operator()( const T &x, const T &y ) const {
        return (x > y) ? y : x;
    }
};


/// Function object that returns the absolute difference of x and y
template<typename T> struct fo_absdiff : public std::binary_function<T, T, T> {
    /// Returns abs( \a x - \a y )
    T operator()( const T &x, const T &y ) const {
        return dai::abs( x - y );
    }
};


/// Represents a vector with entries of type \a T.
/** It is simply a <tt>std::vector</tt><<em>T</em>> with an interface designed for dealing with probability mass functions.
 *
 *  It is mainly used for representing measures on a finite outcome space, for example, the probability
 *  distribution of a discrete random variable. However, entries are not necessarily non-negative; it is also used to
 *  represent logarithms of probability mass functions.
 *
 *  \tparam T Should be a scalar that is castable from and to dai::Real and should support elementary arithmetic operations.
 */
template <typename T> class TProb {
    private:
        /// The vector
        std::vector<T> _p;

    public:
        /// Enumerates different ways of normalizing a probability measure.
        /**
         *  - NORMPROB means that the sum of all entries should be 1;
         *  - NORMLINF means that the maximum absolute value of all entries should be 1.
         */
        typedef enum { NORMPROB, NORMLINF } NormType;
        /// Enumerates different distance measures between probability measures.
        /**
         *  - DISTL1 is the \f$\ell_1\f$ distance (sum of absolute values of pointwise difference);
         *  - DISTLINF is the \f$\ell_\infty\f$ distance (maximum absolute value of pointwise difference);
         *  - DISTTV is the total variation distance (half of the \f$\ell_1\f$ distance);
         *  - DISTKL is the Kullback-Leibler distance (\f$\sum_i p_i (\log p_i - \log q_i)\f$).
         *  - DISTHEL is the Hellinger distance (\f$\frac{1}{2}\sum_i (\sqrt{p_i}-\sqrt{q_i})^2\f$).
         */
        typedef enum { DISTL1, DISTLINF, DISTTV, DISTKL, DISTHEL } DistType;

    /// \name Constructors and destructors
    //@{
        /// Default constructor (constructs empty vector)
        TProb() : _p() {}

        /// Construct uniform probability distribution over \a n outcomes (i.e., a vector of length \a n with each entry set to \f$1/n\f$)
        explicit TProb( size_t n ) : _p(std::vector<T>(n, (T)1 / n)) {}

        /// Construct vector of length \a n with each entry set to \a p
        explicit TProb( size_t n, T p ) : _p(n, p) {}

        /// Construct vector from a range
        /** \tparam TIterator Iterates over instances that can be cast to \a T
         *  \param begin Points to first instance to be added.
         *  \param end Points just beyond last instance to be added.
         *  \param sizeHint For efficiency, the number of entries can be speficied by \a sizeHint.
         */
        template <typename TIterator>
        TProb( TIterator begin, TIterator end, size_t sizeHint=0 ) : _p() {
            _p.reserve( sizeHint );
            _p.insert( _p.begin(), begin, end );
        }

        /// Construct vector from another vector
        /** \tparam S type of elements in \a v (should be castable to type \a T)
         *  \param v vector used for initialization
         */
        template <typename S>
        TProb( const std::vector<S> &v ) : _p() {
            _p.reserve( v.size() );
            _p.insert( _p.begin(), v.begin(), v.end() );
        }
    //@}

        /// Constant iterator over the elements
        typedef typename std::vector<T>::const_iterator const_iterator;
        /// Iterator over the elements
        typedef typename std::vector<T>::iterator iterator;
        /// Constant reverse iterator over the elements
        typedef typename std::vector<T>::const_reverse_iterator const_reverse_iterator;
        /// Reverse iterator over the elements
        typedef typename std::vector<T>::reverse_iterator reverse_iterator;

    /// \name Iterator interface
    //@{
        /// Returns iterator that points to the first element
        iterator begin() { return _p.begin(); }
        /// Returns constant iterator that points to the first element
        const_iterator begin() const { return _p.begin(); }

        /// Returns iterator that points beyond the last element
        iterator end() { return _p.end(); }
        /// Returns constant iterator that points beyond the last element
        const_iterator end() const { return _p.end(); }

        /// Returns reverse iterator that points to the last element
        reverse_iterator rbegin() { return _p.rbegin(); }
        /// Returns constant reverse iterator that points to the last element
        const_reverse_iterator rbegin() const { return _p.rbegin(); }

        /// Returns reverse iterator that points beyond the first element
        reverse_iterator rend() { return _p.rend(); }
        /// Returns constant reverse iterator that points beyond the first element
        const_reverse_iterator rend() const { return _p.rend(); }
    //@}

    /// \name Queries
    //@{
        /// Returns a const reference to the wrapped vector
        const std::vector<T> & p() const { return _p; }

        /// Returns a reference to the wrapped vector
        std::vector<T> & p() { return _p; }

        /// Returns a copy of the \a i 'th entry
        T operator[]( size_t i ) const {
#ifdef DAI_DEBUG
            return _p.at(i);
#else
            return _p[i];
#endif
        }

        /// Returns reference to the \a i 'th entry
        T& operator[]( size_t i ) { return _p[i]; }

        /// Returns length of the vector (i.e., the number of entries)
        size_t size() const { return _p.size(); }

        /// Accumulate over all values, similar to std::accumulate
        template<typename binOp, typename unOp> T accumulate( T init, binOp op1, unOp op2 ) const {
            T t = init;
            for( const_iterator it = begin(); it != end(); it++ )
                t = op1( t, op2(*it) );
            return t;
        }

        /// Returns the Shannon entropy of \c *this, \f$-\sum_i p_i \log p_i\f$
        T entropy() const { return -accumulate( (T)0, std::plus<T>(), fo_plog0p<T>() ); }

        /// Returns maximum value of all entries
        T max() const { return accumulate( (T)(-INFINITY), fo_max<T>(), fo_id<T>() ); }

        /// Returns minimum value of all entries
        T min() const { return accumulate( (T)INFINITY, fo_min<T>(), fo_id<T>() ); }

        /// Returns sum of all entries
        T sum() const { return accumulate( (T)0, std::plus<T>(), fo_id<T>() ); }

        /// Return sum of absolute value of all entries
        T sumAbs() const { return accumulate( (T)0, std::plus<T>(), fo_abs<T>() ); }

        /// Returns maximum absolute value of all entries
        T maxAbs() const { return accumulate( (T)0, fo_max<T>(), fo_abs<T>() ); }

        /// Returns \c true if one or more entries are NaN
        bool hasNaNs() const {
            bool foundnan = false;
            for( typename std::vector<T>::const_iterator x = _p.begin(); x != _p.end(); x++ )
                if( isnan( *x ) ) {
                    foundnan = true;
                    break;
                }
            return foundnan;
        }

        /// Returns \c true if one or more entries are negative
        bool hasNegatives() const {
            return (std::find_if( _p.begin(), _p.end(), std::bind2nd( std::less<T>(), (T)0 ) ) != _p.end());
        }

        /// Returns a pair consisting of the index of the maximum value and the maximum value itself
        std::pair<size_t,T> argmax() const {
            T max = _p[0];
            size_t arg = 0;
            for( size_t i = 1; i < size(); i++ ) {
              if( _p[i] > max ) {
                max = _p[i];
                arg = i;
              }
            }
            return std::make_pair(arg,max);
        }

        /// Returns a random index, according to the (normalized) distribution described by *this
        size_t draw() {
            Real x = rnd_uniform() * sum();
            T s = 0;
            for( size_t i = 0; i < size(); i++ ) {
                s += _p[i];
                if( s > x )
                    return i;
            }
            return( size() - 1 );
        }

        /// Lexicographical comparison
        /** \pre <tt>this->size() == q.size()</tt>
         */
        bool operator<= (const TProb<T> & q) const {
            DAI_DEBASSERT( size() == q.size() );
            return lexicographical_compare( begin(), end(), q.begin(), q.end() );
        }
    //@}

    /// \name Unary transformations
    //@{
        /// Returns the result of applying operation \a op pointwise on \c *this
        template<typename unaryOp> TProb<T> pwUnaryTr( unaryOp op ) const {
            TProb<T> r;
            r._p.reserve( size() );
            std::transform( _p.begin(), _p.end(), back_inserter( r._p ), op );
            return r;
        }

        /// Returns negative of \c *this
        TProb<T> operator- () const { return pwUnaryTr( std::negate<T>() ); }

        /// Returns pointwise absolute value
        TProb<T> abs() const { return pwUnaryTr( fo_abs<T>() ); }

        /// Returns pointwise exponent
        TProb<T> exp() const { return pwUnaryTr( fo_exp<T>() ); }

        /// Returns pointwise logarithm
        /** If \a zero == \c true, uses <tt>log(0)==0</tt>; otherwise, <tt>log(0)==-Inf</tt>.
         */
        TProb<T> log(bool zero=false) const {
            if( zero )
                return pwUnaryTr( fo_log0<T>() );
            else
                return pwUnaryTr( fo_log<T>() );
        }
        
        /// Returns pointwise inverse
        /** If \a zero == \c true, uses <tt>1/0==0</tt>; otherwise, <tt>1/0==Inf</tt>.
         */
        TProb<T> inverse(bool zero=true) const {
            if( zero )
                return pwUnaryTr( fo_inv0<T>() );
            else
                return pwUnaryTr( fo_inv<T>() );
        }

        /// Returns normalized copy of \c *this, using the specified norm
        /** \throw NOT_NORMALIZABLE if the norm is zero
         */
        TProb<T> normalized( NormType norm = NORMPROB ) const {
            T Z = 0;
            if( norm == NORMPROB )
                Z = sum();
            else if( norm == NORMLINF )
                Z = maxAbs();
            if( Z == (T)0 ) {
                DAI_THROW(NOT_NORMALIZABLE);
                return *this;
            } else
                return pwUnaryTr( std::bind2nd( std::divides<T>(), Z ) );
        }
    //@}

    /// \name Unary operations
    //@{
        /// Applies unary operation \a op pointwise
        template<typename unaryOp> TProb<T>& pwUnaryOp( unaryOp op ) {
            std::transform( _p.begin(), _p.end(), _p.begin(), op );
            return *this;
        }

        /// Draws all entries i.i.d. from a uniform distribution on [0,1)
        TProb<T>& randomize() {
            std::generate( _p.begin(), _p.end(), rnd_uniform );
            return *this;
        }

        /// Sets all entries to \f$1/n\f$ where \a n is the length of the vector
        TProb<T>& setUniform () {
            fill( (T)1 / size() );
            return *this;
        }

        /// Applies absolute value pointwise
        const TProb<T>& takeAbs() { return pwUnaryOp( fo_abs<T>() ); }

        /// Applies exponent pointwise
        const TProb<T>& takeExp() { return pwUnaryOp( fo_exp<T>() ); }

        /// Applies logarithm pointwise
        /** If \a zero == \c true, uses <tt>log(0)==0</tt>; otherwise, <tt>log(0)==-Inf</tt>.
         */
        const TProb<T>& takeLog(bool zero=false) {
            if( zero ) {
                return pwUnaryOp( fo_log0<T>() );
            } else
                return pwUnaryOp( fo_log<T>() );
        }

        /// Normalizes vector using the specified norm
        /** \throw NOT_NORMALIZABLE if the norm is zero
         */
        T normalize( NormType norm=NORMPROB ) {
            T Z = 0;
            if( norm == NORMPROB )
                Z = sum();
            else if( norm == NORMLINF )
                Z = maxAbs();
            if( Z == (T)0 )
                DAI_THROW(NOT_NORMALIZABLE);
            else
                *this /= Z;
            return Z;
        }
    //@}

    /// \name Operations with scalars
    //@{
        /// Sets all entries to \a x
        TProb<T> & fill(T x) {
            std::fill( _p.begin(), _p.end(), x );
            return *this;
        }

        /// Adds scalar \a x to each entry
        TProb<T>& operator+= (T x) {
            if( x != 0 )
                return pwUnaryOp( std::bind2nd( std::plus<T>(), x ) );
            else
                return *this;
        }

        /// Subtracts scalar \a x from each entry
        TProb<T>& operator-= (T x) {
            if( x != 0 )
                return pwUnaryOp( std::bind2nd( std::minus<T>(), x ) );
            else
                return *this;
        }

        /// Multiplies each entry with scalar \a x
        TProb<T>& operator*= (T x) {
            if( x != 1 )
                return pwUnaryOp( std::bind2nd( std::multiplies<T>(), x ) );
            else
                return *this;
        }

        /// Divides each entry by scalar \a x
        TProb<T>& operator/= (T x) {
            DAI_DEBASSERT( x != 0 );
            if( x != 1 )
                return pwUnaryOp( std::bind2nd( std::divides<T>(), x ) );
            else
                return *this;
        }

        /// Raises entries to the power \a x
        TProb<T>& operator^= (T x) {
            if( x != (T)1 )
                return pwUnaryOp( std::bind2nd( fo_pow<T>(), x) );
            else
                return *this;
        }
    //@}

    /// \name Transformations with scalars
    //@{
        /// Returns sum of \c *this and scalar \a x
        TProb<T> operator+ (T x) const { return pwUnaryTr( std::bind2nd( std::plus<T>(), x ) ); }

        /// Returns difference of \c *this and scalar \a x
        TProb<T> operator- (T x) const { return pwUnaryTr( std::bind2nd( std::minus<T>(), x ) ); }

        /// Returns product of \c *this with scalar \a x
        TProb<T> operator* (T x) const { return pwUnaryTr( std::bind2nd( std::multiplies<T>(), x ) ); }

        /// Returns quotient of \c *this and scalar \a x, where division by 0 yields 0
        TProb<T> operator/ (T x) const { return pwUnaryTr( std::bind2nd( fo_divides0<T>(), x ) ); }

        /// Returns \c *this raised to the power \a x
        TProb<T> operator^ (T x) const { return pwUnaryTr( std::bind2nd( fo_pow<T>(), x ) ); }
    //@}

    /// \name Operations with other equally-sized vectors
    //@{
        /// Applies binary operation pointwise on two vectors
        /** \tparam binaryOp Type of function object that accepts two arguments of type \a T and outputs a type \a T
         *  \param q Right operand
         *  \param op Operation of type \a binaryOp
         */
        template<typename binaryOp> TProb<T>& pwBinaryOp( const TProb<T> &q, binaryOp op ) {
            DAI_DEBASSERT( size() == q.size() );
            std::transform( _p.begin(), _p.end(), q._p.begin(), _p.begin(), op );
            return *this;
        }

        /// Pointwise addition with \a q
        /** \pre <tt>this->size() == q.size()</tt>
         */
        TProb<T>& operator+= (const TProb<T> & q) { return pwBinaryOp( q, std::plus<T>() ); }

        /// Pointwise subtraction of \a q
        /** \pre <tt>this->size() == q.size()</tt>
         */
        TProb<T>& operator-= (const TProb<T> & q) { return pwBinaryOp( q, std::minus<T>() ); }

        /// Pointwise multiplication with \a q
        /** \pre <tt>this->size() == q.size()</tt>
         */
        TProb<T>& operator*= (const TProb<T> & q) { return pwBinaryOp( q, std::multiplies<T>() ); }

        /// Pointwise division by \a q, where division by 0 yields 0
        /** \pre <tt>this->size() == q.size()</tt>
         *  \see divide(const TProb<T> &)
         */
        TProb<T>& operator/= (const TProb<T> & q) { return pwBinaryOp( q, fo_divides0<T>() ); }

        /// Pointwise division by \a q, where division by 0 yields +Inf
        /** \pre <tt>this->size() == q.size()</tt>
         *  \see operator/=(const TProb<T> &)
         */
        TProb<T>& divide (const TProb<T> & q) { return pwBinaryOp( q, std::divides<T>() ); }

        /// Pointwise power
        /** \pre <tt>this->size() == q.size()</tt>
         */
        TProb<T>& operator^= (const TProb<T> & q) { return pwBinaryOp( q, fo_pow<T>() ); }
    //@}

    /// \name Transformations with other equally-sized vectors
    //@{
        /// Returns the result of applying binary operation \a op pointwise on \c *this and \a q
        /** \tparam binaryOp Type of function object that accepts two arguments of type \a T and outputs a type \a T
         *  \param q Right operand
         *  \param op Operation of type \a binaryOp
         */
        template<typename binaryOp> TProb<T> pwBinaryTr( const TProb<T> &q, binaryOp op ) const {
            DAI_DEBASSERT( size() == q.size() );
            TProb<T> r;
            r._p.reserve( size() );
            std::transform( _p.begin(), _p.end(), q._p.begin(), back_inserter( r._p ), op );
            return r;
        }

        /// Returns sum of \c *this and \a q
        /** \pre <tt>this->size() == q.size()</tt>
         */
        TProb<T> operator+ ( const TProb<T>& q ) const { return pwBinaryTr( q, std::plus<T>() ); }

        /// Return \c *this minus \a q
        /** \pre <tt>this->size() == q.size()</tt>
         */
        TProb<T> operator- ( const TProb<T>& q ) const { return pwBinaryTr( q, std::minus<T>() ); }

        /// Return product of \c *this with \a q
        /** \pre <tt>this->size() == q.size()</tt>
         */
        TProb<T> operator* ( const TProb<T> &q ) const { return pwBinaryTr( q, std::multiplies<T>() ); }

        /// Returns quotient of \c *this with \a q, where division by 0 yields 0
        /** \pre <tt>this->size() == q.size()</tt>
         *  \see divided_by(const TProb<T> &)
         */
        TProb<T> operator/ ( const TProb<T> &q ) const { return pwBinaryTr( q, fo_divides0<T>() ); }

        /// Pointwise division by \a q, where division by 0 yields +Inf
        /** \pre <tt>this->size() == q.size()</tt>
         *  \see operator/(const TProb<T> &)
         */
        TProb<T> divided_by( const TProb<T> &q ) const { return pwBinaryTr( q, std::divides<T>() ); }

        /// Returns \c *this to the power \a q
        /** \pre <tt>this->size() == q.size()</tt>
         */
        TProb<T> operator^ ( const TProb<T> &q ) const { return pwBinaryTr( q, fo_pow<T>() ); }
    //@}

        /// Performs a generalized inner product, similar to std::inner_product
        /** \pre <tt>this->size() == q.size()</tt>
         */
        template<typename binOp1, typename binOp2> T innerProduct( const TProb<T> &q, T init, binOp1 binaryOp1, binOp2 binaryOp2 ) const {
            DAI_DEBASSERT( size() == q.size() );
            return std::inner_product( begin(), end(), q.begin(), init, binaryOp1, binaryOp2 );
        }
};


/// Returns distance between \a p and \a q, measured using distance measure \a dt
/** \relates TProb
 *  \pre <tt>this->size() == q.size()</tt>
 */
template<typename T> T dist( const TProb<T> &p, const TProb<T> &q, typename TProb<T>::DistType dt ) {
    switch( dt ) {
        case TProb<T>::DISTL1:
            return p.innerProduct( q, (T)0, std::plus<T>(), fo_absdiff<T>() );
        case TProb<T>::DISTLINF:
            return p.innerProduct( q, (T)0, fo_max<T>(), fo_absdiff<T>() );
        case TProb<T>::DISTTV:
            return p.innerProduct( q, (T)0, std::plus<T>(), fo_absdiff<T>() ) / 2;
        case TProb<T>::DISTKL:
            return p.innerProduct( q, (T)0, std::plus<T>(), fo_KL<T>() );
        case TProb<T>::DISTHEL:
            return p.innerProduct( q, (T)0, std::plus<T>(), fo_Hellinger<T>() ) / 2;
        default:
            DAI_THROW(UNKNOWN_ENUM_VALUE);
            return INFINITY;
    }
}


/// Writes a TProb<T> to an output stream
/** \relates TProb
 */
template<typename T> std::ostream& operator<< (std::ostream& os, const TProb<T>& p) {
    os << "[";
    std::copy( p.p().begin(), p.p().end(), std::ostream_iterator<T>(os, " ") );
    os << "]";
    return os;
}


/// Returns the pointwise minimum of \a a and \a b
/** \relates TProb
 *  \pre <tt>this->size() == q.size()</tt>
 */
template<typename T> TProb<T> min( const TProb<T> &a, const TProb<T> &b ) {
    return a.pwBinaryTr( b, fo_min<T>() );
}


/// Returns the pointwise maximum of \a a and \a b
/** \relates TProb
 *  \pre <tt>this->size() == q.size()</tt>
 */
template<typename T> TProb<T> max( const TProb<T> &a, const TProb<T> &b ) {
    return a.pwBinaryTr( b, fo_max<T>() );
}


/// Represents a vector with entries of type dai::Real.
typedef TProb<Real> Prob;


} // end of namespace dai


#endif