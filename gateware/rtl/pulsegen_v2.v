`timescale 1ns / 1ps
`default_nettype none
//
// pulsegen_v2.v: Pulse Generator for CCD V2 control 
//
// Copyright 2024 Anhang Li <thelithcore@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

// Terminology:
//  VDDH(8V)-                     /‾‾‾‾‾‾‾\
//                               /.       .\
//  VDDL(0V)-           .-------' .       . '-------.
//                     /.       . .       . .       .\
//  VLO(-9V)-  _______/ .       . .       . .       . \_______
//                    . .       . .       . .       . .
//  TRIG-  ____/\_____._._______._._______._._______._.________
//             |<---->|  Lead   . .       . .       . .
//                    |-|  Posedge VDDL (PEL)      . .
//                    . |<---->|  Hold VDDL 1 (LH1). .
//                    . .      |-|  Posedge VDDH (PEH)
//                    . .      . |<---->|  Hold VDDH (HH)
//                    . .      . .      |-|  Negedge VDDH (NEH)
//                    . .      . .      . |<---->|  Hold VDDL 2 (LH2)
//                    . .      . .      . .      |-|  Negedge VLO (NEL)
//                    . .      . .      . .      . |<---- Low (LO)
// PDN-   ‾‾‾‾‾‾‾‾‾‾‾‾\_.______._.______._.______._/‾‾‾‾‾‾‾‾‾‾‾‾
// PLO-   ____________._/‾‾‾‾‾‾\_.______._/‾‾‾‾‾‾\_.____________
// SELL-  ____________/‾\______._.______._.______/‾\____________
// SELH-  _____________________/‾\______/‾\_____________________
// PHI-   _______________________/‾‾‾‾‾‾\_______________________