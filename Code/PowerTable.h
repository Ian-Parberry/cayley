/// \file PowerTable.h
/// \brief Declaration of the power table class CPowerTable.

#ifndef __powertable__
#define __powertable__

#include <vector>

#include "Permutation.h"

/// \brief Table of all powers of a permutation.
///
/// The power table stores power of permutations as an optimization so that
/// we don't have to keep recomputing them. We just keep computing powers until
/// we get the identity permutation (which we eventually do because groups).

class CPowerTable{
  private:
    std::vector<CPerm*> m_stdPower; ///< Table of powers.
    uint32_t m_nOrder = 0; ///< Order of the underlying permutation.

  public:
    ~CPowerTable(); ///< Destructor.

    void Initialize(const CPerm& p); ///< Initialize.

    const CPerm& operator[](int n) const; ///< Look up power of permutation.
    const uint32_t GetOrder() const; ///< Get the order of the permutation.
}; //CPowerTable

#endif