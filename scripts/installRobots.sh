#!/bin/sh

sudo rfcomm release all
sleep 1

if [ "$(hostname)" = "fishtrack" ]; then
	sudo rfcomm -i 00:1A:7D:DA:71:05 bind 0 10:00:E8:6C:F0:54 # A (PARIS)
	sudo rfcomm -i 5C:F3:70:77:1A:80 bind 1 10:00:E8:6C:F0:69 # B (EPFL)
	sudo rfcomm -i 5C:F3:70:77:1A:85 bind 2 10:00:E8:6C:F0:5B # C (EPFL)
	sudo rfcomm -i 5C:F3:70:77:1A:85 bind 3 10:00:E8:6C:F0:5E # D (PARIS)
	sudo rfcomm -i 00:1A:7D:DA:71:0A bind 4 10:00:E8:6C:F0:59 # E (EPFL)
	sudo rfcomm -i 5C:F3:70:77:1A:85 bind 5 10:00:E8:6C:F0:5A # F (PARIS)
	sudo rfcomm -i 5C:F3:70:77:1A:85 bind 6 10:00:E8:6C:F0:4E # G (PARIS)
	sudo rfcomm -i 5C:F3:70:77:1A:80 bind 7 10:00:E8:6C:F0:49 # H (PARIS)
	sudo rfcomm -i 5C:F3:70:77:1A:80 bind 9 10:00:E8:6C:F0:45 # J (PARIS)

elif [ "$(hostname)" = "fishtrack2" ]; then
	sudo rfcomm -i 00:02:72:AF:00:35 bind 0 10:00:E8:6C:F0:54 # A (PARIS)
	sudo rfcomm -i 00:02:72:AF:00:35 bind 1 10:00:E8:6C:F0:69 # B (EPFL)
	sudo rfcomm -i 00:02:72:CB:AA:F0 bind 2 10:00:E8:6C:F0:5B # C (EPFL)
	sudo rfcomm -i 00:02:72:AF:00:35 bind 3 10:00:E8:6C:F0:5E # D (PARIS)
	sudo rfcomm -i 5C:F3:70:75:00:BE bind 4 10:00:E8:6C:F0:59 # E (EPFL)
	sudo rfcomm -i 5C:F3:70:75:00:BE bind 5 10:00:E8:6C:F0:5A # F (PARIS)
	sudo rfcomm -i 00:02:72:AF:00:35 bind 6 10:00:E8:6C:F0:4E # G (PARIS)
	sudo rfcomm -i 00:02:72:AF:00:35 bind 7 10:00:E8:6C:F0:49 # H (PARIS)
	sudo rfcomm -i 00:02:72:AF:00:35 bind 9 10:00:E8:6C:F0:45 # J (PARIS)

else
	exit
fi


