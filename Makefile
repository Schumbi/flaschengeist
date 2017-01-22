# Uncomment lines below if you have problems with $PATH
SHELL := /bin/bash
PATH := /usr/bin:$(PATH)

all: run

upload: tags
	platformio -f -c vim run --target upload

clean:
	platformio -f -c vim run --target clean
	rm tags

program: tags
	platformio -f -c vim run --target program

uploadfs: tags
	platformio -f -c vim run --target uploadfs

update: tags
	platformio -f -c vim update

run: tags
	platformio -f -c vim run

tags:
	ctags -R src/* ~/.platformio/packages/framework-arduinoespressif8266/cores/esp8266/* .piolibdeps/**
	
