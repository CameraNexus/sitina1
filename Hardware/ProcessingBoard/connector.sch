EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 10 10
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 5400 4200 0    50   Output ~ 0
CSI_HSYNC
Text HLabel 5400 4100 0    50   Output ~ 0
CSI_VSYNC
Text HLabel 5400 5000 0    50   Input ~ 0
AFE_SYNC
Text HLabel 5400 4900 0    50   Input ~ 0
AFE_RST
Text HLabel 5400 4800 0    50   Input ~ 0
AFE_SCK
Text HLabel 5400 4700 0    50   Input ~ 0
AFE_SDATA
Text HLabel 5400 4600 0    50   Input ~ 0
AFE_SL
Text HLabel 5400 2400 0    50   Output ~ 0
CSI_D[13..0]
Text HLabel 5400 4400 0    50   Output ~ 0
CSI_PCLK
$Comp
L Connector_Generic_MountingPin:Conn_01x30_MountingPin J?
U 1 1 5F7F691E
P 6400 3900
F 0 "J?" H 6488 3814 50  0000 L CNN
F 1 "Conn_01x30_MountingPin" H 6488 3723 50  0000 L CNN
F 2 "" H 6400 3900 50  0001 C CNN
F 3 "~" H 6400 3900 50  0001 C CNN
	1    6400 3900
	1    0    0    -1  
$EndComp
Text Label 5600 2600 0    50   ~ 0
CSI_D0
Text Label 5600 2700 0    50   ~ 0
CSI_D1
Text Label 5600 2800 0    50   ~ 0
CSI_D2
Text Label 5600 2900 0    50   ~ 0
CSI_D3
Text Label 5600 3000 0    50   ~ 0
CSI_D4
Text Label 5600 3100 0    50   ~ 0
CSI_D5
Text Label 5600 3200 0    50   ~ 0
CSI_D6
Text Label 5600 3300 0    50   ~ 0
CSI_D7
Text Label 5600 3400 0    50   ~ 0
CSI_D8
Text Label 5600 3500 0    50   ~ 0
CSI_D9
Text Label 5600 3600 0    50   ~ 0
CSI_D10
Text Label 5600 3700 0    50   ~ 0
CSI_D11
Text Label 5600 3800 0    50   ~ 0
CSI_D12
Text Label 5600 3900 0    50   ~ 0
CSI_D13
Text Label 5600 4100 0    50   ~ 0
CSI_VSYNC
Text Label 5600 4200 0    50   ~ 0
CSI_HSYNC
Text Label 5600 4400 0    50   ~ 0
CSI_PCLK
Text Label 5600 4600 0    50   ~ 0
AFE_SL
Text Label 5600 4700 0    50   ~ 0
AFE_SDATA
Text Label 5600 4800 0    50   ~ 0
AFE_SCK
Text Label 5600 4900 0    50   ~ 0
AFE_RST
Text Label 5600 5000 0    50   ~ 0
AFE_SYNC
$Comp
L power:+3V3 #PWR?
U 1 1 5F8052CF
P 6000 5200
F 0 "#PWR?" H 6000 5050 50  0001 C CNN
F 1 "+3V3" V 6015 5328 50  0000 L CNN
F 2 "" H 6000 5200 50  0001 C CNN
F 3 "" H 6000 5200 50  0001 C CNN
	1    6000 5200
	0    -1   -1   0   
$EndComp
$Comp
L Sitina:+SYS #PWR?
U 1 1 5F80574E
P 6000 5300
F 0 "#PWR?" H 6000 5150 50  0001 C CNN
F 1 "+SYS" V 6015 5428 50  0000 L CNN
F 2 "" H 6000 5300 50  0001 C CNN
F 3 "" H 6000 5300 50  0001 C CNN
	1    6000 5300
	0    -1   -1   0   
$EndComp
Entry Wire Line
	5500 2500 5600 2600
Entry Wire Line
	5500 2600 5600 2700
Entry Wire Line
	5500 2700 5600 2800
Entry Wire Line
	5500 2800 5600 2900
Entry Wire Line
	5500 2900 5600 3000
Entry Wire Line
	5500 3000 5600 3100
Entry Wire Line
	5500 3100 5600 3200
Entry Wire Line
	5500 3200 5600 3300
Entry Wire Line
	5500 3300 5600 3400
Entry Wire Line
	5500 3400 5600 3500
Entry Wire Line
	5500 3500 5600 3600
Entry Wire Line
	5500 3600 5600 3700
Entry Wire Line
	5500 3700 5600 3800
Entry Wire Line
	5500 3800 5600 3900
Wire Bus Line
	5500 2400 5400 2400
Wire Wire Line
	6000 5200 6200 5200
Wire Wire Line
	6000 5300 6200 5300
Wire Wire Line
	6200 5400 6000 5400
Wire Wire Line
	6000 5400 6000 5300
Connection ~ 6000 5300
Wire Wire Line
	6200 2500 6100 2500
Wire Wire Line
	6100 2500 6100 4000
Wire Wire Line
	6100 5600 6400 5600
Wire Wire Line
	6200 4000 6100 4000
Connection ~ 6100 4000
Wire Wire Line
	6100 4000 6100 4300
Wire Wire Line
	6200 4300 6100 4300
Connection ~ 6100 4300
Wire Wire Line
	6100 4300 6100 4500
Wire Wire Line
	6200 4500 6100 4500
Connection ~ 6100 4500
Wire Wire Line
	6100 4500 6100 5100
Wire Wire Line
	6200 5100 6100 5100
Connection ~ 6100 5100
Wire Wire Line
	6100 5100 6100 5600
$Comp
L power:GND #PWR?
U 1 1 5F80D4CA
P 6400 5700
F 0 "#PWR?" H 6400 5450 50  0001 C CNN
F 1 "GND" H 6405 5527 50  0000 C CNN
F 2 "" H 6400 5700 50  0001 C CNN
F 3 "" H 6400 5700 50  0001 C CNN
	1    6400 5700
	1    0    0    -1  
$EndComp
Wire Wire Line
	6400 5700 6400 5600
Connection ~ 6400 5600
Wire Wire Line
	5600 2600 6200 2600
Wire Wire Line
	5600 2700 6200 2700
Wire Wire Line
	5600 2800 6200 2800
Wire Wire Line
	5600 2900 6200 2900
Wire Wire Line
	5600 3000 6200 3000
Wire Wire Line
	5600 3100 6200 3100
Wire Wire Line
	5600 3200 6200 3200
Wire Wire Line
	5600 3300 6200 3300
Wire Wire Line
	5600 3400 6200 3400
Wire Wire Line
	5600 3500 6200 3500
Wire Wire Line
	5600 3600 6200 3600
Wire Wire Line
	5600 3700 6200 3700
Wire Wire Line
	5600 3800 6200 3800
Wire Wire Line
	5600 3900 6200 3900
Wire Wire Line
	5400 4100 6200 4100
Wire Wire Line
	5400 4200 6200 4200
Wire Wire Line
	5400 4400 6200 4400
Wire Wire Line
	5400 4600 6200 4600
Wire Wire Line
	5400 4700 6200 4700
Wire Wire Line
	5400 4800 6200 4800
Wire Wire Line
	5400 4900 6200 4900
Wire Wire Line
	5400 5000 6200 5000
Wire Bus Line
	5500 2400 5500 3800
$EndSCHEMATC
