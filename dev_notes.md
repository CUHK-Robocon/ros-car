ros2 on pi needs ubuntu or docker

flash ubuntu 24 into pi w rasp pi imager

"sudo rpi-eeprom-config --edit" and add "PSU_MAX_CURRENT=5000", then sudo reboot to improve the performance

sudo apt-get install openssh-server openssh-client sin can ssh, but cannot vnc yet

install no machine on both the pi and ur laptop

for the pi: NoMachine for ARM TAR.GZ (aarch64) in https://download.nomachine.com/download/?id=30&platform=linux&distro=arm

both pi and ur laptop in the same lan, but that lan cannot be cuhk1x, can open a hotspot urself to work

Note that you need to connect a monitor with the pi for nomachine to work, otherwise it will say sth like “Cannot detect any display running, Do you want NoMachine to create a new display?”

If you have connected a monitor with the pi but that message still pops up, restart the pi

Could not resolve host: [github.com](http://github.com/) 

change dns from auto to manual on the sbc, type 1.1.1.1 or 8.8.8.8

```rosdep install -i --from-path src --rosdistro kilted -y``` before colcon build to install dependencies

commenting out logging stuff and reducing the queue size for publisher & subscriber can greatly improve performance