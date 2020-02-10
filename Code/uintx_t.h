/// \file uintx_t.h
/// \brief Declaration of the extensible unsigned integer class.

#ifndef __uintx_t__
#define __uintx_t__

#include "Includes.h"
#include <cinttypes>

/// \brief The extensible unsigned integer class.
///
/// An extensible unsigned integer can have arbitrary length.

class uintx_t{ 
  private:
    uint32_t* m_pData; ///< Array of 32-bit words, least significant first.
    int m_nSize; ///< Number of words in m_pData.

    void loadstring(const char* string); ///< Load hex string.
    void reallocate(const int s); ///< Reallocate space for s words.
    void grow(const int s); ///< Grow space for s words.
    void normalize(); ///< Remove leading zero words.
    int bitcount(); ///< Number of bits.

  public:
    uintx_t(); ///< Constructor.
    uintx_t(int); ///< Constructor.
    uintx_t(const char*); ///< Constructor.
    uintx_t(const uintx_t&); ///< Copy constructor.

    ~uintx_t(); ///< Destructor

    std::string GetString() const; ///< Get as string.

    //assignment operators

    uintx_t& operator=(const uintx_t&); ///< Assignment.
    uintx_t& operator=(const int); ///< Assignment.
    uintx_t& operator=(const char*); ///< Assignment.

    //addition operators

    uintx_t& operator+=(const uintx_t&); ///< Add to.
    uintx_t& operator+=(const int&); ///< Add to.
    friend uintx_t operator+(uintx_t, uintx_t); ///< Addition.

    //multiplication operators

    uintx_t& operator*=(const uintx_t&); ///< Multiply by.
    friend uintx_t operator*(uintx_t, uintx_t); ///< Multiplication.
    friend uintx_t operator*(uintx_t, int); ///< Multiplication.
    friend uintx_t operator*(int, uintx_t); ///< Multiplication.
    friend uintx_t operator*(uintx_t, uint32_t); ///< Multiplication.
    friend uintx_t operator*(uint32_t, uintx_t); ///< Multiplication.

    //comparison operators

    friend bool operator>(uintx_t, uintx_t); ///< Greater than.
    friend bool operator>(uintx_t, int); ///< Greater than.

    friend bool operator>=(uintx_t, uintx_t); ///< Greater than or equal.
    friend bool operator>=(uintx_t, int); ///< Greater than or equal.

    friend bool operator<(uintx_t, uintx_t); ///< Less than.
    friend bool operator<(uintx_t, int); ///< Less than.

    friend bool operator<=(uintx_t, uintx_t); ///< Less than or equal.
    friend bool operator<=(uintx_t, int); ///< Less than or equal.

    friend bool operator==(uintx_t, uintx_t); ///< Equal to.
    friend bool operator==(uintx_t, uint32_t); ///< Equal to.
    friend bool operator==(uint32_t, uintx_t); ///< Equal to.

    friend bool operator!=(uintx_t, uintx_t); ///< Not equal to.
    friend bool operator!=(uintx_t, uint32_t); ///< Not equal to.
    friend bool operator!=(uint32_t, uintx_t); ///< Not equal to.

    //bit shift operators

    uintx_t& operator<<=(const int); ///< Left shift by.
    friend uintx_t operator<<(uintx_t, int); ///< Left shift.

    uintx_t& operator>>=(const int); ///< Right shift by.
    friend uintx_t operator>>(uintx_t, int); ///< Right shift.

    //bitwise operators

    friend int operator&(uintx_t, int); ///< Bit-wise AND.
    friend uintx_t operator&(uintx_t, const uintx_t&); ///< Bit-wise AND.

    friend int operator|(uintx_t, int); ///< Bit-wise OR operator.
    friend uintx_t operator|(uintx_t, const uintx_t&); ///< Bit-wise OR.

    //subtraction operators

    uintx_t& operator-=(const uintx_t&); ///< Subtract from.
    friend uintx_t operator-(uintx_t, uintx_t); ///< Subtraction.

    //division operators

    uintx_t& operator/=(const uintx_t&); ///< Divide by.
    friend uintx_t operator/(uintx_t, uintx_t); ///< Division.
    friend uintx_t operator/(uintx_t, uint32_t); ///< Division.

    uintx_t& operator%=(const uintx_t&); ///< Remainder.
    friend uintx_t operator%(uintx_t, uintx_t); ///< Remainder.
    friend uintx_t operator%(uintx_t, uint32_t); ///< Remainder.

    //type casts

    operator uint32_t(); ///< Cast to uint32_t.
    operator uint64_t(); ///< Cast to uint64_t.
};

#endif
