/// \file Cayley32.cpp
/// \brief Implementation of the Cayley32 pseudo-random number generator.

#include "Includes.h"
#include "Cayley32.h"

//////////////////////////////////////////////////////////////////////////////
//Cayley32e functions

Cayley32e::Cayley32e(): CCayley(32){
} //constructor

/// Generate a pseudo-random permutation and map it to a 64-bit unsigned int,
/// as follows. Update the current permutation, then exclusive-or together
/// the product of the permutation map entries times 32 random strings.
/// These strings are fixed in this implementation but they should be replaced
/// and not be made public to protect against reverse engineering.
/// \return A pseudo-random 64-bit unsigned integer.

uint64_t Cayley32e::rand(){
  NextPerm(); //update current permutation
  CPerm& perm = *m_pCurPerm; //shorthand for the current permutation
  
  uint64_t num = 
    (perm[ 0]*0x0d7e11b44d8e8161) ^ (perm[ 1]*0x3d43a82e494a9972) ^ 
    (perm[ 2]*0x71b941e4c1557ec7) ^ (perm[ 3]*0x56bf34559248d37c) ^ 
    (perm[ 4]*0x445db48764d3c5c8) ^ (perm[ 5]*0xd2b96a4ba16b5c56) ^ 
    (perm[ 6]*0xb2bbaa127223e3da) ^ (perm[ 7]*0x3232fd669cd2918e) ^ 
    (perm[ 8]*0x331d3d1bd619e971) ^ (perm[ 9]*0x74b3680644295539) ^ 
    (perm[10]*0xb491addfb1af0f5b) ^ (perm[11]*0xa3caa6455b313d54) ^ 
    (perm[12]*0xb6257e45a726fa52) ^ (perm[13]*0xd413cd54747f43b1) ^ 
    (perm[14]*0x706873eeb3583e05) ^ (perm[15]*0x3fd0d37b7f24589c) ^ 
    (perm[16]*0xc04cb886d76abce0) ^ (perm[17]*0x3ecfdec3d519aedd) ^ 
    (perm[18]*0xbb4f1bccb25c3e51) ^ (perm[19]*0xb1b80c550732d50f) ^ 
    (perm[20]*0x7c5015c795b5c8c2) ^ (perm[21]*0xb2d8190706c770a8) ^
    (perm[22]*0x0d7e11b44d8e8161) ^ (perm[23]*0x3d43a82e494a9972) ^ 
    (perm[24]*0x71b941e4c1557ec7) ^ (perm[25]*0x56bf34559248d37c) ^
    (perm[26]*0x445db48764d3c5c8) ^ (perm[27]*0xd2b96a4ba16b5c56) ^
    (perm[28]*0xb2bbaa127223e3da) ^ (perm[29]*0x3232fd669cd2918e) ^
    (perm[30]*0x331d3d1bd619e971) ^ (perm[31]*0x74b3680644295539); 
        
  m_nDelayLine[m_nTail] = num; //enter into delay line
  m_nTail = (m_nTail + 1)%m_nDelay; //advance delay line

  return num^m_nDelayLine[m_nTail]; //strengthen pseudo-random number
} //rand

//////////////////////////////////////////////////////////////////////////////
//Cayley32 functions

/// The default constructor declares Cayley32 to be a 64-bit instance of
/// CCayley with a permutation size of 32.

Cayley32::Cayley32(){
} //constructor

/// Choose the generators and initialize the power tables. 
/// A pair of fixed generators is used here, but they should be replaced
/// and not be made public to protect against reverse engineering.
/// CCayley::ChooseGenerators() will find generators that have a high
/// probability of being strong.

void Cayley32::ChooseGenerators(){ 
  uintx_t gen0("350F1C2036E12600512A8400920E");
  uintx_t gen1("EEDC82EE2D472B430D13E5066CD5B");
  
  m_nPower[0].Initialize(CPerm(32, gen0)); 
  m_nPower[1].Initialize(CPerm(32, gen1)); 
  
  assert(m_nPower[0].GetOrder() == m_nOrder);
  assert(m_nPower[1].GetOrder() == m_nOrder);
} //ChooseGenerators

/// Initialize the pseudorandom number generator by choosing the generators
/// and choosing a pseudorandom initial permutation.
/// \param seed Seed value.

void Cayley32::srand(uintx_t& seed){
  ChooseGenerators();
  *m_pCurPerm = CPerm(32, seed); //pseudorandom initial permutation
} //srand
