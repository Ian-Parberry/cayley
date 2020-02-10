/// \file Permutation.cpp
/// \brief Implementation of the permutation CPerm.

#include <string.h> //for memcpy
#include "Permutation.h"
#include "Includes.h"

////////////////////////////////////////////////////////////////////////////
//Constructors and destructors.

#pragma region structors

/// Construct the identity permutation, that is, the permutation that maps
/// everything to itself.
/// \param n Size of permutation.

CPerm::CPerm(uint8_t n): m_nSize(n){
  m_nMap = new uint8_t[n];

  for(uint8_t i=0; i<m_nSize; i++)
    m_nMap[i] = i;
} //constructor

/// Use the method of Hall and Knuth, "Combinatorial analysis and computers",
/// The American Mathematical Monthly 72(2):21-28, 1965, to construct a 
/// permutation from its reverse lexicographic number using mixed-radix
/// arithmetic. This is the inverse of operator uintx_t().
/// \param n Size of permutation.
/// \param m Reverse lexicographic number of permutation.

CPerm::CPerm(uint8_t n, uintx_t m): CPerm(n){
  uintx_t* factorial = new uintx_t[n]; //factorials
  factorial[0] = 1;

  for(uint8_t i=1; i<n; i++)
    factorial[i] = uint32_t(i)*factorial[i - 1];

  const uintx_t nfactorial = uint32_t(n)*factorial[n - 1];
  m %= nfactorial; //prevent m from being larger than n!

  uint32_t* c = new uint32_t[n]; //c[i] will be the number of p[0..i-1] < p[i]
  uint32_t* d = new uint32_t[n]; //helper

  for(int i=0; i<n; i++){ //initialize c and d
    c[i] = 0;
    d[i] = i;
  } //for
  
  //compute c[0..n-1]
  
  for(int i=n-1; i>0; i--){
    c[i] = uint32_t(m/factorial[i]);
    m = m%factorial[i];
  } //for
  
  //use c[] and d[] to compute the permutation
  
  for(int i=n-1; i>=0; i--){
    m_nMap[i] = d[c[i]];
    for(int j=c[i]; j<i; j++)
      d[j] = d[j + 1];
  } //for

  //clean up and exit
  
  delete [] factorial;
  delete [] d;
  delete [] c;
} //constructor

/// Construct a permutation from a permutation table. It is assumed that the
/// permutation table has the right size and does indeed describe a permutation.
/// \param n Size of permutation.
/// \param init Permutation table.

CPerm::CPerm(uint8_t n, uint8_t init[]): CPerm(n){
  for(uint8_t i=0; i<m_nSize; i++)
    m_nMap[i] = init[i];
} //constructor

/// Construct a deep copy of a permutation.
/// \param p The permutation to copy from.

CPerm::CPerm(const CPerm& p): CPerm(p.m_nSize){
  for(uint8_t i=0; i<m_nSize; i++)
    m_nMap[i] = p.m_nMap[i];
} //copy constructor

/// The destructor.

CPerm::~CPerm(){
  delete [] m_nMap;
} //destructor

#pragma endregion structors

////////////////////////////////////////////////////////////////////////////
// Randomization functions.

#pragma region randomization

/// Use the Mersenne Twister to choose a pseudo-random permutation with a
/// uniform distribution, that is, each permutation is equally likely.
/// \param rng An external random number generator to use as a seed.

void CPerm::Randomize(uint64_t (*rng)(void)){
  for(uint8_t i=0; i<m_nSize-1; i++){ 
    const int j = rng()%((uint64_t)m_nSize - i) + i; //random target
    std::swap(m_nMap[i], m_nMap[j]);
  } //for
} //Randomize

/// Use the Mersenne Twister to choose a pseudo-random odd permutation with a 
/// uniform distribution, that is, each odd permutation is equally likely.
/// \param rng An external random number generator to use as a seed.

void CPerm::RandomizeOdd(uint64_t (*rng)(void)){
  int nCount = 0; //number of transpositions

  for(uint8_t i=0; i<m_nSize-2; i++){ //all except last pair to enforce oddness
    const int j = rng()%((uint64_t)m_nSize - i) + i; //random target

    if(i != j){ //not self-target
      std::swap(m_nMap[i], m_nMap[j]); //make the transposition
      ++nCount; //one more transposition, obviously
    } //if
  } //for

  if(nCount ^ 1) //even permutation, ruh-roh
    std::swap(m_nMap[m_nSize - 2], m_nMap[m_nSize - 1]); //fix it
} //RandomizeOdd

/// Choose a pseudo-random permutation with a uniform distribution, that is, 
/// each permutation is equally likely. The parameter consists of an array
/// of numbers that are assumed to be pseudorandom. It is also assumed that
/// the array contains at least m_nSize numbers.
/// \param s An array of pseudorandom numbers.

 void CPerm::Randomize(uint32_t s[]){
  for(uint8_t i=0; i<m_nSize-1; i++){ 
    const uint8_t j = s[i]%(m_nSize - i) + i; //random target
    printf("(%u, %u) ", i, j);
    std::swap(m_nMap[i], m_nMap[j]);
  } //for
  printf("\n");
 } //Randomize
 
#pragma endregion randomization

//////////////////////////////////////////////////////////////////////////////
// Reader functions and tests.

#pragma region readersandtests

/// Reader function for the size of the permutation, that is, the number of
/// elements being permuted.
/// \return Permutation size.

uint8_t CPerm::GetSize() const{
  return m_nSize;
} //GetSize

/// Test whether this is the identity permutation, that is, the permutation
/// that maps everything to itself.
/// \return true If this is the identity permutation.

bool CPerm::IsIdentity() const{
  for(uint8_t i=0; i<m_nSize; i++)
    if(m_nMap[i] != i)return false;

  return true;
} //IsIdentity

#pragma endregion readersandtests

//////////////////////////////////////////////////////////////////////////////
// print functions

#pragma region printing

/// Print the permutation map on one line as a list of comma-separated numbers.

void CPerm::printmap() const{
  for(uint8_t i=0; i<m_nSize-1; i++)
    printf("%u, ", m_nMap[i]);
  printf("%u\n", m_nMap[m_nSize - 1]);
} //printmap

/// Print the reverse lexicographic number as a hexadecimal string.

void CPerm::printnum() const{
  printf("%s\n", GetNum<uintx_t>().GetString().c_str());
} //printnum

#pragma endregion printing

//////////////////////////////////////////////////////////////////////////////
// operators

#pragma region operators

/// Reader function for the map.
/// \param n Index into map.
/// \return nth element of map.

uint8_t CPerm::operator[](uint8_t n) const{
  return m_nMap[n];
} //operator[]

/// Perform a deep copy of a permutation.
/// \param p A permutation.
/// \return A reference to this permutation after the copy.

CPerm& CPerm::operator=(const CPerm& p){
  assert(p.m_nSize == m_nSize); //safety

  if(this != &p)
    memcpy(m_nMap, p.m_nMap, m_nSize);

  return *this;
} //operator=

/// Test whether a pair of permutations are different. Permutations are
/// different iff they are different sizes or they have different maps. 
/// Uses operator== to do the actual work.
/// \param p0 A permutation.
/// \param p1 A permutation.
/// \return true if p0 and p1 are different.

bool operator!=(const CPerm& p0, const CPerm& p1){
  return !(p0 == p1);
} //operator!=

/// Test whether a pair of permutations are identical. Permutations are
/// identical iff they are the same size and they have identical maps.
/// \param p0 A permutation.
/// \param p1 A permutation.
/// \return true if p0 and p1 are identical.

bool operator==(const CPerm& p0, const CPerm& p1){
  assert(p0.m_nSize == p1.m_nSize); //safety

  bool ok = true; //true if we think they are equal

  for(uint8_t i=0; i<p0.m_nSize && ok; i++) //note early exit
    ok = ok && (p0.m_nMap[i] == p1.m_nMap[i]); //maps must be identical

  return ok;
} //operator==

/// Permutation composition, that is, post-multiplication by a permutation.
/// \param p A permutation.
/// \return A reference to this permutation after composition.

const CPerm& CPerm::operator*=(const CPerm& p){
  const uint8_t* pmap = p.m_nMap; //p's map table

  for(uint8_t i=0; i<m_nSize; i++){ //for each map entry
    uint8_t& m = m_nMap[i]; //current map entry
    m = pmap[m]; //apply second map
  } //for

  return *this;
} //operator*=

/// Use the method of Hall and Knuth, "Combinatorial analysis and computers",
/// The American Mathematical Monthly 72(2):21-28, 1965, to compute the 
/// index of the permutation in reverse lexicographic order using mixed-radix
/// arithmetic. This is the inverse of the constructor CPerm(int, uintx_t).
/// \tparam uint An unsigned integer type.
/// \return The reverse lexicographic number of this permutation.

template<class uint> uint CPerm::GetNum() const{
  uint n = 0; //return result
  uint ifactorial = 1; //0!

  for(uint8_t i=1; i<m_nSize; i++){
    ifactorial *= i; //i!
    int count = 0; //number of earlier entries less than the i'th one

    for(uint8_t j=0; j<i; j++) 
      count += m_nMap[j] < m_nMap[i];
    
    n += count*ifactorial; 
  } //for

  return n;
} //GetNum

/////////////////////////////////////////////////////////////////////////////
//explicit template instantiations

template uintx_t CPerm::GetNum<uintx_t>() const;
template uint64_t CPerm::GetNum<uint64_t>() const;
template uint32_t CPerm::GetNum<uint32_t>() const;
template uint16_t CPerm::GetNum<uint16_t>() const;
template uint8_t  CPerm::GetNum<uint8_t>() const;

#pragma endregion operators