// Automatically generated C++ file on Tue Jun  4 03:17:40 2024
//
// To build with Digital Mars C++ Compiler:
//
//    dmc -mn -WD pulsegen.cpp Vpulsegen*.cpp verilated.lib kernel32.lib
//
// The intended design of the Verilog interface is such that this
// file should not need editing unless you change the symbol.

#include <verilated.h>
#include "Vpulsegen.h"

extern "C" __declspec(dllexport) void Destroy(Vpulsegen *instance)
{
   if(instance)
      delete(instance);
}

extern "C" __declspec(dllexport) void pulsegen(Vpulsegen **instance, double t, union uData *data)
{
   if(!*instance)
      *instance = new Vpulsegen;
   (*instance)->clk = data[0].b;
   (*instance)->reset = data[1].b;
   (*instance)->t1 = data[2].ui;
   (*instance)->t2 = data[3].i;
   (*instance)->trig = data[4].b;

   (*instance)->eval();

   data[5].b = (*instance)->q;
   data[6].b = (*instance)->qbar;
}
