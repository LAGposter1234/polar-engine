#!/bin/bash

if [ "$EUID" -ne 0 ]; then
	echo "Please run as root"
	exit 1
fi

cp bin/polar /usr/bin/polar
chmod +x /usr/bin/polar
echo "Polar installed to /usr/bin/polar"
