EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 9 10
Title "Sitina Ne"
Date ""
Rev "R0P2"
Comp "ZephRay"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 2300 1100 0    50   Input ~ 0
SDRAM_BA[1..0]
Text Label 2700 1500 0    50   ~ 0
SDRAM_BA0
Text Label 2700 1600 0    50   ~ 0
SDRAM_BA1
Text Label 2700 1700 0    50   ~ 0
SDRAM_A0
Text Label 2700 1800 0    50   ~ 0
SDRAM_A1
Text Label 2700 1900 0    50   ~ 0
SDRAM_A2
Text Label 2700 2000 0    50   ~ 0
SDRAM_A3
Text Label 2700 2100 0    50   ~ 0
SDRAM_A4
Text Label 2700 2200 0    50   ~ 0
SDRAM_A5
Text Label 2700 2300 0    50   ~ 0
SDRAM_A6
Text Label 2700 2400 0    50   ~ 0
SDRAM_A7
Text Label 2700 2500 0    50   ~ 0
SDRAM_A8
Text Label 2700 2600 0    50   ~ 0
SDRAM_A9
Text Label 2700 2700 0    50   ~ 0
SDRAM_A10
Text Label 2700 2800 0    50   ~ 0
SDRAM_A11
Text Label 2700 2900 0    50   ~ 0
SDRAM_A12
$Comp
L Sitina:MT48LC32M16A2B4 U300
U 1 1 5D066AA0
P 3900 2600
F 0 "U300" H 3900 4181 50  0000 C CNN
F 1 "MT48LC16M16A2BG" H 3900 4090 50  0000 C CNN
F 2 "Footprints:BGA-54_9x9_8.0x14.0mm" H 3900 2600 50  0001 C CNN
F 3 "" H 3300 3850 50  0001 C CNN
	1    3900 2600
	1    0    0    -1  
$EndComp
Text Label 4700 1500 0    50   ~ 0
SDRAM_D0
Text Label 4700 1600 0    50   ~ 0
SDRAM_D1
Text Label 4700 1700 0    50   ~ 0
SDRAM_D2
Text Label 4700 1800 0    50   ~ 0
SDRAM_D3
Text Label 4700 1900 0    50   ~ 0
SDRAM_D4
Text Label 4700 2000 0    50   ~ 0
SDRAM_D5
Text Label 4700 2100 0    50   ~ 0
SDRAM_D6
Text Label 4700 2200 0    50   ~ 0
SDRAM_D7
Text Label 4700 2300 0    50   ~ 0
SDRAM_D8
Text Label 4700 2400 0    50   ~ 0
SDRAM_D9
Text Label 4700 2500 0    50   ~ 0
SDRAM_D10
Text Label 4700 2600 0    50   ~ 0
SDRAM_D11
Text Label 4700 2700 0    50   ~ 0
SDRAM_D12
Text Label 4700 2800 0    50   ~ 0
SDRAM_D13
Text Label 4700 2900 0    50   ~ 0
SDRAM_D14
Text Label 4700 3000 0    50   ~ 0
SDRAM_D15
Text Label 2700 3000 0    50   ~ 0
SDRAM_CS0
Text Label 2700 3100 0    50   ~ 0
SDRAM_CKE
Text Label 2700 3200 0    50   ~ 0
SDRAM_CLK
Text Label 2700 3300 0    50   ~ 0
SDRAM_DM0
Text Label 2700 3400 0    50   ~ 0
SDRAM_DM1
Text Label 2700 3500 0    50   ~ 0
SDRAM_WE
Text Label 2700 3600 0    50   ~ 0
SDRAM_CAS
Text Label 2700 3700 0    50   ~ 0
SDRAM_RAS
$Comp
L power:GND #PWR0102
U 1 1 5D06B228
P 3600 4000
F 0 "#PWR0102" H 3600 3750 50  0001 C CNN
F 1 "GND" H 3605 3827 50  0000 C CNN
F 2 "" H 3600 4000 50  0001 C CNN
F 3 "" H 3600 4000 50  0001 C CNN
	1    3600 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 4000 4100 4000
Connection ~ 3600 4000
Connection ~ 3700 4000
Wire Wire Line
	3700 4000 3600 4000
Connection ~ 3800 4000
Wire Wire Line
	3800 4000 3700 4000
Connection ~ 3900 4000
Wire Wire Line
	3900 4000 3800 4000
Connection ~ 4000 4000
Wire Wire Line
	4000 4000 3900 4000
Connection ~ 4100 4000
Wire Wire Line
	4100 4000 4000 4000
Text Label 2700 4700 0    50   ~ 0
SDRAM_BA0
Text Label 2700 4800 0    50   ~ 0
SDRAM_BA1
Text Label 2700 4900 0    50   ~ 0
SDRAM_A0
Text Label 2700 5000 0    50   ~ 0
SDRAM_A1
Text Label 2700 5100 0    50   ~ 0
SDRAM_A2
Text Label 2700 5200 0    50   ~ 0
SDRAM_A3
Text Label 2700 5300 0    50   ~ 0
SDRAM_A4
Text Label 2700 5400 0    50   ~ 0
SDRAM_A5
Text Label 2700 5500 0    50   ~ 0
SDRAM_A6
Text Label 2700 5600 0    50   ~ 0
SDRAM_A7
Text Label 2700 5700 0    50   ~ 0
SDRAM_A8
Text Label 2700 5800 0    50   ~ 0
SDRAM_A9
Text Label 2700 5900 0    50   ~ 0
SDRAM_A10
Text Label 2700 6000 0    50   ~ 0
SDRAM_A11
Text Label 2700 6100 0    50   ~ 0
SDRAM_A12
$Comp
L Sitina:MT48LC32M16A2B4 U301
U 1 1 5D0708D1
P 3900 5800
F 0 "U301" H 3900 7381 50  0000 C CNN
F 1 "MT48LC16M16A2BG" H 3900 7290 50  0000 C CNN
F 2 "Footprints:BGA-54_9x9_8.0x14.0mm" H 3900 5800 50  0001 C CNN
F 3 "" H 3300 7050 50  0001 C CNN
	1    3900 5800
	1    0    0    -1  
$EndComp
Text Label 4700 4700 0    50   ~ 0
SDRAM_D0
Text Label 4700 4800 0    50   ~ 0
SDRAM_D1
Text Label 4700 4900 0    50   ~ 0
SDRAM_D2
Text Label 4700 5000 0    50   ~ 0
SDRAM_D3
Text Label 4700 5100 0    50   ~ 0
SDRAM_D4
Text Label 4700 5200 0    50   ~ 0
SDRAM_D5
Text Label 4700 5300 0    50   ~ 0
SDRAM_D6
Text Label 4700 5400 0    50   ~ 0
SDRAM_D7
Text Label 4700 5500 0    50   ~ 0
SDRAM_D8
Text Label 4700 5600 0    50   ~ 0
SDRAM_D9
Text Label 4700 5700 0    50   ~ 0
SDRAM_D10
Text Label 4700 5800 0    50   ~ 0
SDRAM_D11
Text Label 4700 5900 0    50   ~ 0
SDRAM_D12
Text Label 4700 6000 0    50   ~ 0
SDRAM_D13
Text Label 4700 6100 0    50   ~ 0
SDRAM_D14
Text Label 4700 6200 0    50   ~ 0
SDRAM_D15
Text Label 2700 6200 0    50   ~ 0
SDRAM_CS0
Text Label 2700 6300 0    50   ~ 0
SDRAM_CKE
Text Label 2700 6400 0    50   ~ 0
SDRAM_CLK
Text Label 2700 6500 0    50   ~ 0
SDRAM_DM0
Text Label 2700 6600 0    50   ~ 0
SDRAM_DM1
Text Label 2700 6700 0    50   ~ 0
SDRAM_WE
Text Label 2700 6800 0    50   ~ 0
SDRAM_CAS
Text Label 2700 6900 0    50   ~ 0
SDRAM_RAS
$Comp
L power:GND #PWR0103
U 1 1 5D0708EF
P 3600 7200
F 0 "#PWR0103" H 3600 6950 50  0001 C CNN
F 1 "GND" H 3605 7027 50  0000 C CNN
F 2 "" H 3600 7200 50  0001 C CNN
F 3 "" H 3600 7200 50  0001 C CNN
	1    3600 7200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 7200 4100 7200
Connection ~ 3600 7200
Connection ~ 3700 7200
Wire Wire Line
	3700 7200 3600 7200
Connection ~ 3800 7200
Wire Wire Line
	3800 7200 3700 7200
Connection ~ 3900 7200
Wire Wire Line
	3900 7200 3800 7200
Connection ~ 4000 7200
Wire Wire Line
	4000 7200 3900 7200
Connection ~ 4100 7200
Wire Wire Line
	4100 7200 4000 7200
Text HLabel 2300 1200 0    50   Input ~ 0
SDRAM_A[12..0]
Entry Wire Line
	2600 1400 2700 1500
Entry Wire Line
	2600 1500 2700 1600
Entry Wire Line
	2500 1600 2600 1700
Entry Wire Line
	2500 1700 2600 1800
Entry Wire Line
	2500 1800 2600 1900
Entry Wire Line
	2500 1900 2600 2000
Entry Wire Line
	2500 2000 2600 2100
Entry Wire Line
	2500 2100 2600 2200
Entry Wire Line
	2500 2200 2600 2300
Entry Wire Line
	2500 2300 2600 2400
Entry Wire Line
	2500 2400 2600 2500
Entry Wire Line
	2500 2500 2600 2600
Entry Wire Line
	2500 2600 2600 2700
Entry Wire Line
	2500 2700 2600 2800
Entry Wire Line
	2500 2800 2600 2900
Entry Wire Line
	2400 3200 2500 3300
Entry Wire Line
	2400 3300 2500 3400
Entry Wire Line
	2600 4600 2700 4700
Entry Wire Line
	2600 4700 2700 4800
Entry Wire Line
	2500 4800 2600 4900
Entry Wire Line
	2500 4900 2600 5000
Entry Wire Line
	2500 5000 2600 5100
Entry Wire Line
	2500 5100 2600 5200
Entry Wire Line
	2500 5200 2600 5300
Entry Wire Line
	2500 5300 2600 5400
Entry Wire Line
	2500 5400 2600 5500
Entry Wire Line
	2500 5500 2600 5600
Entry Wire Line
	2500 5600 2600 5700
Entry Wire Line
	2500 5700 2600 5800
Entry Wire Line
	2500 5800 2600 5900
Entry Wire Line
	2500 5900 2600 6000
Entry Wire Line
	2500 6000 2600 6100
Entry Wire Line
	2400 6400 2500 6500
Entry Wire Line
	2400 6500 2500 6600
Wire Wire Line
	2700 1500 3100 1500
Wire Wire Line
	2700 1600 3100 1600
Wire Wire Line
	2600 1700 3100 1700
Wire Wire Line
	2600 1800 3100 1800
Wire Wire Line
	2600 1900 3100 1900
Wire Wire Line
	2600 2000 3100 2000
Wire Wire Line
	2600 2100 3100 2100
Wire Wire Line
	2600 2200 3100 2200
Wire Wire Line
	2600 2300 3100 2300
Wire Wire Line
	2600 2400 3100 2400
Wire Wire Line
	2600 2500 3100 2500
Wire Wire Line
	2600 2600 3100 2600
Wire Wire Line
	2600 2700 3100 2700
Wire Wire Line
	2600 2800 3100 2800
Wire Wire Line
	2600 2900 3100 2900
Wire Wire Line
	2500 3300 3100 3300
Wire Wire Line
	2500 3400 3100 3400
Wire Wire Line
	2700 4700 3100 4700
Wire Wire Line
	2700 4800 3100 4800
Wire Wire Line
	2600 4900 3100 4900
Wire Wire Line
	2600 5000 3100 5000
Wire Wire Line
	2600 5100 3100 5100
Wire Wire Line
	2600 5200 3100 5200
Wire Wire Line
	2600 5300 3100 5300
Wire Wire Line
	2600 5400 3100 5400
Wire Wire Line
	2600 5500 3100 5500
Wire Wire Line
	2600 5600 3100 5600
Wire Wire Line
	2600 5700 3100 5700
Wire Wire Line
	2600 5800 3100 5800
Wire Wire Line
	2600 5900 3100 5900
Wire Wire Line
	2600 6000 3100 6000
Wire Wire Line
	2600 6100 3100 6100
Wire Wire Line
	2500 6500 3100 6500
Wire Wire Line
	2500 6600 3100 6600
Text HLabel 2300 1300 0    50   Input ~ 0
SDRAM_DM[1..0]
Wire Bus Line
	2300 1100 2600 1100
Wire Bus Line
	2300 1200 2500 1200
Wire Bus Line
	2300 1300 2400 1300
Text HLabel 1800 3000 0    50   Input ~ 0
SDRAM_CS0
Text HLabel 1800 3100 0    50   Input ~ 0
SDRAM_CKE
Text HLabel 1800 3200 0    50   Input ~ 0
SDRAM_CLK
Text HLabel 1800 3500 0    50   Input ~ 0
SDRAM_WE
Text HLabel 1800 3600 0    50   Input ~ 0
SDRAM_CAS
Text HLabel 1800 3700 0    50   Input ~ 0
SDRAM_RAS
Wire Wire Line
	3100 6900 2300 6900
Wire Wire Line
	2300 6900 2300 3700
Wire Wire Line
	2300 3700 3100 3700
Wire Wire Line
	3100 6800 2200 6800
Wire Wire Line
	2200 6800 2200 3600
Wire Wire Line
	3100 3600 2200 3600
Wire Wire Line
	2100 6700 2100 3500
Wire Wire Line
	3100 3500 2100 3500
Wire Wire Line
	2000 6400 2000 3200
Wire Wire Line
	3100 6300 1900 6300
Wire Wire Line
	1900 6300 1900 3100
Text HLabel 1800 6200 0    50   Input ~ 0
SDRAM_CS1
Wire Wire Line
	3100 3100 1900 3100
Wire Wire Line
	1800 3000 3100 3000
Wire Wire Line
	1800 3100 1900 3100
Connection ~ 1900 3100
Wire Wire Line
	1800 3200 2000 3200
Connection ~ 2000 3200
Wire Wire Line
	1800 3500 2100 3500
Connection ~ 2100 3500
Wire Wire Line
	1800 3600 2200 3600
Connection ~ 2200 3600
Wire Wire Line
	1800 3700 2300 3700
Connection ~ 2300 3700
Wire Wire Line
	1800 6200 3100 6200
Wire Wire Line
	2100 6700 3100 6700
Entry Wire Line
	5200 1500 5300 1400
Entry Wire Line
	5200 1600 5300 1500
Entry Wire Line
	5200 1700 5300 1600
Entry Wire Line
	5200 1800 5300 1700
Entry Wire Line
	5200 1900 5300 1800
Entry Wire Line
	5200 2000 5300 1900
Entry Wire Line
	5200 2100 5300 2000
Entry Wire Line
	5200 2200 5300 2100
Entry Wire Line
	5200 2300 5300 2200
Entry Wire Line
	5200 2400 5300 2300
Entry Wire Line
	5200 2500 5300 2400
Entry Wire Line
	5200 2600 5300 2500
Entry Wire Line
	5200 2700 5300 2600
Entry Wire Line
	5200 2800 5300 2700
Entry Wire Line
	5200 2900 5300 2800
Entry Wire Line
	5200 3000 5300 2900
Entry Wire Line
	5200 4700 5300 4600
Entry Wire Line
	5200 4800 5300 4700
Entry Wire Line
	5200 4900 5300 4800
Entry Wire Line
	5200 5000 5300 4900
Entry Wire Line
	5200 5100 5300 5000
Entry Wire Line
	5200 5200 5300 5100
Entry Wire Line
	5200 5300 5300 5200
Entry Wire Line
	5200 5400 5300 5300
Entry Wire Line
	5200 5500 5300 5400
Entry Wire Line
	5200 5600 5300 5500
Entry Wire Line
	5200 5700 5300 5600
Entry Wire Line
	5200 5800 5300 5700
Entry Wire Line
	5200 5900 5300 5800
Entry Wire Line
	5200 6000 5300 5900
Entry Wire Line
	5200 6100 5300 6000
Entry Wire Line
	5200 6200 5300 6100
Wire Bus Line
	5300 1000 2300 1000
Text HLabel 2300 1000 0    50   BiDi ~ 0
SDRAM_D[15..0]
Wire Wire Line
	4700 1500 5200 1500
Wire Wire Line
	4700 1600 5200 1600
Wire Wire Line
	4700 1700 5200 1700
Wire Wire Line
	4700 1800 5200 1800
Wire Wire Line
	4700 1900 5200 1900
Wire Wire Line
	4700 2000 5200 2000
Wire Wire Line
	4700 2100 5200 2100
Wire Wire Line
	4700 2200 5200 2200
Wire Wire Line
	4700 2300 5200 2300
Wire Wire Line
	4700 2400 5200 2400
Wire Wire Line
	4700 2500 5200 2500
Wire Wire Line
	4700 2600 5200 2600
Wire Wire Line
	4700 2700 5200 2700
Wire Wire Line
	4700 2800 5200 2800
Wire Wire Line
	4700 2900 5200 2900
Wire Wire Line
	4700 3000 5200 3000
Wire Wire Line
	4700 4700 5200 4700
Wire Wire Line
	4700 4800 5200 4800
Wire Wire Line
	4700 4900 5200 4900
Wire Wire Line
	4700 5000 5200 5000
Wire Wire Line
	4700 5100 5200 5100
Wire Wire Line
	4700 5200 5200 5200
Wire Wire Line
	4700 5300 5200 5300
Wire Wire Line
	4700 5400 5200 5400
Wire Wire Line
	4700 5500 5200 5500
Wire Wire Line
	4700 5600 5200 5600
Wire Wire Line
	4700 5700 5200 5700
Wire Wire Line
	4700 5800 5200 5800
Wire Wire Line
	4700 5900 5200 5900
Wire Wire Line
	4700 6000 5200 6000
Wire Wire Line
	4700 6100 5200 6100
Wire Wire Line
	4700 6200 5200 6200
$Comp
L Device:C C300
U 1 1 5D100514
P 5700 1350
F 0 "C300" H 5815 1396 50  0000 L CNN
F 1 "100nF" H 5815 1305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 5738 1200 50  0001 C CNN
F 3 "~" H 5700 1350 50  0001 C CNN
	1    5700 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C301
U 1 1 5D101C9A
P 6100 1350
F 0 "C301" H 6215 1396 50  0000 L CNN
F 1 "100nF" H 6215 1305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 6138 1200 50  0001 C CNN
F 3 "~" H 6100 1350 50  0001 C CNN
	1    6100 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C302
U 1 1 5D106F82
P 6500 1350
F 0 "C302" H 6615 1396 50  0000 L CNN
F 1 "100nF" H 6615 1305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 6538 1200 50  0001 C CNN
F 3 "~" H 6500 1350 50  0001 C CNN
	1    6500 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C303
U 1 1 5D10D3C7
P 6900 1350
F 0 "C303" H 7015 1396 50  0000 L CNN
F 1 "100nF" H 7015 1305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 6938 1200 50  0001 C CNN
F 3 "~" H 6900 1350 50  0001 C CNN
	1    6900 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C304
U 1 1 5D10D3CD
P 7300 1350
F 0 "C304" H 7415 1396 50  0000 L CNN
F 1 "100nF" H 7415 1305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 7338 1200 50  0001 C CNN
F 3 "~" H 7300 1350 50  0001 C CNN
	1    7300 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C305
U 1 1 5D10D3D3
P 7700 1350
F 0 "C305" H 7815 1396 50  0000 L CNN
F 1 "100nF" H 7815 1305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 7738 1200 50  0001 C CNN
F 3 "~" H 7700 1350 50  0001 C CNN
	1    7700 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C306
U 1 1 5D112B1A
P 8100 1350
F 0 "C306" H 8215 1396 50  0000 L CNN
F 1 "100nF" H 8215 1305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 8138 1200 50  0001 C CNN
F 3 "~" H 8100 1350 50  0001 C CNN
	1    8100 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C308
U 1 1 5D125D5D
P 5700 4550
F 0 "C308" H 5815 4596 50  0000 L CNN
F 1 "100nF" H 5815 4505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 5738 4400 50  0001 C CNN
F 3 "~" H 5700 4550 50  0001 C CNN
	1    5700 4550
	1    0    0    -1  
$EndComp
$Comp
L Device:C C309
U 1 1 5D125D63
P 6100 4550
F 0 "C309" H 6215 4596 50  0000 L CNN
F 1 "100nF" H 6215 4505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 6138 4400 50  0001 C CNN
F 3 "~" H 6100 4550 50  0001 C CNN
	1    6100 4550
	1    0    0    -1  
$EndComp
$Comp
L Device:C C310
U 1 1 5D125D69
P 6500 4550
F 0 "C310" H 6615 4596 50  0000 L CNN
F 1 "100nF" H 6615 4505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 6538 4400 50  0001 C CNN
F 3 "~" H 6500 4550 50  0001 C CNN
	1    6500 4550
	1    0    0    -1  
$EndComp
$Comp
L Device:C C311
U 1 1 5D125D6F
P 6900 4550
F 0 "C311" H 7015 4596 50  0000 L CNN
F 1 "100nF" H 7015 4505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 6938 4400 50  0001 C CNN
F 3 "~" H 6900 4550 50  0001 C CNN
	1    6900 4550
	1    0    0    -1  
$EndComp
$Comp
L Device:C C312
U 1 1 5D125D75
P 7300 4550
F 0 "C312" H 7415 4596 50  0000 L CNN
F 1 "100nF" H 7415 4505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 7338 4400 50  0001 C CNN
F 3 "~" H 7300 4550 50  0001 C CNN
	1    7300 4550
	1    0    0    -1  
$EndComp
$Comp
L Device:C C313
U 1 1 5D125D7B
P 7700 4550
F 0 "C313" H 7815 4596 50  0000 L CNN
F 1 "100nF" H 7815 4505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 7738 4400 50  0001 C CNN
F 3 "~" H 7700 4550 50  0001 C CNN
	1    7700 4550
	1    0    0    -1  
$EndComp
$Comp
L Device:C C314
U 1 1 5D125D81
P 8100 4550
F 0 "C314" H 8215 4596 50  0000 L CNN
F 1 "100nF" H 8215 4505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 8138 4400 50  0001 C CNN
F 3 "~" H 8100 4550 50  0001 C CNN
	1    8100 4550
	1    0    0    -1  
$EndComp
$Comp
L Device:C C307
U 1 1 5D12C3B4
P 8500 1350
F 0 "C307" H 8615 1396 50  0000 L CNN
F 1 "4.7uF" H 8615 1305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 8538 1200 50  0001 C CNN
F 3 "~" H 8500 1350 50  0001 C CNN
	1    8500 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C315
U 1 1 5D12C732
P 8500 4550
F 0 "C315" H 8615 4596 50  0000 L CNN
F 1 "4.7uF" H 8615 4505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 8538 4400 50  0001 C CNN
F 3 "~" H 8500 4550 50  0001 C CNN
	1    8500 4550
	1    0    0    -1  
$EndComp
$Comp
L Device:C C316
U 1 1 5D12D320
P 9250 1350
F 0 "C316" H 9365 1396 50  0000 L CNN
F 1 "22uF" H 9365 1305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 9288 1200 50  0001 C CNN
F 3 "~" H 9250 1350 50  0001 C CNN
	1    9250 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:L L300
U 1 1 5D1306EF
P 9500 1200
F 0 "L300" V 9690 1200 50  0000 C CNN
F 1 "120R" V 9599 1200 50  0000 C CNN
F 2 "Inductor_SMD:L_0603_1608Metric" H 9500 1200 50  0001 C CNN
F 3 "~" H 9500 1200 50  0001 C CNN
	1    9500 1200
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 5D1319AA
P 9250 1500
F 0 "#PWR0104" H 9250 1250 50  0001 C CNN
F 1 "GND" H 9255 1327 50  0000 C CNN
F 2 "" H 9250 1500 50  0001 C CNN
F 3 "" H 9250 1500 50  0001 C CNN
	1    9250 1500
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR0106
U 1 1 5D131E9C
P 9750 1100
F 0 "#PWR0106" H 9750 950 50  0001 C CNN
F 1 "+3V3" H 9765 1273 50  0000 C CNN
F 2 "" H 9750 1100 50  0001 C CNN
F 3 "" H 9750 1100 50  0001 C CNN
	1    9750 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	9750 1100 9750 1200
Wire Wire Line
	9750 1200 9650 1200
Wire Wire Line
	9350 1200 9250 1200
Connection ~ 3700 1200
Wire Wire Line
	3700 1200 3600 1200
Connection ~ 3800 1200
Wire Wire Line
	3800 1200 3700 1200
Connection ~ 3900 1200
Wire Wire Line
	3900 1200 3800 1200
Connection ~ 4000 1200
Wire Wire Line
	4000 1200 3900 1200
Connection ~ 4100 1200
Wire Wire Line
	4100 1200 4000 1200
Wire Wire Line
	4200 1200 4100 1200
Connection ~ 6100 1200
Wire Wire Line
	6100 1200 5700 1200
Connection ~ 6500 1200
Wire Wire Line
	6500 1200 6100 1200
Connection ~ 6900 1200
Wire Wire Line
	6900 1200 6500 1200
Connection ~ 7300 1200
Wire Wire Line
	7300 1200 6900 1200
Connection ~ 7700 1200
Wire Wire Line
	7700 1200 7300 1200
Connection ~ 8100 1200
Wire Wire Line
	8100 1200 7700 1200
Connection ~ 8500 1200
Wire Wire Line
	8500 1200 8100 1200
Connection ~ 9250 1200
$Comp
L power:GND #PWR0107
U 1 1 5D13E2E6
P 8500 1500
F 0 "#PWR0107" H 8500 1250 50  0001 C CNN
F 1 "GND" H 8505 1327 50  0000 C CNN
F 2 "" H 8500 1500 50  0001 C CNN
F 3 "" H 8500 1500 50  0001 C CNN
	1    8500 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 1500 6100 1500
Connection ~ 8500 1500
Connection ~ 6100 1500
Wire Wire Line
	6100 1500 6500 1500
Connection ~ 6500 1500
Wire Wire Line
	6500 1500 6900 1500
Connection ~ 6900 1500
Wire Wire Line
	6900 1500 7300 1500
Connection ~ 7300 1500
Wire Wire Line
	7300 1500 7700 1500
Connection ~ 7700 1500
Wire Wire Line
	7700 1500 8100 1500
Connection ~ 8100 1500
Wire Wire Line
	8100 1500 8500 1500
$Comp
L power:GND #PWR0108
U 1 1 5D145CF3
P 8500 4700
F 0 "#PWR0108" H 8500 4450 50  0001 C CNN
F 1 "GND" H 8505 4527 50  0000 C CNN
F 2 "" H 8500 4700 50  0001 C CNN
F 3 "" H 8500 4700 50  0001 C CNN
	1    8500 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 4700 6100 4700
Connection ~ 8500 4700
Connection ~ 6100 4700
Wire Wire Line
	6100 4700 6500 4700
Connection ~ 6500 4700
Wire Wire Line
	6500 4700 6900 4700
Connection ~ 6900 4700
Wire Wire Line
	6900 4700 7300 4700
Connection ~ 7300 4700
Wire Wire Line
	7300 4700 7700 4700
Connection ~ 7700 4700
Wire Wire Line
	7700 4700 8100 4700
Connection ~ 8100 4700
Wire Wire Line
	8100 4700 8500 4700
Connection ~ 3700 4400
Wire Wire Line
	3700 4400 3600 4400
Connection ~ 3800 4400
Wire Wire Line
	3800 4400 3700 4400
Connection ~ 3900 4400
Wire Wire Line
	3900 4400 3800 4400
Connection ~ 4000 4400
Wire Wire Line
	4000 4400 3900 4400
Connection ~ 4100 4400
Wire Wire Line
	4100 4400 4000 4400
Wire Wire Line
	4200 4400 4100 4400
Connection ~ 6100 4400
Wire Wire Line
	6100 4400 5700 4400
Connection ~ 6500 4400
Wire Wire Line
	6500 4400 6100 4400
Connection ~ 6900 4400
Wire Wire Line
	6900 4400 6500 4400
Connection ~ 7300 4400
Wire Wire Line
	7300 4400 6900 4400
Connection ~ 7700 4400
Wire Wire Line
	7700 4400 7300 4400
Connection ~ 8100 4400
Wire Wire Line
	8100 4400 7700 4400
Connection ~ 8500 4400
Wire Wire Line
	8500 4400 8100 4400
Wire Wire Line
	5700 1200 4200 1200
Connection ~ 5700 1200
Connection ~ 4200 1200
Wire Wire Line
	4200 4400 5700 4400
Connection ~ 4200 4400
Connection ~ 5700 4400
$Comp
L Device:C C317
U 1 1 61B6E18D
P 9250 4550
F 0 "C317" H 9365 4596 50  0000 L CNN
F 1 "22uF" H 9365 4505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 9288 4400 50  0001 C CNN
F 3 "~" H 9250 4550 50  0001 C CNN
	1    9250 4550
	1    0    0    -1  
$EndComp
$Comp
L Device:L L301
U 1 1 61B6E193
P 9500 4400
F 0 "L301" V 9690 4400 50  0000 C CNN
F 1 "120R" V 9599 4400 50  0000 C CNN
F 2 "Inductor_SMD:L_0603_1608Metric" H 9500 4400 50  0001 C CNN
F 3 "~" H 9500 4400 50  0001 C CNN
	1    9500 4400
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 61B6E199
P 9250 4700
F 0 "#PWR0109" H 9250 4450 50  0001 C CNN
F 1 "GND" H 9255 4527 50  0000 C CNN
F 2 "" H 9250 4700 50  0001 C CNN
F 3 "" H 9250 4700 50  0001 C CNN
	1    9250 4700
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR0110
U 1 1 61B6E19F
P 9750 4300
F 0 "#PWR0110" H 9750 4150 50  0001 C CNN
F 1 "+3V3" H 9765 4473 50  0000 C CNN
F 2 "" H 9750 4300 50  0001 C CNN
F 3 "" H 9750 4300 50  0001 C CNN
	1    9750 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	9750 4300 9750 4400
Wire Wire Line
	9750 4400 9650 4400
Wire Wire Line
	9350 4400 9250 4400
Connection ~ 9250 4400
Wire Wire Line
	8500 1200 9250 1200
Wire Wire Line
	8500 4400 9250 4400
Wire Wire Line
	2000 6400 3100 6400
Wire Wire Line
	2000 3200 3100 3200
Wire Bus Line
	2400 1300 2400 6500
Wire Bus Line
	2600 1100 2600 4700
Wire Bus Line
	2500 1200 2500 6000
Wire Bus Line
	5300 1000 5300 6100
$EndSCHEMATC