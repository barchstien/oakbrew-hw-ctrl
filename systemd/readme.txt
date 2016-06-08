#script for systemd
cp oakbrew-hw-control.service /etc/systemd/system
#enable at boot time
systemctl enable oakbrew-hw-control
#start
systemctl start oakbrew-hw-control
