# GardenDiversity
<p align="center">
<img src="images/folder.jpg">
</p>

## Nature is as connected as IoT
The interest in local gardening has signifcantly risen after Covid-19. Recent surveys show that 80% of the European population are willing to privately contribute to conserve natural habitats. In addition, the new EU Biodiversity Strategy has been just announced aiming to put Europe’s biodiversity on a path to recovery by 2030. With over 150 million gardens in Europe, covering an area larger than all our national nature reserves put together, what we choose to do with our gardens really does matter for nature and climate.

## Biodiversity is measurable, but how?
In nature, everything’s interconnected – without trees, there are no flowers, without flowers, there are no insects, without insects, there are no birds. It is precisely this variation that’s important to keep everything in balance. Private gardens work exactly the same way – the more variety, the better. Garden designs, certainly within the urban environment, are crucial for biodiversity.
Recent research in ecological acoustics shows that the soundscape monitors very well the biodiversity of the underlying ecological system allowing to draw statistical inference how individual events impact each other.
 
What we are missing are effective possibilities to monitor the progresses we make in improving the local biodiversity, pollution and environmental health. Having possiblities to account for ecological improvements, we can offer incentives to compensate private efforts into preserving local natural habitats and improving quality of life in urban districts.

## Our solution
We are working on developing a low cost IoT node (less than 100 $) that enables to monitor and improve the local biodiversity over time. The node runs standalone on solar power and can be placed inside the own garden. It measures the soundscape of the garden environment with neural networks and classifies how it affects the biodiversity. The app is controllable by a smartphone and provide’s smart analytics to prevent negative effects, as garden pests. By combining the IoT node with a recommender engine, we send notices how seasonal gardening measures can improve the measured biodiversity. We are currently ramping up the project and search for development partners.

## Hardware

### Following hardware is required for the IoT Node:
- Raspberry Pi Zero 2 W
- Micro-SDHC Card
- Witty Pi 4 Mini Real Time Clock and Power Management 
- INMP 441 I2S Microphone
- Waveshare 16120 Solar Power Management Module
- Waveshare 19598 Solar Panel (18V 10W)
- LP-785060 Lithium-Polymer battery, 3,7V, 2500mAh with 2 Pin JST connector 
- USB-C to USB-A Cable

### To setup the hardware perform following steps:
1. Plug the Witty Pi 4 header on the Raspberry Pi Zero 2 W
2. Connect the Witty Pi 4 header with the USB-C to USB-A cable to the Solar Power Management Module
3. Connect the Solar Power Management Module with the Solar Panel
4. Connect the Li-Po battery with the Solar Power Management Module
5. Connect the Witty Pi 4 header to the I2S Microphone
6. GPIO 18 - Clock (SCK, BCK)
7. GPIO 19 - Word Select (WS, LRCK)
8. GPIO 20 - Data Line (DIN, SD)
9. 3.3V DC Power - VDD
10. Ground - GND
11. Start the Raspberry Pi Zero 2 W over the button at the Witty Pi 4 header

## Software Installation
1. Install the Raspberry Pi Imager
2. Install Raspberry Pi OS Lite (64 bit) on the SD Card with the Raspberry Pi Imager
3. Insert the SD Card into the Raspberry Pi and let it boot
4. Login into the Pi (Default account: pi, Password: raspberry).
5. Clone the project:
	```
	sudo apt-get -y update
	sudo apt-get -y upgrade
	sudo apt install git-all -y
	git clone https://github.com/pffelix/GardenDiversity
	```
	
6. Run the installation script
	```
	chmod +x GardenDiversity/src/firmware/installer.sh
	```
