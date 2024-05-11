# update system
sudo apt-get -y update
sudo apt-get -y upgrade
cd ~

# install Git
sudo apt install git-all -y

# clone Github repository
git clone https://github.com/pffelix/GardenDiversity

# install Python
sudo apt install python3.9
sudo apt install python3-pip -y

# install I2S microphone driver
sudo pip3 install --upgrade adafruit-python-shell
wget https://raw.githubusercontent.com/adafruit/Raspberry-Pi-Installer-Scripts/master/i2smic.py
sudo python3 i2smic.py

# install Witty Pi 4 
wget https://www.uugear.com/repo/WittyPi4/install.sh
sudo sh install.sh
cd uwi
./diagnose.sh
cd ~

# install Python modules
pip3 install numpy
sudo apt-get install libasound2-dev -y
pip3 install pyalsaaudio
pip3 install paho-mqtt

# install Tensorflow version 2.10.0 with custom operators for Python 3.9 and 64 bit Raspberry Pi OS (adapt if version changes)
sudo -H pip3 install --no-cache-dir https://github.com/PINTO0309/TensorflowLite-bin/releases/download/v2.10.0/tflite_runtime-2.10.0-cp39-none-linux_aarch64.whl
pip3 install -U numpy

# install service
sudo touch /lib/systemd/system/gardendiversity.service
echo "[Unit]
Description=GardenDiversity Service

[Service]
Type=idle
User=pi
ExecStart=/usr/bin/python /home/pi/GardenDiversity/src/raspberry/firmware/src/main.py

[Install]
WantedBy=multi-user.target" | sudo tee /lib/systemd/system/gardendiversity.service -a
sudo chmod 644 /lib/systemd/system/gardendiversity.service
sudo systemctl daemon-reload
sudo systemctl enable gardendiversity.service

# reboot
sudo reboot