/// \file Cayley32.h
/// \brief Declaration of the Cayley32 pseudo-random number generator.

#ifndef __Cayley32__
#define __Cayley32__

#include "Cayley.h"

/// \brief The Cayley PRNG over \f$S_{32}\f$ with pseudo-random generators.
///
/// A 64-bit Cayley PRNG with permutation size 32 and pseudo-random generators
/// generated using the Mersenne Twister.

class Cayley32e: public CCayley{
  public:
    Cayley32e(); ///< Constructor.
    uint64_t rand(); ///< Generate 64 pseudo-random bits.
}; //Cayley32e

//////////////////////////////////////////////////////////////////////////////

/// \brief The Cayley PRNG over \f$S_{32}\f$ with fixed generators.
///
/// A 64-bit Cayley PRNG with permutation size 32 and fixed generators.

class Cayley32: public Cayley32e{
  private:
    void ChooseGenerators(); ///< Choose generators.

  public:
    Cayley32(); ///< Constructor.
    void srand(uintx_t& seed); ///< Seed the generator.
}; //Cayley32

#endif