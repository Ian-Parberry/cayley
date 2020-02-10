/// \file PowerTable.cpp
/// \brief Implementation of the power table class CPowerTable.

#include "PowerTable.h"

/// Delete all of the permutations we computed to fill the power table.

CPowerTable::~CPowerTable(){
  for(auto p: m_stdPower)
    delete p;
} //destructor

/// Initialize the power table to hold all of the powers of a permutation up 
/// to one less than its order.
/// \param p The initial permutation.

void CPowerTable::Initialize(const CPerm& p){
  //clean up in case we are re-initializing
  for(auto p: m_stdPower)
    delete p;
  m_stdPower.clear();

  //build new table

  const int n = p.GetSize(); //permutation size

  m_stdPower.push_back(new CPerm(n)); //first entry is p^0
  CPerm q(p); //a power of p, which starts out at p^1
  m_nOrder = 1; //its order might be 1 as far as we know

  while(!q.IsIdentity()){ //eventually we'll get back to the identity
    m_stdPower.push_back(new CPerm(q)); //push the current power of p
    q *= p; //next power of p
    m_nOrder++; //next order
  } //while
} //Initialize

/// Reader function for the order of the permutation. Assumes that Initialize() 
/// has been called.
/// \return The order of the permutation whose powers are in this table.

const uint32_t CPowerTable::GetOrder() const{
  return m_nOrder;
} //GetOrder

/// Reader function for the permutation table. Assumes that Initialize() 
/// has been called.
/// \param n An exponent.
/// \return The n'th power of the permutation in this table.

const CPerm& CPowerTable::operator[](int n) const{
  return *(m_stdPower[n]);
} //operator[]

