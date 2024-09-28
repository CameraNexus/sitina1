# Old Versions

Page for documenting old versions of this project. What happened, what worked, what didn't, etc.

### Rev 0.1 - Dec 2017

Initial revision. I had the idea of building a camera late 2016. The initial name of the project was called "F3 Digital" and it was based around the KAI-16000 sensor. As you might have guessed from the name F3 Digital, it's actually about turning the famous Nikon F3 film SLR camera into a DSLR. I was not prepared for the project tbh. There are too many challenges. Back in 2016 I actually had no ideas about how these things worked, I know basically nothing about circuit design, power electronics, programming, mechanical engineering or anything. Plus I didn't have a job back then I had no income to actually support me continue the project. So it basically went nowhere. I was quite frustrated back then to a point I put all my electronics DIY projects to a halt until I feel like I have learned enough things to restart.

![r0p1](/assets/r0p1.png)

The half assembled prototype:

![r0p1_prototype](/assets/r0p1_prototype.jpg)

One major bug killed it is I made the footprint of the ADC mirrored.

Main hardware specs for this failed attempt:

- MCU: iMXRT1052
- FPGA: Spartan6 LX9
- CCD: KAI-16000
- ADC: AD9824

### Rev 0.2 - Mar 2021

Fast forward to 2021, one day when I was browsing eBay I saw someone selling bunch of KAI-11000 sensors at a very low price. I thought it's time to restart the project, even though I wasn't sure if I am ready or not. I was clearly not. I encountered three main challenges during the design process

- Integrating with the Nikon F3
- Driving the high capacitance clock lines
- Using the ADI AFE chip

Let's talk through them.

The first challenge is on the circuit design. CCD sensor signal lines are huge capacitive loads to the driver, yet they demand fast rise and fall times. The voltage also differs from different signals. For example, some are 5V swing around 3V, some are 9V swing around -4V. This would also require designing voltage shifters that not only change the amplitude, but also change the ground reference point. Then finally these circuits need power supplies to provide 10s of different voltage rails, sitting right next to sensitive analog circuits.

The second challenge is using the ADI AFE chip. The AFE chip integrates in a timing generator for the CCD, at first I thought it would be a good idea to just use that to generate all the timings for the CCD. This unfortunately turned out to be much harder than I anticipated. The timing generator is extremely flexible, supporting all sort of different things you may want to do with the CCD. Often there are multiple ways to program it to achieve the same output. The bad part? It has got hundreds of registers to control the behavior of the generator, and one has to be really familiar with its architecture and the intended output to be able to program it correctly. I didn't find any example code, I also had no prior experience with CCD so I wasn't even sure what kind of signal output I should be looking for. So this part ended up becoming challenging as well.

The third challenge is about integrating everything nicely with the F3 camera itself. However, attacking multiple challenges myself at the same time is just not a good strategy. I decided to focus on 1 and 2 first, trying to getting a working image capturing device before turning it into a camera.

This revision introduces the following major changes:

1. Use AD9990 for ADC/ AFE/ TG
2. Remove FPGA
3. Use KAI-11000 instead of KAI-16000
4. Redesigned H/V driver
5. Use 2.8" 640x480 LCD

This reversion continued the diversion between a sensor board and a processor board:

![r0p2_cb](/assets/r0p2_cb.jpg)

![r0p2_pb](/assets/r0p2_pb.jpg)

### Rev 0.3 - Dec 2021

This revision included minor bug fixes, but only to reveal bigger issues.

1. Fix AD9990 TEST pin connection
2. Minor bug fixes for driver
3. Fix RT1052 integrated DCDC connection

### Rev 0.4 - Mar 2022

With this revision I decided to make it more like a dev board rather than a camera board. This enables easy access to all the signals for hooking up an oscillioscope or logic analyzer.

![r0p4](/assets/r0p4.jpeg)

![r0p4_ccd](/assets/r0p4_ccd.jpg)

Notably this is the first time ever I got the CCD working: https://x.com/zephray_wenting/status/1519542527040401408.

### Rev 0.5 - Apr 2022

At this point I more or less have a somewhat working image capturing device, it's time to think about the F3 camera integration issue itself. This is not a single issue, but multiple issues:

1. because there is a thick protective glass in the front of the sensor, when the sensor is installed at the place where normally film goes to, the flange focal distance is increased. This changes the range of the distance which the lens can focus on. While this may not be a big deal, this problem doesn't have a clean solution.
2. the Nikon F3 exposes limited information to the external world. While it's internal electronics knows the ISO settings, the shutter speed dial, there is no easy way to access these. Disassembling the camera to add wires is possible but quite hard.  Otherwise I am stuck with the signal available to the official motor drive (which only has shutter release and shutter close).
3. How do I even attach this thing to the camera? The mechanism to hold the original film compartment door might simply not be strong enough the hold all the stuff I am adding. There is no place to put any reasonably sized battery (even though it might sounds like you can put a big battery in place of the film roll, don't forget there will be a film rewind fork sticking into the middle of the compartment. You don't want that to punch into the battery pack.) Making it like a grip and screw into the camera like the official motor drive was the only viable option I could come up with.

Eventually I think all of these are resolvable, it just takes time. So I started designing a basic chassis for it. My initial idea is to make it look like the Instax SQ1. Though the final design deviated away from it. But the idea of using a square LCD screen lived on.

![r0p5_concept](/assets/r0p5_concept_1.jpg)

![r0p5_concept](/assets/r0p5_concept_2.jpg)

To help simplify the overall construction, the sensor board + processor 2-board design has been merged into a single-board design.

![r0p5_prototype](/assets/r0p5_prototype.jpg)

Main changes for this revision:

1. Switch from iMXRT1052 to iMX6ULL
2. Use 4" 720x720 LCD instead
3. Add 100M ethernet
4. Redesigned CCD power supply
5. Bug fixes for driver level-shifter

### Rev 0.6 - Mar 2023

Turns out I get more impatient as time goes by. At this point I have been quite frustrated and don't want to deal with this project anymore. I don't even want to deal with Linux. So I again switched to use a microcontroller. And made a video about whatever I was able to get out of it, hoping this would be the end of this project. I mean it's a crappy design and crappy camera, but it does take pictures.

Photos I took during debugging:

![r0p6_testing](/assets/r0p6_testing.jpeg)

Main changes:

1. Switch from iMX6ULL to iMXRT1176
2. Shrink board size
3. Removed builtin ethernet
4. CCD power supply bug fixes
5. Use MIPI DSI LCD instead of DPI

### Rev 0.7 - Aug 2023

Turned out the video went quite popular, getting 300K views on Bilibili and YouTube combined. To a point where don't want to claim that that questionable camera is the best I can do. So I continued.

I have several complains about the previous design:

- Bad image quality
- Problematic power supply design (noise issues, unable to supply current needed by the inefficient clock line driver)
- Problematic clock line driver (no slew control, ramp too slow, inefficient)
- Basically impossible to calibrate/ adjust the distance/ flatness between lens and sensor
- No builtin IR cut filter
- Too big
- Grip feels bad
- Full touchscreen operation is a bad idea and I knew it
- Only support fully manual lenses
- LCD is a bit hard to see outside

To address these issues I started designing the new version. I start with the basic idea of fastening both the sensor and the lens onto one metal part (instead of on 2 different plastic parts):

![r0p7_basic_idea](/assets/r0p7_basic_idea.jpg)

To help with miniaturize the camera, the screen size has shrunk from 4.0" down to 3.4". The new screen also has a 1000 nits maximum brightness, which should help with outdoor visibility a bit. This would also mean it's no longer possible to fit all components on a single PCB due to 2 reasons:

- I don't want to assert any external force on the sensor board, so all the IO ports and buttons should reside on another board
- It just doesn't fit

A common solution is to just stack more boards on the back side of the camera. Many industrial and consumer camera do this. I mean this is a pretty reasonable design. For me though, I want to try something different. To keep the camera thin, I want to stack more board in the front side, so I don't add additional thickness other than the bulky CCD sensor, the long pin + connector, and the thick high brightness backlight module. The only downside is that it would need to have a big cutout in the middle:

![r0p7_cut](/assets/r0p7_cut.jpg)

The rest of the design basically follows on. I again decided to focus on the image capturing part itself and worry about the grip later. I did model up the grip so I think it would fit. The final design looks like this (note how the grip is just a mock-up):

![r0p7_mockup_1](/assets/r0p7_mockup_1.jpg)

![r0p7_mockup_2](/assets/r0p7_mockup_2.jpg)

I went ahead and ordered the board and got it soldered and did some initial testing, and found some quite stupid issues preventing it from working.

![r0p7_boards](/assets/r0p7_boards.jpg)

Main changes compared to r0p6:

1. Switch back to iMXRT1052
2. New board shape and overall design
3. Use 3.4" LCD instead of 4"
4. Split into 3 boards (MB, PB, SB)
5. Use individual MOSFETs for V-drivers

### Rev 0.8 - Jan 2024

My original plan back in December for r0.7 is to have the main body of the camera up and running and then worry about the grip later. That didn't quite work out: there were bugs in the boards, and I forgot to order some components, etc. Moreover, as I am designing the grip, it's becoming clear that I need to make the main body a few mm larger than previously designed in December (it's still going to be smaller than the previous revision though.) So I need to do a re-spin of everything anyway. As a result I have been focusing on getting the design right rather than trying to bring up the old prototypes.

I also spent some time to fix my broken 3D printer so it's up and running again (though with questionable printing quality) so I can print models of the grip to make sure the feeling isn't absolutely garbage:

![r0p8_grip_test](/assets/r0p8_grip_test.jpg)

One major change I did is adding pogo pins for the lens. In the past the lens mount has been passive, meaning it only work with fully manual lenses. This obviously requires putting down these pins on to the board, so I am no longer able to keep the round cutout (which I like a lot):

![r0p8_mb](/assets/r0p8_mb.jpg)

While the mainboard has a smaller cutout, the power board's cut out has been enlarged. This is accomodate the holder for the IR cut filter lens.

![r0p8_pb](/assets/r0p8_pb.jpg)

After printing a model of the full camera, this revision is almost ready to go:

![r0p8_model](/assets/r0p8_model.jpg)

As said initially, the front piece would be metal. Other pieces can be SLA printed plastic. The following is a rendering of how it could look like:

![r0p8_render](/assets/r0p8_render.jpg)

Main changes compared to r0p7:

1. Add fan speed control
2. Add active lens contact
3. Add flash sync port
4. Allow tripod mount
5. Revise power supply design
6. Change connector type

### Rev 0.9 - Mar 2024

Except r0p8 didn't happen! By the time I finished the r0p8 version, my friend Yukidamayaki-san (who have been helping out on designing the analog side of things) said he just learned a good technique for driving the CCD: resonant drivers. I am not going into details of it right now as it wouldn't get implemented until r0.11 but this drives a pretty significant architecture change of the camera design: use an FPGA (like back in r0.1!).

Every revision ever since 0.2 has an architecture like the following (r0.6's block diagram used here):

![r0p6_bd](/assets/r0p6_bd.jpeg)

On r0.9, the MCU has been replaced by an SoC FPGA (Xilinx Zynq 7010) + small MCU for house keeping tasks. With the new architecture, the FPGA generates all the timing necessary to drive the CCD, and the image processing can be potentially done in hardware (not saying that I will, but I could).

![r0p9_bd](/assets/r0p9_bd.jpeg)

I have been tempted to use FPGA in this project, but didn't find enough reason to do so back then. On the other hand I have pretty good reason against it: it just makes things complicated. But this time I have a real reason to do so: the resonant driver. This requires more timing control than what AD9990 can provide (or at least, what I would like to program). I have complained in the video that AD9990 is just hard to program for. To generate even more elaborate waveform, FPGA is the way to go. Instead of adding another glue FPGA into the equation, I decided to simply replace the original MCU with a Zynq device. Which should also provide better CPU performance as well. But, to not test too many new thing at one time, I am still using the traditional driver for this revision (R0.9). If everything goes well I can switch to the new driver in the future and compare how much it improves.

I have also got the entire thing assembled:

![r0p9_1](/assets/r0p9_1.jpeg)

![r0p9_2](/assets/r0p9_2.jpeg)

![r0p9_3](/assets/r0p9_3.jpeg)

![r0p9_4](/assets/r0p9_4.jpeg)

![r0p9_5](/assets/r0p9_5.jpeg)

![r0p9_6](/assets/r0p9_6.jpeg)

![r0p9_7](/assets/r0p9_7.jpeg)

Most of the hardware works (Zynq, LCD, CCD driver, grip, etc.), but also due to the redesign, there are some serious bugs. So one more revision is needed before I switch to the new driver design.

Main changes compared to r0p8:

1. Combine MB and PB into IOB
2. Switch to Zynq 7010 (AGAIN?)
3. Use FPGA for timing gen
4. Remove USB data port

### Rev 0.10 - Jun 2024

As mentiond in r0.9 notes... this is a bug fix release. A new sensorboard release is also included with bug fixes, but it's not produced, instead, the older board is patched with bodge wires and reused. Some enclosure bug fix changes are included as well, so a new set of enclosure is also produced.

1. Fix LCD backlight range
2. Make the co-proc always ON
3. Add dedicated FPGA JTAG port
4. Add 2 additional buttons on the grip
5. Revise power supply to fix boot up problem
6. Fix grip button connection
7. Fix grip battery connector footprint and location
8. Use through-hole pogo-pins instead of SMT
9. Fix ribbon connector direction
10. Fix DDR power supply
11. New spring-loaded lens mount design

The color scheme has also been changed to black to better match with the lens:

![r0p10](/assets/r0p10.jpg)

### Rev 0.11 - Aug 2024

Two sub-versions exists for this revision: R0.11 and R0.11-B. The main change for the non-B version is an experimental reactive V driver is implemented. The B version still uses the old V driver.
