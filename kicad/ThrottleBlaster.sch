EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "ThrottleBlaster"
Date "2024-03-24"
Rev "0.3"
Comp "Scrap Computing"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L power:GND #PWR0103
U 1 1 65F56C1B
P 6050 2950
F 0 "#PWR0103" H 6050 2700 50  0001 C CNN
F 1 "GND" V 6055 2822 50  0000 R CNN
F 2 "" H 6050 2950 50  0001 C CNN
F 3 "" H 6050 2950 50  0001 C CNN
	1    6050 2950
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 65F56E23
P 6050 3450
F 0 "#PWR0104" H 6050 3200 50  0001 C CNN
F 1 "GND" V 6055 3322 50  0000 R CNN
F 2 "" H 6050 3450 50  0001 C CNN
F 3 "" H 6050 3450 50  0001 C CNN
	1    6050 3450
	0    -1   -1   0   
$EndComp
$Comp
L Device:D_Schottky D1
U 1 1 65F5820D
P 6200 1900
F 0 "D1" H 6200 2117 50  0000 C CNN
F 1 "D_Schottky" H 6200 2026 50  0000 C CNN
F 2 "Diode_THT:D_5W_P10.16mm_Horizontal" H 6200 1900 50  0001 C CNN
F 3 "~" H 6200 1900 50  0001 C CNN
	1    6200 1900
	0    -1   -1   0   
$EndComp
$Comp
L Switch:SW_Push SW1
U 1 1 65F5C34E
P 2250 3850
F 0 "SW1" V 2204 3998 50  0000 L CNN
F 1 "SW_Push" V 2295 3998 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 2250 4050 50  0001 C CNN
F 3 "~" H 2250 4050 50  0001 C CNN
	1    2250 3850
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_POT RV1
U 1 1 65F5DC08
P 7000 3300
F 0 "RV1" H 6930 3254 50  0000 R CNN
F 1 "10K" H 6930 3345 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 7000 3300 50  0001 C CNN
F 3 "~" H 7000 3300 50  0001 C CNN
	1    7000 3300
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 65F54A14
P 6100 1150
F 0 "#PWR0105" H 6100 900 50  0001 C CNN
F 1 "GND" V 6105 1022 50  0000 R CNN
F 2 "" H 6100 1150 50  0001 C CNN
F 3 "" H 6100 1150 50  0001 C CNN
	1    6100 1150
	1    0    0    -1  
$EndComp
NoConn ~ 6000 1150
NoConn ~ 5900 1150
Wire Wire Line
	6050 2850 6200 2850
$Comp
L ThrottleBlaster:FloppyPower U3
U 1 1 65F52717
P 6300 1150
F 0 "U3" V 5800 900 50  0000 C CNN
F 1 "FloppyPower" V 5900 900 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Horizontal" H 6300 1150 50  0001 C CNN
F 3 "" H 6300 1150 50  0001 C CNN
	1    6300 1150
	0    1    1    0   
$EndComp
$Comp
L Device:Rotary_Encoder_Switch SW2
U 1 1 65FB8E0E
P 3550 4200
F 0 "SW2" V 3500 3800 50  0000 L CNN
F 1 "Rotary_Encoder_Switch" V 3600 3100 50  0000 L CNN
F 2 "Rotary_Encoder:RotaryEncoder_Alps_EC11E-Switch_Vertical_H20mm" H 3400 4360 50  0001 C CNN
F 3 "~" H 3550 4460 50  0001 C CNN
	1    3550 4200
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0107
U 1 1 65FEE5A0
P 5050 3450
F 0 "#PWR0107" H 5050 3200 50  0001 C CNN
F 1 "GND" V 5055 3322 50  0000 R CNN
F 2 "" H 5050 3450 50  0001 C CNN
F 3 "" H 5050 3450 50  0001 C CNN
	1    5050 3450
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0108
U 1 1 65FEE9EC
P 5050 2950
F 0 "#PWR0108" H 5050 2700 50  0001 C CNN
F 1 "GND" V 5055 2822 50  0000 R CNN
F 2 "" H 5050 2950 50  0001 C CNN
F 3 "" H 5050 2950 50  0001 C CNN
	1    5050 2950
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 65FF082D
P 2250 4050
F 0 "#PWR0109" H 2250 3800 50  0001 C CNN
F 1 "GND" H 2255 3877 50  0000 C CNN
F 2 "" H 2250 4050 50  0001 C CNN
F 3 "" H 2250 4050 50  0001 C CNN
	1    2250 4050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0110
U 1 1 65FF6900
P 7000 3450
F 0 "#PWR0110" H 7000 3200 50  0001 C CNN
F 1 "GND" H 7005 3277 50  0000 C CNN
F 2 "" H 7000 3450 50  0001 C CNN
F 3 "" H 7000 3450 50  0001 C CNN
	1    7000 3450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0112
U 1 1 6601F27F
P 3550 3900
F 0 "#PWR0112" H 3550 3650 50  0001 C CNN
F 1 "GND" H 3555 3727 50  0000 C CNN
F 2 "" H 3550 3900 50  0001 C CNN
F 3 "" H 3550 3900 50  0001 C CNN
	1    3550 3900
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0113
U 1 1 6601FACD
P 3450 4500
F 0 "#PWR0113" H 3450 4250 50  0001 C CNN
F 1 "GND" H 3455 4327 50  0000 C CNN
F 2 "" H 3450 4500 50  0001 C CNN
F 3 "" H 3450 4500 50  0001 C CNN
	1    3450 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 3150 3650 3900
Wire Wire Line
	3450 3900 3450 3050
Wire Wire Line
	3800 3250 3800 4500
Wire Wire Line
	3800 4500 3650 4500
Wire Wire Line
	7400 4300 7400 4200
$Comp
L Device:R R1
U 1 1 65FCF503
P 6950 4000
F 0 "R1" V 7157 4000 50  0000 C CNN
F 1 "1K" V 7066 4000 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 6880 4000 50  0001 C CNN
F 3 "~" H 6950 4000 50  0001 C CNN
	1    6950 4000
	0    -1   -1   0   
$EndComp
$Comp
L Transistor_FET:2N7000 Q1
U 1 1 65F5EF5A
P 7300 4000
F 0 "Q1" H 7504 4046 50  0000 L CNN
F 1 "2N7000" H 7504 3955 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 7500 3925 50  0001 L CIN
F 3 "https://www.onsemi.com/pub/Collateral/NDS7002A-D.PDF" H 7300 4000 50  0001 L CNN
	1    7300 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	6850 3350 6850 3300
$Comp
L power:GND #PWR0111
U 1 1 66001E51
P 3200 2950
F 0 "#PWR0111" H 3200 2700 50  0001 C CNN
F 1 "GND" V 3205 2822 50  0000 R CNN
F 2 "" H 3200 2950 50  0001 C CNN
F 3 "" H 3200 2950 50  0001 C CNN
	1    3200 2950
	0    -1   -1   0   
$EndComp
$Comp
L ThrottleBlaster:7SegmentDisplayTM1637 U1
U 1 1 65F6412F
P 3200 2600
F 0 "U1" H 2350 2250 50  0000 C CNN
F 1 "7SegmentDisplayTM1637" H 2350 2350 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 3200 2600 50  0001 C CNN
F 3 "" H 3200 2600 50  0001 C CNN
	1    3200 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 2750 5050 2750
Wire Wire Line
	3200 2850 5050 2850
Wire Wire Line
	6200 2050 6200 2200
Connection ~ 3800 3250
Wire Wire Line
	3800 3250 5050 3250
Wire Wire Line
	3450 3050 5050 3050
Wire Wire Line
	3650 3150 5050 3150
$Comp
L Jumper:Jumper_2_Open JP1
U 1 1 661001AF
P 4550 3350
F 0 "JP1" H 4596 3262 50  0000 R CNN
F 1 "MODE" H 4450 3300 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 4550 3350 50  0001 C CNN
F 3 "~" H 4550 3350 50  0001 C CNN
	1    4550 3350
	1    0    0    -1  
$EndComp
$Comp
L Jumper:Jumper_2_Open JP2
U 1 1 66101426
P 4550 3550
F 0 "JP2" H 4596 3462 50  0000 R CNN
F 1 "POT Enable" H 4450 3500 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 4550 3550 50  0001 C CNN
F 3 "~" H 4550 3550 50  0001 C CNN
	1    4550 3550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 66111830
P 4350 3350
F 0 "#PWR01" H 4350 3100 50  0001 C CNN
F 1 "GND" V 4355 3177 50  0000 C CNN
F 2 "" H 4350 3350 50  0001 C CNN
F 3 "" H 4350 3350 50  0001 C CNN
	1    4350 3350
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR02
U 1 1 66112007
P 4350 3550
F 0 "#PWR02" H 4350 3300 50  0001 C CNN
F 1 "GND" V 4355 3377 50  0000 C CNN
F 2 "" H 4350 3550 50  0001 C CNN
F 3 "" H 4350 3550 50  0001 C CNN
	1    4350 3550
	0    1    1    0   
$EndComp
Wire Wire Line
	6050 3150 6400 3150
Wire Wire Line
	6050 3350 6700 3350
Wire Wire Line
	6050 3550 6500 3550
$Comp
L Connector:Conn_01x01_Male J1
U 1 1 65FDF326
P 8450 3800
F 0 "J1" H 8422 3732 50  0000 R CNN
F 1 "Conn_01x01_Male" H 8422 3823 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Horizontal" H 8450 3800 50  0001 C CNN
F 3 "~" H 8450 3800 50  0001 C CNN
	1    8450 3800
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0114
U 1 1 65FDD588
P 7400 4300
F 0 "#PWR0114" H 7400 4050 50  0001 C CNN
F 1 "GND" H 7405 4127 50  0000 C CNN
F 2 "" H 7400 4300 50  0001 C CNN
F 3 "" H 7400 4300 50  0001 C CNN
	1    7400 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	7400 3800 7700 3800
Wire Wire Line
	6500 4000 6800 4000
Wire Wire Line
	6500 3550 6500 4000
Wire Wire Line
	3200 3050 3400 3050
Wire Wire Line
	3400 3050 3400 2050
Wire Wire Line
	3400 2050 6200 2050
NoConn ~ 6050 3650
NoConn ~ 6050 3750
NoConn ~ 6050 3850
NoConn ~ 6050 3950
NoConn ~ 6050 4050
NoConn ~ 6050 4150
NoConn ~ 6050 4250
NoConn ~ 6050 4350
NoConn ~ 6050 4450
NoConn ~ 6050 4550
NoConn ~ 6050 4650
NoConn ~ 5050 4650
NoConn ~ 5050 4550
NoConn ~ 5050 4450
NoConn ~ 5050 4350
NoConn ~ 5050 4250
NoConn ~ 5050 4150
NoConn ~ 5050 4050
NoConn ~ 5050 3950
NoConn ~ 6050 3250
NoConn ~ 6050 3050
NoConn ~ 6050 2750
Wire Wire Line
	6200 1550 6200 1750
Wire Wire Line
	6200 1150 6200 1550
Connection ~ 6200 1550
$Comp
L power:+5V #PWR0106
U 1 1 65FB6A22
P 6200 1550
F 0 "#PWR0106" H 6200 1400 50  0001 C CNN
F 1 "+5V" V 6215 1678 50  0000 L CNN
F 2 "" H 6200 1550 50  0001 C CNN
F 3 "" H 6200 1550 50  0001 C CNN
	1    6200 1550
	0    1    1    0   
$EndComp
Connection ~ 6200 2050
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 6623D785
P 6200 2200
F 0 "#FLG0101" H 6200 2275 50  0001 C CNN
F 1 "PWR_FLAG" V 6200 2328 50  0000 L CNN
F 2 "" H 6200 2200 50  0001 C CNN
F 3 "~" H 6200 2200 50  0001 C CNN
	1    6200 2200
	0    -1   -1   0   
$EndComp
$Comp
L Device:C C1
U 1 1 65F774D8
P 6700 3500
F 0 "C1" H 6815 3546 50  0000 L CNN
F 1 "100pF" H 6815 3455 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W1.6mm_P2.50mm" H 6738 3350 50  0001 C CNN
F 3 "~" H 6700 3500 50  0001 C CNN
	1    6700 3500
	1    0    0    -1  
$EndComp
Connection ~ 6700 3350
Wire Wire Line
	6700 3350 6850 3350
$Comp
L power:GND #PWR03
U 1 1 65F77A5D
P 6700 3650
F 0 "#PWR03" H 6700 3400 50  0001 C CNN
F 1 "GND" H 6705 3477 50  0000 C CNN
F 2 "" H 6700 3650 50  0001 C CNN
F 3 "" H 6700 3650 50  0001 C CNN
	1    6700 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 3350 5050 3350
Wire Wire Line
	4750 3550 5050 3550
Wire Wire Line
	2250 3250 2250 3650
Wire Wire Line
	2250 3250 3800 3250
Wire Wire Line
	2700 3650 5050 3650
$Comp
L power:GND #PWR04
U 1 1 65F993B3
P 2700 4050
F 0 "#PWR04" H 2700 3800 50  0001 C CNN
F 1 "GND" H 2705 3877 50  0000 C CNN
F 2 "" H 2700 4050 50  0001 C CNN
F 3 "" H 2700 4050 50  0001 C CNN
	1    2700 4050
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW3
U 1 1 65F988F7
P 2700 3850
F 0 "SW3" V 2746 3802 50  0000 R CNN
F 1 "SW_Push" V 2655 3802 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 2700 4050 50  0001 C CNN
F 3 "~" H 2700 4050 50  0001 C CNN
	1    2700 3850
	0    -1   -1   0   
$EndComp
$Comp
L Interface_UART:MAX3232 U4
U 1 1 660281FF
P 5000 6100
F 0 "U4" V 4954 7344 50  0000 L CNN
F 1 "MAX3232" V 5045 7344 50  0000 L CNN
F 2 "Package_SO:SOIC-16W_5.3x10.2mm_P1.27mm" H 5050 5050 50  0001 L CNN
F 3 "https://datasheets.maximintegrated.com/en/ds/MAX3222-MAX3241.pdf" H 5000 6200 50  0001 C CNN
	1    5000 6100
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 6602AE2B
P 3800 6100
F 0 "#PWR0101" H 3800 5850 50  0001 C CNN
F 1 "GND" H 3805 5927 50  0000 C CNN
F 2 "" H 3800 6100 50  0001 C CNN
F 3 "" H 3800 6100 50  0001 C CNN
	1    3800 6100
	1    0    0    -1  
$EndComp
Connection ~ 6200 2200
Wire Wire Line
	6200 2200 6200 2850
Wire Wire Line
	4900 3750 5050 3750
$Comp
L ThrottleBlaster:RaspberryPi_Pico U2
U 1 1 65F5261C
P 4450 4900
F 0 "U2" V 5550 6100 50  0000 C CNN
F 1 "RaspberryPi_Pico" V 5550 5500 50  0000 C CNN
F 2 "ThrottleBlaster:raspberry_pi_pico" H 3750 1400 50  0001 C CNN
F 3 "" H 3750 1400 50  0001 C CNN
	1    4450 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 6100 6400 6100
Wire Wire Line
	4900 3750 4900 5300
Wire Wire Line
	4500 5300 4500 3850
Wire Wire Line
	4500 3850 5050 3850
NoConn ~ 4300 5300
NoConn ~ 4700 6900
$Comp
L power:GND #PWR0102
U 1 1 6603468D
P 4700 5300
F 0 "#PWR0102" H 4700 5050 50  0001 C CNN
F 1 "GND" H 4705 5127 50  0000 C CNN
F 2 "" H 4700 5300 50  0001 C CNN
F 3 "" H 4700 5300 50  0001 C CNN
	1    4700 5300
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0115
U 1 1 66034B58
P 4300 6900
F 0 "#PWR0115" H 4300 6650 50  0001 C CNN
F 1 "GND" H 4305 6727 50  0000 C CNN
F 2 "" H 4300 6900 50  0001 C CNN
F 3 "" H 4300 6900 50  0001 C CNN
	1    4300 6900
	1    0    0    -1  
$EndComp
$Comp
L Device:C C6
U 1 1 660351CA
P 6400 6250
F 0 "C6" H 6515 6296 50  0000 L CNN
F 1 "1uF" H 6515 6205 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 6438 6100 50  0001 C CNN
F 3 "~" H 6400 6250 50  0001 C CNN
	1    6400 6250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0116
U 1 1 66035CEC
P 6400 6400
F 0 "#PWR0116" H 6400 6150 50  0001 C CNN
F 1 "GND" H 6405 6227 50  0000 C CNN
F 2 "" H 6400 6400 50  0001 C CNN
F 3 "" H 6400 6400 50  0001 C CNN
	1    6400 6400
	1    0    0    -1  
$EndComp
$Comp
L Device:C C4
U 1 1 660361DE
P 5750 5300
F 0 "C4" V 5498 5300 50  0000 C CNN
F 1 "1uF" V 5589 5300 50  0000 C CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 5788 5150 50  0001 C CNN
F 3 "~" H 5750 5300 50  0001 C CNN
	1    5750 5300
	0    1    1    0   
$EndComp
$Comp
L Device:C C5
U 1 1 660368AB
P 5750 6900
F 0 "C5" V 5498 6900 50  0000 C CNN
F 1 "1uF" V 5589 6900 50  0000 C CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 5788 6750 50  0001 C CNN
F 3 "~" H 5750 6900 50  0001 C CNN
	1    5750 6900
	0    1    1    0   
$EndComp
$Comp
L Device:C C3
U 1 1 660373B4
P 5400 7050
F 0 "C3" H 5285 7004 50  0000 R CNN
F 1 "1uF" H 5285 7095 50  0000 R CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 5438 6900 50  0001 C CNN
F 3 "~" H 5400 7050 50  0001 C CNN
	1    5400 7050
	-1   0    0    1   
$EndComp
$Comp
L Device:C C2
U 1 1 66038E8A
P 5100 7050
F 0 "C2" H 5215 7096 50  0000 L CNN
F 1 "1uF" H 5215 7005 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 5138 6900 50  0001 C CNN
F 3 "~" H 5100 7050 50  0001 C CNN
	1    5100 7050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0117
U 1 1 66039913
P 5100 7200
F 0 "#PWR0117" H 5100 6950 50  0001 C CNN
F 1 "GND" H 5105 7027 50  0000 C CNN
F 2 "" H 5100 7200 50  0001 C CNN
F 3 "" H 5100 7200 50  0001 C CNN
	1    5100 7200
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x03_Male J2
U 1 1 6603B4BC
P 3600 7400
F 0 "J2" H 3708 7681 50  0000 C CNN
F 1 "Serial" H 3708 7590 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 3600 7400 50  0001 C CNN
F 3 "~" H 3600 7400 50  0001 C CNN
	1    3600 7400
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 7300 4500 7300
Wire Wire Line
	4500 7300 4500 6900
Wire Wire Line
	3800 7500 4900 7500
Wire Wire Line
	4900 7500 4900 6900
$Comp
L power:GND #PWR0118
U 1 1 6603F193
P 3800 7400
F 0 "#PWR0118" H 3800 7150 50  0001 C CNN
F 1 "GND" V 3805 7272 50  0000 R CNN
F 2 "" H 3800 7400 50  0001 C CNN
F 3 "" H 3800 7400 50  0001 C CNN
	1    3800 7400
	0    -1   -1   0   
$EndComp
Text Label 3950 7300 0    50   ~ 0
Rx
Text Label 3950 7500 0    50   ~ 0
Tx
$Comp
L Device:R R2
U 1 1 6607F259
P 7850 3800
F 0 "R2" V 7643 3800 50  0000 C CNN
F 1 "100 Ohms" V 7734 3800 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 7780 3800 50  0001 C CNN
F 3 "~" H 7850 3800 50  0001 C CNN
	1    7850 3800
	0    1    1    0   
$EndComp
Wire Wire Line
	8000 3800 8250 3800
$Comp
L power:GND #PWR0119
U 1 1 660C571F
P 5400 7200
F 0 "#PWR0119" H 5400 6950 50  0001 C CNN
F 1 "GND" H 5405 7027 50  0000 C CNN
F 2 "" H 5400 7200 50  0001 C CNN
F 3 "" H 5400 7200 50  0001 C CNN
	1    5400 7200
	1    0    0    -1  
$EndComp
Wire Wire Line
	6400 6100 6400 3150
Connection ~ 6400 6100
Connection ~ 6400 3150
Wire Wire Line
	6400 3150 7000 3150
$EndSCHEMATC
