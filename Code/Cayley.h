/// \file Cayley.h
/// \brief Declaration of the Cayley pseudo-random number generator.

#ifndef __Cayley__
#define __Cayley__

#include "PowerTable.h"
#include <cinttypes>

/// \brief The Cayley PRNG.
///
/// CCayley is a templated class that allows you to declare instances of Cayley
/// that generate 8-bit, 16-bit, 32-bit, or 64-bit pseudorandom numbers
/// (CCayley<uint8_t>, CCayley<uint16_t>, CCayley<uint32_t>, and CCayley<uint64_t>,
/// respectively) using a symmetric group of any size permitted by the
/// declaration of CPerm. Given access to another PRNG for initialization
/// purposes, it will construct a pair of pseudorandom generators for the
/// symmetric group and pseudorandom masks. It is recommended that this
/// functionality be used during initial exploration and testing, and that
/// a further class be derived from this one using fixed permutation size,
/// generators, and masks that pass any test for pseudorandomness that you
/// might prefer, such as DieHarder.

class CCayley{
  protected:
    uint32_t m_nSize = 0; ///< Size of permutations.

    uint32_t m_nOrder = 0; ///< Order of generators.
    CPowerTable m_nPower[2]; ///< Power tables for a pair of generators.
    CPerm* m_pCurPerm = nullptr; ///< Current permutation.
    
    static const int m_nDelay = 32; ///< Delay size.

    uint64_t m_nDelayLine[m_nDelay] = { 
      0x57ea5e79bb7b58dc, 0x03198e239ff8ba7d,
      0x7779bd2aeb666379, 0x5de2cf0e048781c3,
      0x89faeceacabe7821, 0xbf5a9b43b4e550ae,
      0x24e37a696814c67e, 0x45e199269f6ad385,
      0xf1df54ec42d8fba8, 0x089f41735277a11d,
      0x602c3888033edae0, 0xc71fee188d41a646,
      0x379121f47085af73, 0x9419d15d410b8eeb,
      0x760744f26b4c05b0, 0x3c68c1fb83c9a47e,
      0xa10d29f01e2f225e, 0x39792d6f9700f5cb,
      0xf5016c43b32d066c, 0x692d0a2cbcc083c0,
      0x229bfc31ea3beeff, 0xe9e6fd8bbf4033b8,
      0x74e8c4ad7bd95bd0, 0xeedb9cede270c79b,
      0x9abd1906822b22ac, 0x3b57c6458e330f89,
      0x7fc8519dfd26353d, 0x2874406cd5a54ba0,
      0x9fe7daf93fe577a2, 0x83d1c7bb3d29cd1f,
      0xbb2d2cbb68483f3d, 0x39af233d402946ec
    }; ///< Delay line.

    int m_nTail = 0; ///< Index of last element in delay line.

    void ChooseGenerators(uint64_t (*rnd)(void)); ///< Choose generators.
    void NextPerm(); ///< Compute next permutation.

  public:
    CCayley(uint32_t n); ///< Constructor.
    ~CCayley(); ///< Destructor.

    virtual void srand(uint64_t (*rnd)(void)); ///< Seed the generator.

    CPerm GetGenerator(int i) const; ///< Get generator.
    const CPerm& GetPerm() const; ///< Get current permutation.
    const uint32_t GetSize() const; ///< Get permutation size.
}; //CCayley

#endif