EESchema Schematic File Version 4
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Switch:SW_Push SW2
U 1 1 5BEFC880
P 3600 1850
F 0 "SW2" H 3600 2135 50  0000 C CNN
F 1 "SW_Push" H 3600 2044 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 3600 2050 50  0001 C CNN
F 3 "" H 3600 2050 50  0001 C CNN
	1    3600 1850
	1    0    0    -1  
$EndComp
$Comp
L MCU_Microchip_ATtiny:ATtiny85-20PU U1
U 1 1 5BEFABCE
P 2800 2050
F 0 "U1" H 2270 2096 50  0000 R CNN
F 1 "ATtiny85-20PU" H 2270 2005 50  0000 R CNN
F 2 "Package_DIP:DIP-8_W7.62mm" H 2800 2050 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf" H 2800 2050 50  0001 C CNN
	1    2800 2050
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push_SPDT SW1
U 1 1 5BEFCD7C
P 1200 3750
F 0 "SW1" H 1200 4035 50  0000 C CNN
F 1 "SW_Push_SPDT" H 1200 3944 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1200 3750 50  0001 C CNN
F 3 "" H 1200 3750 50  0001 C CNN
	1    1200 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	1400 3650 1000 3650
Wire Wire Line
	1000 3650 1000 3750
Wire Wire Line
	2400 3850 2400 2650
Wire Wire Line
	2400 2650 2800 2650
Connection ~ 2400 3850
Wire Wire Line
	2400 3850 1400 3850
Wire Wire Line
	2800 1450 2100 1450
Wire Wire Line
	2100 1450 2100 3150
Wire Wire Line
	2100 3150 2800 3150
Wire Wire Line
	3800 1850 3800 3850
Wire Wire Line
	3400 1750 3900 1750
Wire Wire Line
	3900 1750 3900 3450
Wire Wire Line
	3900 3450 3150 3450
Wire Wire Line
	3400 1950 3700 1950
Wire Wire Line
	3700 1950 3700 3550
Wire Wire Line
	3700 3550 3150 3550
Wire Wire Line
	2400 3850 2800 3850
$Comp
L ssd1306-i2c:ssd1306-i2c U2
U 1 1 5BEFD998
P 2800 3500
F 0 "U2" H 2522 3546 50  0000 R CNN
F 1 "ssd1306-i2c" H 2522 3455 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 2450 3550 50  0001 C CNN
F 3 "" H 2450 3550 50  0001 C CNN
	1    2800 3500
	1    0    0    -1  
$EndComp
Connection ~ 2800 3850
Wire Wire Line
	2800 3850 3800 3850
NoConn ~ 3400 2050
NoConn ~ 3400 2150
NoConn ~ 3400 2250
$Comp
L Device:Battery_Cell BT1
U 1 1 5BEFE105
P 1000 2500
F 0 "BT1" H 1118 2596 50  0000 L CNN
F 1 "Battery_Cell" H 1118 2505 50  0000 L CNN
F 2 "Battery:BatteryHolder_MPD_BC2003_1x2032" V 1000 2560 50  0001 C CNN
F 3 "~" V 1000 2560 50  0001 C CNN
	1    1000 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 1450 1000 1450
Wire Wire Line
	1000 1450 1000 2300
Connection ~ 2100 1450
Wire Wire Line
	1000 2600 1000 3650
Connection ~ 1000 3650
$EndSCHEMATC
