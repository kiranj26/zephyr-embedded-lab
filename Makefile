# Zephyr S3 Lab — one front door for everything. Run `make help`.
# Variables: APP, BOARD, BOARD_ROOT, PORT, BAUD, SECS, NAME, N
APP ?=
SECS ?= 8
export BOARD BOARD_ROOT PORT BAUD PLATFORM

.PHONY: help setup bootstrap image up down shell ports list build flash monitor capture run menuconfig clean new-app notes doctor openocd pins

help:
	@echo "Setup:    make setup | make bootstrap | make image | make up | make doctor"
	@echo "Daily:    make run APP=01        (build + flash + monitor)"
	@echo "          make build APP=04      make flash APP=04     make monitor"
	@echo "          make capture SECS=10   (reset board, print serial output, exit)"
	@echo "Course:   make run APP=course/01_blink   (build the reference code read-only)"
	@echo "Tools:    make list | make ports | make menuconfig APP=03 | make shell | make clean APP=03"
	@echo "Create:   make new-app NAME=13_my_idea | make notes N=04"
	@echo "Debug:    make openocd (host) then GDB/VS Code in the container"
	@echo "Safety:   make pins FILE=workspace/apps/04_button_led/boards/esp32s3_devkitc.overlay"

setup:
	@scripts/setup-mac.sh
bootstrap:
	@scripts/bootstrap.sh
image:
	@scripts/image.sh
up:
	@scripts/up.sh
down:
	@docker stop zephyr-lab >/dev/null 2>&1 && echo "stopped" || echo "not running"
shell:
	@docker exec -it zephyr-lab bash
ports:
	@scripts/ports.sh
list:
	@scripts/list.sh
build:
	@scripts/build.sh "$(APP)"
flash:
	@scripts/flash.sh "$(APP)"
monitor:
	@scripts/monitor.sh
capture:
	@scripts/capture.sh $(SECS)
run: build flash monitor
menuconfig:
	@scripts/build.sh "$(APP)" --menuconfig
clean:
	@scripts/clean.sh "$(APP)"
new-app:
	@scripts/new-app.sh "$(NAME)"
notes:
	@scripts/new-notes.sh "$(N)"
doctor:
	@scripts/doctor.sh
openocd:
	@scripts/openocd.sh
pins:
	@python3 scripts/check_pins.py $(FILE)
