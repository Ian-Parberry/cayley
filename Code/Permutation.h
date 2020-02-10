/// \file Permutation.h
/// \brief Declaration of the permutation CPerm.

#ifndef __permutation__
#define __permutation__

#include <cinttypes>

#include "uintx_t.h"

/// \brief Permutation.
///
/// A permutation that can be created from an array, from a reverse
/// lexicographic number or pseudorandomly using a PRNG. Functionality includes
/// permutation composition and calculating its reverse lexicographic number. 
/// Permutations can have size at most 256 since it uses a table of 8-bit
/// unsigned integers in order to improve caching by decreasing its memory
/// footprint.

class CPerm{
  private:
    uint8_t* m_nMap; ///< Permutation sends i to m_nMap[i].
    uint8_t m_nSize; ///< Number of things being permuted.

  public:
    std::vector<uint8_t> m_vecCycle; ///< Cycle notation.

    CPerm(uint8_t n); ///< Constructor.
    CPerm(uint8_t n, uintx_t m); ///< Constructor.
    CPerm(uint8_t n, uint8_t init[]); ///< Constructor.
    CPerm(const CPerm& p); ///< Copy constructor.

    ~CPerm(); ///< Destructor.

    void Randomize(uint64_t (*rng)(void)); ///< Set to random permutation.
    void RandomizeOdd(uint64_t (*rng)(void)); ///< Set to random odd permutation.

    void Randomize(uint32_t s[]); ///< Set to random permutation.

    uint8_t GetSize() const; ///< Get size.
    bool IsIdentity() const; ///< Identity permutation test.

    void printmap() const; ///< Print as a map.
    void printnum() const; ///< Print reverse lexicographic number.

    template<class uint> uint GetNum() const; ///< Get reverse lexicographic number.

    uint8_t operator[](uint8_t n) const; ///< Get nth element of map.
    CPerm& operator=(const CPerm& p); ///< Assignment operator.
    const CPerm& operator*=(const CPerm& p); ///< Permutation composition.

    friend bool operator==(const CPerm& p0, const CPerm& p1); ///< Equality test.
}; //CPerm

//binary operators

bool operator==(const CPerm& p0, const CPerm& p1); ///< Is equal to.
bool operator!=(const CPerm& p0, const CPerm& p1); ///< Is not equal to.

#endif