/// \file Cayley.cpp
/// \brief Implementation of the Cayley pseudo-random number generator.

#include "Includes.h"
#include "Cayley.h"

///////////////////////////////////////////////////////////////////////////////
//Useful constants

#pragma region constants

/// \brief Landau's function \f$g(n)\f$ for \f$0 <= n \leq 64\f$.

extern const uint32_t g_nLandau[] = { 
       1,       1,       2,       3,       4, //g(0-4)
       6,       6,      12,      15,      20, //g(5-9)
      30,      30,      60,      60,      84, //g(10-14)
     105,     140,     210,     210,     420, //g(15-19)
     420,     420,     420,     840,     840, //g(20-24)
    1260,    1260,    1540,    2310,    2520, //g(25-29)
    4620,    4620,    5460,    5460,    9240, //g(30-34)
    9240,   13860,   13860,   16380,   16380, //g(35-39)
   27720,   30030,   32760,   60060,   60060, //g(40-44)
   60060,   60060,  120120,  120120,  180180, //g(45-49)
  180180,  180180,  180180,  360360,  360360, //g(50-54)
  360360,  360360,  471240,  471240,  556920, //g(55-59)
 1021020, 1021020, 1141140, 1141140, 2042040  //g(60-64)
}; //g_nLandau

#pragma endregion constants

///////////////////////////////////////////////////////////////////////////////
//CCayley functions

/// Construct the current permutation and set the order of the generators using
/// the Landau table.
/// \param n The permutation size.

CCayley::CCayley(uint32_t n):
  m_nSize(n)
{
  assert(n < 64); //for safety: this is the size of our Landau table.
  m_nOrder = g_nLandau[n];
  m_pCurPerm = new CPerm(n);
} //constructor

/// The destructor.

CCayley::~CCayley(){
  delete m_pCurPerm;
} //destructor

/// Get generator.
/// \param i Generator number, either 0 or 1.
/// \return Hex string of generator reverse lexicographic number.

CPerm CCayley::GetGenerator(int i) const{
  assert(i == 0 || i == 1);
  return CPerm(m_nPower[i][1]);
} //Generator

/// Choose a pair of pseudorandom odd permutations of maximal order that have
/// no common fixed point. It is unlikely that a pair of random permutations
/// will have the same fixed point but it is possible. Build tables of powers
/// of these generators to speed up the computation.
/// \param rnd An external PRNG for seeding.

void CCayley::ChooseGenerators(uint64_t (*rnd)(void)){
  assert(rnd != nullptr); //safety

  CPerm p(m_nSize); //current permutation
  bool ok = false; //whether chosen permutations are ok

  while(!ok){
    do{ //choose the first generator; a max-order pseudo-random permutation
      p.Randomize(rnd); //choose a pseudorandom permutation 
      m_nPower[0].Initialize(p); //initialize its power table and its order
    }while(m_nPower[0].GetOrder() < m_nOrder); //insist on max order

    do{ //choose the second generator; a max-order pseudo-random odd permutation
      p.RandomizeOdd(rnd); //choose a pseudorandom odd permutation
      m_nPower[1].Initialize(p); //initialize its power table and its order
    }while(m_nPower[1].GetOrder() < m_nOrder); //insist on max order

    //reject the generators if they have a common fixed point

    ok = true; //ok so far

    const CPerm& p0 = m_nPower[0][1]; //shorthand for first generator
    const CPerm& q0 = m_nPower[1][1]; //shorthand for second generator

    for(uint32_t i=0; i<m_nSize; i++)
      ok = ok && !(p0[i] == i && q0[i] == i);
  } //while
} //ChooseGenerators

/// Initialize the pseudo-random number generator by choosing the generators
/// and the initial permutation.
/// \param rand An external random number generator to use as a seed.

void CCayley::srand(uint64_t (*rand)(void)){
  ChooseGenerators(rand); //random generators
  m_pCurPerm->Randomize(rand); //random permutations
} //Initialize

/// Get the current permutation.
/// \return Const reference to the current permutation.

const CPerm& CCayley::GetPerm() const{
  return *m_pCurPerm;
} //GetPerm

/// Get the current permutation.
/// \return The permutation size.

const uint32_t CCayley::GetSize() const{
  return m_nSize;
} //GetSize

/// Generate the next pseudo-random permutation.

void CCayley::NextPerm(){
  static unsigned int i = 0; //generator parity; determines current generator 
  CPerm& perm = *m_pCurPerm; //shorthand for the current permutation
  const uint32_t n = m_nDelayLine[m_nTail]%m_nOrder; //exponent

  perm *= m_nPower[i][n]; //multiply by generator i to the power n
  i ^= 1; //flip generator parity
  assert(i < 2); //safety
} //NextPerm

