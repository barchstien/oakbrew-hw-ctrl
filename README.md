# oakbrew-hw-ctrl

C11 gcc
depends on : libconfig

## build & deploy
```bash
pacman -Sy --needed libconfig cmake gcc git
git clone git@github.com:barchstien/oakbrew-hw-ctrl.git
mkdir oakbrew-hw-ctrl-build
cd oakbrew-hw-ctrl-build
cmake ../oakbrew-hw-ctrl
make -j
#install the service
#assuming all is in /root/dev/
cd ../oakbrew-hw-ctrl
cp systemd/oakbrew-hw-control.service /etc/systemd/system
#enable at boot time
systemctl enable oakbrew-hw-control
#start
systemctl start oakbrew-hw-control
```

## config

enable SPI, I2C, I2S and compatible PCM512X driver (IQAudio)
```bash
#add to /boot/config.txt
dtparam=spi=on
dtparam=i2c_arm=on
dtparam=i2s=on
dtoverlay=iqaudio-dacplus

#add to /etc/modules-load.d/raspberrypi.conf
snd-bcm2835
i2c-dev
i2c-bcm2708

#alsa
pacman -S alsa-lib alsa-tools alsa-utils
```
