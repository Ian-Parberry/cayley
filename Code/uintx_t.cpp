/// \file uintx_t.cpp
/// \brief Implementation of the extensible unsigned integer class.

#include "uintx_t.h"

#include "Includes.h"

const int HalfBytesInWord = 2*sizeof(uint32_t); ///< Number of nibbles in a word.
const int BitsInHalfByte = 4; ///< Number of bits in a nibble.
const int BitsInWord = 8*sizeof(uint32_t); ///< Number of bits in a word

/////////////////////////////////////////////////////////////////////////////
//Constructors and destructors.

#pragma region structors

/// Void constructor.

uintx_t::uintx_t():m_nSize(0), m_pData(nullptr){ 
} //void constructor

/// Integer constructor.
/// \param i Initial value.

uintx_t::uintx_t(int i): m_nSize(1){ 
  m_pData = new uint32_t[1]; 
  m_pData[0] = i;
} //integer constructor

/// String constructor.
/// \param string String containing initial value in hexadecimal.

uintx_t::uintx_t(const char *string){ 
  const int n = strlen(string);
  m_nSize = (int)std::ceil((double)n/HalfBytesInWord);
  m_pData = new uint32_t[m_nSize]; //grab space

  for(int i=0; i<m_nSize; i++)
    m_pData[i] = 0; //clear m_pData

  loadstring(string); //load string
} //string constructor

/// Copy constructor.
/// \param x Extensible unsigned integer to be copied.
/// \return Reference to this extensible unsigned integer after copying.

uintx_t::uintx_t(const uintx_t& x){ 
  m_nSize = x.m_nSize;
  m_pData = new uint32_t[m_nSize]; //grab space

  for(int i=0; i<m_nSize; i++)
    m_pData[i] = x.m_pData[i]; //load m_pData
} //copy constructor

/// Destructor.

uintx_t::~uintx_t(){ 
  delete [] m_pData; 
} //destructor

#pragma endregion structors

/////////////////////////////////////////////////////////////////////////////
// General purpose functions.

#pragma region general

/// Convert to a hexadecimal string.
/// \return Hexidecimal string.

std::string uintx_t::GetString() const{
  std::string s; //result

  //convert to a backwards hex string, ie. least-significant digit first

  for(int i=0; i<m_nSize; i++){ //for each word, least significant first
    uint32_t n = m_pData[i]; //current word
    const uint32_t size = 2*sizeof(uint32_t); //number of digits in a word

    for(auto i=0; i<size; i++){ //for each digit, least-significant first
      uint32_t digit = n & 0xF; //grab a digit
      char c = char(digit + (digit < 10? '0': 'A' - 10));
      s += c; //append to string
      n >>= 4; //next digit
    } //while
  } //for

  //remove leading zeros, if any

  for(auto it=std::prev(s.end()); it!=s.begin() && *it=='0';){
    --it; //prepare for next iteration
    s.pop_back(); //remove leading zero
  } //while

  if(s == "")s = "0"; //safety

  //reverse the string to make it most significant digit first, and return

  std::reverse(s.begin(), s.end());
  return s;
} //GetString

/// Change the number of words allocated and zero out the value stored.
/// \param s Number of words to allocate.

void uintx_t::reallocate(const int s){ 
  if(m_nSize != s){ //if change needed
    m_nSize = s; 
    delete [] m_pData; 
    m_pData = new uint32_t[s]; //get space

    for(int i=0; i<m_nSize; i++)
      m_pData[i] = 0; //zero it out
  } //if
} //reallocate

/// Increase the amount of space allocated while keeping the value stored.
/// \param s Number of words to increase to.

void uintx_t::grow(const int s){
  uint32_t* olddata = m_pData; //old m_pData
  int oldsize = m_nSize; //old m_nSize

  if(m_nSize <= s){ //if really an increase in m_nSize
    m_pData = new uint32_t[s]; //grab new space
    m_nSize = s; 
    
    int i; 

    for(i=0; i<oldsize; i++)
      m_pData[i] = olddata[i]; //copy over old digits

    for(; i<m_nSize; i++)
      m_pData[i] = 0; //zero out the rest

    delete [] olddata; //recycle old space
  } //if
} //grow

/// \return Least significant 32-bit unsigned integer.

uintx_t::operator uint32_t(){
  return m_pData[0];
} //uint32_t

/// Construct a 64-bit unsigned integer from the two least-significant words.
/// \return Least significant 64-bit unsigned integer.

uintx_t::operator uint64_t(){
  return (uint64_t(m_pData[1]) << 32) | m_pData[0];
} //uint64_t

/// Minimize the amount of storage by removing the leading zero words
/// and reallocating a smaller amount of space if possible.

void uintx_t::normalize(){
  uint32_t* olddata = m_pData; //old m_pData
  int top;

  for(top=m_nSize - 1; top>0 && m_pData[top]==0; top--);

  if(top < m_nSize - 1){ //if really a change in m_nSize
    m_nSize = top + 1; 
    m_pData = new uint32_t[m_nSize]; //grab new space

    for(int i=0; i<=top; i++)
      m_pData[i] = olddata[i]; //copy over old digits

    delete [] olddata; //recycle old space
  } //if
} //normalize

/// Set the value stored to a hex value.
/// \param string A null-terminated string containing a hex value.

void uintx_t::loadstring(const char* string){ 
  int i;
  int word = m_nSize - 1; //current word in long integer
  int digitcount = strlen(string); //number of digits in string

  int shift = (digitcount%HalfBytesInWord)*BitsInHalfByte; //shift within word
  if(shift <= 0)shift = BitsInWord; //wrap shift amount

  for(i=0; i<m_nSize; i++)
    m_pData[i]=0; //clear m_pData

  for(i=0; i<digitcount; i++){ //load each digit from string
    int digit = 0;

    //convert string[i] from char to digit

    if(string[i] >= '0' && string[i] <='9')
      digit = string[i] - '0';

    else if(string[i] >= 'A' && string[i] <= 'F')
      digit = 10+string[i] - 'A';

    else if(string[i] >= 'a' && string[i] <= 'f')
      digit = 10 + string[i]-'a';

    else assert(false); //safety

    //compute shift amount

    if(shift <= 0){
      shift = BitsInWord;
      word--;
    } //if

    shift = shift - BitsInHalfByte;

    //load digit into word of m_pData

    m_pData[word] = m_pData[word] | (digit<<shift);
  } //for
} //loadstring

/// Compute the number of significant bits in the value stored.
/// \return the number of bits stored.

int uintx_t::bitcount(){
  if(m_nSize <= 0)return 0;

  uint32_t word = m_pData[m_nSize - 1]; //most significant word in x
  int count = 0; //counter

  while(word > 0){
    word >>= 1;
    count++;
  } //while

  return count + (m_nSize - 1)*BitsInWord;
} //bitcount

#pragma endregion general

/////////////////////////////////////////////////////////////////////////////
//Assignment operators.

#pragma region assignment

/// Assign a extensible unsigned integer.
/// \param x Extensible unsigned integer to be copied.
/// \return Reference to this extensible unsigned integer after copying.

uintx_t& uintx_t::operator=(const uintx_t& x){ 
  if(this != &x){ //protect against self assignment
    reallocate(x.m_nSize); //grab enough space
    for(int i=0; i<m_nSize; i++) //copy over data
      m_pData[i] = x.m_pData[i];
  } //if

  return *this;
} //operator=

/// Assign an integer.
/// \param i Integer to be copied.
/// \return Reference to this extensible unsigned integer after copying.

uintx_t& uintx_t::operator=(const int i){ 
  reallocate(1); 
  *m_pData = i;
  return *this;
} //operator=

/// Assign a hex value from a null-terminated string.
/// \param s A null-terminated string containing a hex value.
/// \return Reference to this extensible unsigned integer after copying.

uintx_t& uintx_t::operator=(const char* s){ 
  const int n = (int)ceil((double)strlen(s)/HalfBytesInWord);
  reallocate(n);
  loadstring(s);
  return *this;
} //operator=

#pragma endregion assignment

/////////////////////////////////////////////////////////////////////////////
// Addition operators.

#pragma region addition

/// Add a pair of extensible unsigned integers.
/// \param x A extensible unsigned integer.
/// \param y A extensible unsigned integer.
/// \return x + y.

uintx_t operator+(uintx_t x, uintx_t y){
  return x += y;
} //operator+

/// Add an integer.
/// \param y An integer.
/// \return Reference to this extensible unsigned integer after y has been added.

uintx_t& uintx_t::operator+=(const int& y){
  (*this) += uintx_t(y);
  return *this;
} //operator+=

/// Add a extensible unsigned integer.
/// \param y A extensible unsigned integer.
/// \return Reference to this extensible unsigned integer after y has been added.

uintx_t& uintx_t::operator+=(const uintx_t& y){ 
  uint32_t left, right, left_msb, right_msb; //operands and their msb's
  uint32_t sum, sum_msb; //sum and its msb
  uint32_t carry = 0; //single-bit carry

  const uint32_t mask_msb = 1 << (BitsInWord-1); //mask for most significant bit
  const uint32_t mask_lsb = ~mask_msb; //mask for all but most significant bit

  int i; //looping variable
  int oldsize = m_nSize;

  grow(m_nSize > y.m_nSize? m_nSize: y.m_nSize); //make enough space for result

  for(i=0; i<m_nSize; i++){ //for each word in the result
    //grab a word from each operand
    left = i < oldsize? m_pData[i]: 0;
    right = i < y.m_nSize? y.m_pData[i]: 0;

    //extract the most significant bit (msb) from each
    left_msb = (left & mask_msb) >> (BitsInWord - 1);
    right_msb = (right & mask_msb) >> (BitsInWord - 1);

    //zero out the msb from each
    left = left & mask_lsb;
    right = right & mask_lsb;

    //add them
    sum = left + right + carry;

    //compute carry
    sum_msb = (sum & mask_msb) >> (BitsInWord - 1);
    sum = sum & mask_lsb;
    carry = left_msb + right_msb + sum_msb; //carry is either 0, 1, 2, or 3 here

    //put leading bit of carry back into sum
    if((carry == 1) || (carry == 3))
      sum = sum | mask_msb;

    m_pData[i] = sum;

    //pass along leading bit of carry
    carry >>= 1;
  } //for

  if(carry >= 1){ //carry of 1 fell out, need more space for result
    grow(m_nSize + 1); //need one more place for carry
    m_pData[m_nSize - 1] = 1; //set most significant digit
  } //if

  return *this;
} //operator+=

#pragma endregion addition

/////////////////////////////////////////////////////////////////////////////
// Comparison operators.

#pragma region comparison

/// Greater than test for two extensible unsigned integers.
/// \param x A extensible unsigned integer.
/// \param y A extensible unsigned integer.
/// \return true If x is greater than y.

bool operator>(uintx_t x, uintx_t y){ 
  if(x.m_nSize > y.m_nSize)return true; //x>y
  if(x.m_nSize < y.m_nSize)return false; //x<y

  //check m_pData
  for(int i=x.m_nSize-1; i>=0; i--)
    if(x.m_pData[i] > y.m_pData[i])return true; //x>y
    else if(x.m_pData[i] < y.m_pData[i])return false; //x<y

  return false; //they're equal
} //operator>

/// Greater than test for a extensible unsigned integer and an integer.
/// \param x A extensible unsigned integer.
/// \param y An integer.
/// \return true If x is greater than y.

bool operator>(uintx_t x, int y){ 
  return x > uintx_t(y); //lazy way to do it
} //operator>

/// Greater than or equal to test for two extensible unsigned integers.
/// \param x A extensible unsigned integer.
/// \param y A extensible unsigned integer.
/// \return true If x is greater than or equal to y.

bool operator>=(uintx_t x, uintx_t y){ 
  if(x.m_nSize > y.m_nSize)return true; //x>y
  if(x.m_nSize < y.m_nSize)return false; //x<y

  //check m_pData
  for(int i=x.m_nSize-1; i>=0; i--)
    if(x.m_pData[i] >y .m_pData[i])return true; //x>y
    else if(x.m_pData[i] < y.m_pData[i])return false; //x<y

  return true; //they're equal
} //operator>=

/// Greater than or equal to test for a extensible unsigned integer and
/// an integer.
/// \param x A extensible unsigned integer.
/// \param y An integer.
/// \return true If x is greater than or equal to y.

bool operator>=(uintx_t x, int y){ 
  return x >= uintx_t(y); //lazy way to do it
} //operator>=

/// Less than test for two extensible unsigned integers.
/// \param x A extensible unsigned integer.
/// \param y A extensible unsigned integer.
/// \return true If x is less than y.

bool operator<(uintx_t x, uintx_t y){ 
  return y > x; //lazy again
} //operator<

/// Less than test for a extensible unsigned integer and an integer.
/// \param x A extensible unsigned integer.
/// \param y An integer.
/// \return true If x is less than y.

bool operator<(uintx_t x, int y){ 
  return x < uintx_t(y); //lazy way to do it
} //operator<

/// Less than or equal to test for two extensible unsigned integers.
/// \param x A extensible unsigned integer.
/// \param y A extensible unsigned integer.
/// \return true If x is less than or equal to y.

bool operator<=(uintx_t x, uintx_t y){ 
  return y >= x; //lazy again
} //operator<=

/// Less than or equal to test for a extensible unsigned integer and an integer.
/// \param x A extensible unsigned integer.
/// \param y An integer.
/// \return true If x is less than or equal to y.

bool operator<=(uintx_t x, int y){ 
  return x <= uintx_t(y); //lazy way to do it
} //operator<=

/// Equality test for two extensible unsigned integers.
/// \param x A extensible unsigned integer.
/// \param y A extensible unsigned integer.
/// \return true If x is equal to y.

bool operator==(uintx_t x, uintx_t y){ 
  if(x.m_nSize != y.m_nSize)
    return false;

  //check m_pData
  for(int i=x.m_nSize - 1; i>=0; i--)
    if(x.m_pData[i] != y.m_pData[i])
      return false;

  return true; //they're equal
} //operator==

/// Equality test for a extensible unsigned integer and an unsigned integer.
/// \param x A extensible unsigned integer.
/// \param y An unsigned integer.
/// \return true If x is equal to y.

bool operator==(uintx_t x, uint32_t y){ 
  return (x.m_nSize == 1) && (x.m_pData[0] == y);
} //operator==

/// Equality test for an unsigned integer and a extensible unsigned integer.
/// \param x An unsigned integer.
/// \param y A extensible unsigned integer.
/// \return true If x is equal to y.

bool operator==(uint32_t x, uintx_t y){ 
  return y == x;
} //operator==

/// Unequality test for two extensible unsigned integers.
/// \param x A extensible unsigned integer.
/// \param y A extensible unsigned integer.
/// \return true If x is not equal to y.

bool operator!=(uintx_t x, uintx_t y){ 
  return !(x == y); 
} //operator!=

/// Unequality test for a extensible unsigned integer and an unsigned integer.
/// \param x A extensible unsigned integer.
/// \param y An unsigned integer.
/// \return true If x is not equal to y.

bool operator!=(uintx_t x, uint32_t y){ 
  return !(x == y); 
} //operator!=

/// Unequality test for an unsigned integer and a extensible unsigned integer.
/// \param x An unsigned integer.
/// \param y A extensible unsigned integer.
/// \return true If x is not equal to y.

bool operator!=(uint32_t x, uintx_t y){ 
  return !(x == y); 
} //operator!=

#pragma endregion comparison

/////////////////////////////////////////////////////////////////////////////
// Bit shift operators.

#pragma region shift

/// Left-shift this.
/// \param distance Number of bits to left-shift by.
/// \return Reference to this extensible unsigned integer after left-shifting.

uintx_t& uintx_t::operator<<=(const int distance){ 
  //grow to required m_nSize
  if(*this > 0){
    int oldsize = m_nSize; //save old m_nSize for later

    //compute new number of bits - divide by BitsPerWord and round up
    grow((this->bitcount() + distance + BitsInWord - 1)/BitsInWord);

    //shift by word
    int dest = m_nSize - 1; //copy destination
    int src = dest - distance/BitsInWord; //copy source

    while(src >= 0){ //until end of source
      if(src < oldsize)
        m_pData[dest] = m_pData[src]; //copy
      dest--; src--; //move along
    } //while

    while(dest >= 0)
      m_pData[dest--] = 0; //fill bottom with zeros

    //shift within words
    const int d = distance%BitsInWord; //shift distance within words

    if(d > 0)
      for(dest=m_nSize - 1; dest>=0; dest--){
        m_pData[dest] <<= d;
        if(dest > 0)
          m_pData[dest] = m_pData[dest] | (m_pData[dest - 1] >> (BitsInWord - d));
      } //for
  } //if

  return *this;
} //operator<<=

/// Left-shift a extensible unsigned integer.
/// \param x A extensible unsigned integer.
/// \param d Number of bits to left-shift by.
/// \return x left-shifted by d bits.

uintx_t operator<<(uintx_t x, int d){ 
  return x <<= d; 
} //operator<<

/// Right-shift this.
/// \param distance Number of bits to right-shift by.
/// \return Reference to this extensible unsigned integer after right-shifting.

uintx_t& uintx_t::operator>>=(const int distance){ 
  //find new size
  int newsize = (this->bitcount() - distance + BitsInWord - 1)/BitsInWord;

  if(newsize<1)
    *this = 0;

  else{
    //shift by word
    int dest = 0; //copy destination
    int src = dest + distance/BitsInWord; //copy source
    if(dest != src)
      while(src < m_nSize){ //until end of source
        m_pData[dest] = m_pData[src]; //copy
        m_pData[src] = 0; //zero out copied word
        dest++; src++; //move along
      } //whilke

    //shift within words
    const int d = distance%BitsInWord; //shift distance within words
    if(d > 0)
      for(dest=0; dest<newsize; dest++){
        m_pData[dest] >>= d;
        if(dest < m_nSize - 1)
          m_pData[dest] = m_pData[dest] | (m_pData[dest + 1] << (BitsInWord - d));
      } //for
  } //else

  for(int i=newsize; i<m_nSize; i++)
    m_pData[i] = 0; //zero out unused portion

  this->normalize(); //remove leading zero words

  return *this;
} //operator>>=

/// Right-shift a extensible unsigned integer.
/// \param x A extensible unsigned integer.
/// \param d Number of bits to right-shift by.
/// \return x right-shifted by d bits.

uintx_t operator>>(uintx_t x, int d){ 
  return x >>= d; 
} //operator>>

#pragma endregion shift

/////////////////////////////////////////////////////////////////////////////
// Bitwise operators.

#pragma region bitwise

/// Logical AND of two extensible unsigned integers.
/// \param x A extensible unsigned integer.
/// \param y A extensible unsigned integer
/// \return x ANDed with y.

uintx_t operator&(uintx_t x, const uintx_t& y){
  const int n = std::min(x.m_nSize, y.m_nSize);

  for(int i=0; i<n; i++)
    x.m_pData[i] &= y.m_pData[i];

  return x;
} //operator|

/// Logical AND of a extensible unsigned integer and an integer.
/// \param x A extensible unsigned integer.
/// \param y A word.
/// \return The most significant word of x ANDed with y.

int operator&(uintx_t x, int y){
  return x.m_pData[0] & y;
} //operator&

/// Logical OR of two extensible unsigned integers.
/// \param x A extensible unsigned integer.
/// \param y A extensible unsigned integer
/// \return x ORed with y.

uintx_t operator|(uintx_t x, const uintx_t& y){
  const int n = std::min(x.m_nSize, y.m_nSize);

  for(int i=0; i<n; i++)
    x.m_pData[i] |= y.m_pData[i];

  return x;
} //operator|

/// Logical OR of a extensible unsigned integer and an integer.
/// \param x A extensible unsigned integer.
/// \param y A word.
/// \return The most significant word of x ORed with y.

int operator|(uintx_t x, int y){
  return x.m_pData[0] | y;
} //operator|

#pragma endregion bitwise

/////////////////////////////////////////////////////////////////////////////
// Multiplication operators.

#pragma region multiplication

/// Multiply two extensible unsigned integers.
/// \param y A extensible unsigned integer.
/// \param z A extensible unsigned integer
/// \return y multiplied by z.

uintx_t operator*(uintx_t y, uintx_t z){ 
  uintx_t result(0); //place for returned result

  while(z > 0){
    result += y * z.m_pData[0];
    y <<= BitsInWord;
    z >>= BitsInWord;
  } //while

  return result;
} //operator*

/// Multiply a extensible unsigned integer by an integer.
/// \param x A extensible unsigned integer.
/// \param y An integer.
/// \return x multiplied by y.

uintx_t operator*(uintx_t x, int y){ 
  uintx_t word, carry(0);

  word.grow(x.m_nSize);
  carry.grow(x.m_nSize + 1);

  for(int i=0; i<x.m_nSize; i++){
    uint64_t prod = (uint64_t)x.m_pData[i]*(uint64_t)y;
    word.m_pData[i] = unsigned(prod & 0xFFFFFFFF);
    carry.m_pData[i + 1] = unsigned(prod >> 32);
  } //for

  word.normalize();
  carry.normalize();

  return word + carry;
} //operator*

/// Multiply an integer by a extensible unsigned integer.
/// \param x An integer.
/// \param y A extensible unsigned integer.
/// \return x multiplied by y.

uintx_t operator*(int x, uintx_t y){ 
  return y*x;
} //operator*

/// Multiply a extensible unsigned integer by an unsigned integer.
/// \param x A extensible unsigned integer.
/// \param y An unsigned integer.
/// \return x multiplied by y.

uintx_t operator*(uintx_t x, uint32_t y){ 
  uintx_t word, carry(0);

  word.grow(x.m_nSize);
  carry.grow(x.m_nSize + 1);

  for(int i=0; i<x.m_nSize; i++){
    uint64_t prod = (uint64_t)x.m_pData[i]*(uint64_t)y;
    word.m_pData[i] = unsigned(prod & 0xFFFFFFFF);
    carry.m_pData[i + 1] = unsigned(prod >> 32);
  } //for

  word.normalize();
  carry.normalize();

  return word + carry;
} //operator*

/// Multiply an unsigned integer by a extensible unsigned integer.
/// \param x An unsigned integer.
/// \param y A extensible unsigned integer.
/// \return x multiplied by y.

uintx_t operator*(uint32_t x, uintx_t y){ 
  return y*x;
} //operator*

/// Multiply by a extensible unsigned integer.
/// \param y A extensible unsigned integer.
/// \return Reference to this extensible unsigned integer after multiplication by y.

uintx_t& uintx_t::operator*=(const uintx_t& y){ 
  return *this = *this * y;
} //operator*=

#pragma endregion multiplication

/////////////////////////////////////////////////////////////////////////////
// Subtraction operators.

#pragma region subtraction

/// Subtract a extensible unsigned integer from a extensible unsigned integer.
/// \param x A extensible unsigned integer.
/// \param y A extensible unsigned integer
/// \return y subtracted from x, if non-negative.

uintx_t operator-(uintx_t x, uintx_t y){ 
  return x -= y;
} //operator-

/// Subtract a extensible unsigned integer.
/// \param y A extensible unsigned integer.
/// \return Reference to this extensible unsigned integer after y is subtracted.

uintx_t& uintx_t::operator-=(const uintx_t& y){ 
  if(y >= *this)
    *this = 0; //subtracting something too big

  else if(y > 0){
    uint32_t left, right; //operands
    bool borrow = false; //single-bit borrow
    int i; //looping variable

    for(i=0; i<m_nSize; i++){ //for each word in the result
      //grab a word from each operand
      left=m_pData[i];
      right = (i < y.m_nSize)? y.m_pData[i]: 0;

      //subtract them
      if(borrow)
        borrow = ++right == 0; //try to add borrow to right

      m_pData[i] = left - right; //subtraction of uint32_t borrows automatically

      if(left < right)
        borrow = true;
    } //for
  } //else

  normalize();

  return *this;
} //operator-=

#pragma endregion subtraction

/////////////////////////////////////////////////////////////////////////////
// Division and remainder operators.

#pragma region division

/// Divide a extensible unsigned integer by a extensible unsigned integer.
/// \param y A extensible unsigned integer.
/// \param z An unsigned integer.
/// \return x divided by y, rounded down.

uintx_t operator/(uintx_t y, uintx_t z){
  uintx_t q(0); //result

  if(y >= z){
    uintx_t r(y), w(z);

    w <<= y.bitcount() - z.bitcount();

    while(w <= y)
      w <<= 1;

    while(w > z){
      q <<= 1; 
      w >>= 1;

      if(w <= r){
        r -= w;
        q += 1;
      } //if
    } //while
  } //if

  return q;
} //operator/

/// Divide a extensible unsigned integer by an unsigned integer.
/// \param y A extensible unsigned integer.
/// \param z An unsigned integer.
/// \return y divided by z, rounded down.

uintx_t operator/(uintx_t y, uint32_t z){ 
  return y/uintx_t(z);
} //operator/

/// Divide by a extensible unsigned integer.
/// \param y A extensible unsigned integer.
/// \return Reference to this extensible unsigned integer after division.

uintx_t& uintx_t::operator/=(const uintx_t& y){ 
  return *this = *this/y;
} //operator/

/// Remainder after dividing a extensible unsigned integer by a extensible
/// unsigned integer.
/// \param y A extensible unsigned integer.
/// \param z A extensible unsigned integer.
/// \return The remainder after y is divided by z.

uintx_t operator%(uintx_t y, uintx_t z){ 
  uintx_t w(z);

  while(w <= y)
    w <<= 1;

  while(w > z){ 
    w >>= 1;

    if(w <= y)
      y -= w;
  } //while

  return y;
} //operator%

/// Remainder after dividing by a extensible unsigned integer.
/// \param y A extensible unsigned integer.
/// \return Reference to this extensible unsigned integer after remaindering.

uintx_t& uintx_t::operator%=(const uintx_t& y){ 
  return *this = *this%y;
} //operator%=

/// Remainder after dividing a extensible unsigned integer by an unsigned integer.
/// \param y A extensible unsigned integer.
/// \param z An unsigned integer.
/// \return The remainder after y is divided by z.

uintx_t operator%(uintx_t y, uint32_t z){ 
  return y%uintx_t(z);
} //operator%

#pragma endregion division
