/// \file Main.cpp
/// \brief Main.

#ifdef _MSC_VER //Windows Visual Studio
  #include <conio.h>
#endif

#include "Includes.h"
#include "uintx_t.h"
#include "Cayley32.h"

//function prototypes

void init_genrand64(uint64_t seed); ///< Initialize Mersenne Twister.
uint64_t genrand64_int64(void); ///< Mersenne Twister.
uint64_t CPUTimeInNanoseconds(); ///< CPU time in nanoseconds.

/// \brief Task type

enum class Task{
  Time, Generate, GenerateEx, GenerateMT, None
}; //Task

/// \brief Print help.
///
/// Print canned help message to stdout.

void PrintHelp(){
  printf("Cayley32: A pseudorandom number generator based on the ");
  printf("symmetric group S_23.\n");
  printf("Usage:\ngenerator.exe [-s seed] [-g] [-ge] [-gm] [-h]\n");
  printf("  -s n: Seed n, a hex number (defaults to fixed value)\n");
  printf("  -g: Generate infinite Cayley32 pseudorandom bits\n");
  printf("  -ge: Generate infinite Cayley32e pseudorandom bits\n");
  printf("  -gm: Generate infinite Mersenne Twister pseudorandom bits\n");
  printf("  -h: This help.\n");
  printf("To report run-time: ./generator.exe\n");
  printf("To test with DieHarder: ");
  printf("./generator.exe -s 99999 -g | dieharder -g 200 -a\n");
} //PrintHelp

/// \brief Get generator parameters from argv.
///
/// Parse the command line arguments for settings.
/// \param argc Number of command line arguments.
/// \param argv Command line arguments.
/// \param seed [OUT] Seed.
/// \param t [OUT] Task.

void GetParams(int argc, char *argv[], uintx_t& seed, Task& t){
  seed = 999999; //default seed
  t = Task::Time; //default task

  for(int i=1; i<argc; i++){
    std::string s0 = argv[i];

    if(s0 == "-s")
      seed = uintx_t(argv[i + 1]);
    
    else if(s0 == "-g")
      t = Task::Generate;
    
    else if(s0 == "-ge")
      t = Task::GenerateEx;
    
    else if(s0 == "-gm")
      t = Task::GenerateMT;
    
    else if(s0 == "-h"){
      t = Task::None;
      PrintHelp();
    } //else if
  } //for
} //GetParams

/// \brief Write an infinite number of pseudorandom bits to stdout.
///
/// The bitstream is intended to be piped into Dieharder, which requires an
/// arbitrary length bitstream. Dieharder will break the pipe when it has
/// enough data. The output of the PNRG is accumulated in a buffer before
/// being written to stdout.
/// \param rnd A PRNG.
/// \param nBufSize Buffer size in 8-byte blocks.

template<typename t> void Generate(const t& rnd, uint64_t nBufSize){
  const FILE* unused = freopen(nullptr, "wb", stdout); //stdout in binary mode

  uint64_t* buffer = new uint64_t[nBufSize]; //buffer for pseudo-random numbers

  while(true){ //keep generating bufferfuls of data and throwing it to stdout
    uint64_t* p = buffer; //output pointer, set to start of buffer

    for(uint64_t i=0; i<nBufSize; i++) //fill buffer with pseudo-random UINT64s
      *p++ = rnd(); //generate uint64_t and append to buffer

    const size_t bytecount = nBufSize*sizeof(uint64_t); //buffer size in bytes
    fwrite((uint8_t*)buffer, bytecount, 1, stdout); //output buffer as bytes
  } //while

  delete [] buffer;
} //Generate

/// \brief Time a PRNG.
///
/// Measure the average number of nanoseconds per bit used by a PRNG.
/// \param rand Pseudorandom number generation function.
/// \param n Number of 64-bit words to generate.
/// \return Number of nanoseconds per bit.

template<typename t> double Time(const t& rand, uint64_t n){
  const uint64_t t0 = CPUTimeInNanoseconds(); //start CPU time

  for(uint32_t i=0; i<n; i++)
    rand(); //generate uint64_t and throw it away
  
  return double(CPUTimeInNanoseconds() - t0)/double(n*sizeof(uint64_t));
} //Time

/// \brief Time Cayley and the Mersenne Twister.
///
/// Print to the console the average number of nanoseconds per bit used by
/// Cayley and the Mersenne Twister.
/// \param pCayley Pointer to an instance of the Cayley32 PRNG.
/// \param n Number of 64-bit words to generate.

void Time(Cayley32* pCayley, uint64_t n){ 
  const uint32_t mb = (8*n*sizeof(uint64_t))/1048576LL; //number of Mb
  
  printf("Timing the generation of %u Megabits ", mb);
  printf("by Cayley32 and the Mersenne Twister.\n");

  const double t0 = Time([&](){pCayley->rand();}, n);
  printf("Cayley32: %0.2f nanoseconds per bit\n", t0);

  const double t1 = Time([&](){genrand64_int64();}, n);
  printf("Mersenne Twister: %0.2f nanoseconds per bit\n", t1);
  
  printf("Cayley32 is %0.1f times slower\n", t0/t1);
} //Time

/// \brief Main.
///
/// \param argc Number of arguments.
/// \param argv Arguments.
/// \return 0 (What could *possibly* go wrong?)

int main(int argc, char *argv[]){
  uintx_t seed = 9999999; //default seed 
  Task t = Task::Time; //default task

  GetParams(argc, argv, seed, t); //get parameters from command line args
  
  init_genrand64((uint64_t)seed); //seed Mersenne Twister

  Cayley32e cayley32e; //new PRNG with pseudorandom generators
  cayley32e.srand(genrand64_int64); //seed it
  
  Cayley32 cayley32; //new PRNG with fixed generators
  cayley32.srand(seed); //seed it

  //cayley32.GetGenerator(0).printnum();
  //cayley32.GetGenerator(1).printnum();

  const uint32_t nBufSize = 10485760; //buffer size

  switch(t){ //depending on the task
    case Task::Time:
      Time(&cayley32, 33554432);

      #ifdef _MSC_VER //Windows Visual Studio
        _cputs("\nHit Almost Any Key to Exit...\n");
        _getch();
      #endif
    break;

    case Task::Generate: //fixed generators
      Generate([&](){return cayley32.rand();}, nBufSize);
    break;

    case Task::GenerateEx: //pseudo-random generators
      Generate([&](){return cayley32e.rand();}, nBufSize);
    break;

    case Task::GenerateMT: //Mersenne Twister for baseline
      Generate([&](){return genrand64_int64();}, nBufSize);
    break;
  } //switch

  return 0;
} //main
