//
// Sitina1
// Copyright 2024 Wenting Zhang
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
#include "metadata.h"

#define ISO_OFFSET  67  // 8.15 dB
#define STEP_PER_DB 28  // 1.0024 dB

const ISO_SETPOINT iso_setpoints[] = {
    {100, 4, 0 * STEP_PER_DB + ISO_OFFSET, "100"},
    {160, 6, 0 * STEP_PER_DB + ISO_OFFSET, "140"},
    {200, 7, 0 * STEP_PER_DB + ISO_OFFSET, "200"},
    {400, 7, 6 * STEP_PER_DB + ISO_OFFSET, "400"},
    {800, 7, 12 * STEP_PER_DB + ISO_OFFSET, "800"},
    {1600, 7, 18 * STEP_PER_DB + ISO_OFFSET, "1600"},
    {3200, 7, 24 * STEP_PER_DB + ISO_OFFSET, "3200"},
    {6400, 7, 30 * STEP_PER_DB + ISO_OFFSET, "6400"}
};

const int no_iso_setpoints = sizeof(iso_setpoints) / sizeof(ISO_SETPOINT);

const SHUT_SETPOINT shut_setpoints[] = {
    {200000,  325, "1/5"},
    {166667,  725, "1/6"},
    {125000, 1224, "1/8"},
    {100000, 1524, "1/10"},
    { 76923, 1800, "1/13"},
    { 66667, 1923, "1/15"},
    { 50000, 2123, "1/20"},
    { 40000, 2243, "1/25"},
    { 33333, 2323, "1/30"},
    { 25000, 2423, "1/40"},
    { 20000, 2483, "1/50"},
    { 16667, 2523, "1/60"},
    { 12500, 2573, "1/80"},
    { 10000, 2603, "1/100"},
    {  8000, 2627, "1/125"},
    {  6250, 2648, "1/160"},
    {  5000, 2663, "1/200"},
    {  4000, 2675, "1/250"},
    {  3125, 2685, "1/320"},
    {  2500, 2693, "1/400"},
    {  2000, 2699, "1/500"},
    {  1563, 2704, "1/640"},
    {  1250, 2708, "1/800"},
    {  1000, 2711, "1/1000"},
    {   800, 2713, "1/1250"},
    {   625, 2715, "1/1600"},
    {   500, 2717, "1/2000"},
    {   400, 2718, "1/2500"},
    {   313, 2719, "1/3200"},
    {   250, 2720, "1/4000"},
    {   125, 2721, "1/8000"}
};

const int no_shut_setpoints = sizeof(shut_setpoints) / sizeof(SHUT_SETPOINT);

// Global states
// Stored as index
int current_shutter = 0;
int current_iso = 0;
AE_MODE current_aem = AEM_M;
