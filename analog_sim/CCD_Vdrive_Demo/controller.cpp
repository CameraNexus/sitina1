// Automatically generated C++ file on Mon Jun  3 14:42:22 2024
//
// To build with Digital Mars C++ Compiler:
//
//    dmc -mn -WD controller.cpp Vcontroller*.cpp verilated.lib kernel32.lib
//
// The intended design of the Verilog interface is such that this
// file should not need editing unless you change the symbol.

#include <verilated.h>
#include "Vcontroller.h"

extern "C" __declspec(dllexport) void Destroy(Vcontroller *instance)
{
   if(instance)
      delete(instance);
}

extern "C" __declspec(dllexport) void controller(Vcontroller **instance, double t, union uData *data)
{
   if(!*instance)
      *instance = new Vcontroller;
   (*instance)->clk = data[0].b;
   (*instance)->reset = data[1].b;
   (*instance)->trig = data[2].b;

   (*instance)->eval();

   data[3].uc = (*instance)->state;
   data[4].b = (*instance)->pd;
   data[5].b = (*instance)->pul;
   data[6].b = (*instance)->puh;
   data[7].b = (*instance)->sel;
   data[8].b = (*instance)->seh;
   data[9].b = (*instance)->lss;
}
