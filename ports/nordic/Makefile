# This file is part of the CircuitPython project: https://circuitpython.org
#
# SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
#
# SPDX-License-Identifier: MIT

include ../../py/circuitpy_mkenv.mk

ifneq ($(SD), )
	include bluetooth/bluetooth_common.mk
endif

CROSS_COMPILE = arm-none-eabi-

FATFS_DIR = lib/oofatfs

INC += -I.
INC += -I../..
INC += -I$(BUILD)
INC += -I$(BUILD)/genhdr
INC += -I./../../lib/cmsis/inc
INC += -I./boards/$(BOARD)
INC += -isystem ./nrfx
INC += -isystem ./nrfx/hal
INC += -isystem ./nrfx/mdk
INC += -isystem ./nrfx/drivers/include
INC += -isystem ./nrfx/drivers/src
INC += -I./bluetooth
INC += -I./peripherals
INC += -I../../lib/mp-readline
INC += -I../../lib/tinyusb/src
INC += -I../../supervisor/shared/usb

#Debugging/Optimization
ifeq ($(DEBUG), 1)
  CFLAGS += -ggdb3
  OPTIMIZATION_FLAGS = -Og
else
  OPTIMIZATION_FLAGS ?= -O2 -fno-inline-functions
  CFLAGS += -DNDEBUG -ggdb3
endif

ifeq ($(NRF_DEBUG_PRINT), 1)
  CFLAGS += -DNRF_DEBUG_PRINT=1
endif

# option to override compiler optimization level, set in boards/$(BOARD)/mpconfigboard.mk
CFLAGS += $(OPTIMIZATION_FLAGS)

CFLAGS += $(INC) -Wall -Werror -std=gnu11 -nostdlib -fshort-enums $(BASE_CFLAGS) $(CFLAGS_MOD) $(COPT) -Werror=missing-prototypes

# Nordic Softdevice SDK header files contains inline assembler that has
# broken constraints. As a result the IPA-modref pass, introduced in gcc-11,
# is able to "prove" that arguments to wrapper functions generated with
# the SVCALL() macro are unused and, as a result, the optimizer will remove
# code within the callers that sets up these arguments (which results in
# a broken bootloader). The broken headers come from Nordic-supplied zip
# files and are not trivial to patch so, for now, we'll simply disable the
# new gcc-11 inter-procedural optimizations.
ifeq (,$(findstring unrecognized,$(shell $(CC) $(CFLAGS) -fno-ipa-modref 2>&1)))
CFLAGS += -fno-ipa-modref
endif

# Undo some warnings.
# nrfx does casts that increase alignment requirements.
CFLAGS += -Wno-cast-align

NRF_DEFINES += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += $(NRF_DEFINES)

CFLAGS += \
	-mthumb \
	-mabi=aapcs-linux \
	-mfloat-abi=hard \
	-mcpu=cortex-m4 \
	-mfpu=fpv4-sp-d16

# TODO: check this
CFLAGS += -D__START=main

LDFLAGS = $(CFLAGS) -nostartfiles -Wl,-nostdlib -Wl,-z,max-page-size=0x1000 -Wl,-T,$(GENERATED_LD_FILE) -Wl,-Map=$@.map -Wl,-cref -Wl,-gc-sections -specs=nano.specs
LIBS := -lgcc -lc

LDFLAGS += -mthumb -mcpu=cortex-m4

# Use toolchain libm if we're not using our own.
ifndef INTERNAL_LIBM
LIBS += -lm
endif

# TinyUSB defines
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_NRF5X -DCFG_TUD_CDC_RX_BUFSIZE=1024 -DCFG_TUD_CDC_TX_BUFSIZE=1024 -DCFG_TUD_MSC_BUFSIZE=4096 -DCFG_TUD_MIDI_RX_BUFSIZE=128 -DCFG_TUD_MIDI_TX_BUFSIZE=128

SRC_NRFX = $(addprefix nrfx/,\
	drivers/src/nrfx_power.c \
	drivers/src/nrfx_spim.c \
	drivers/src/nrfx_timer.c \
	drivers/src/nrfx_twim.c \
	drivers/src/nrfx_uarte.c \
	drivers/src/nrfx_gpiote.c \
	drivers/src/nrfx_rtc.c \
	drivers/src/nrfx_nvmc.c \
	drivers/src/nrfx_wdt.c \
	)

ifdef EXTERNAL_FLASH_DEVICES
	ifeq ($(QSPI_FLASH_FILESYSTEM),1)
		SRC_NRFX += nrfx/drivers/src/nrfx_qspi.c
	endif
endif


SRC_C += \
	background.c \
	boards/$(BOARD)/board.c \
	boards/$(BOARD)/pins.c \
	device/$(MCU_VARIANT)/startup_$(MCU_SUB_VARIANT).c \
	bluetooth/ble_drv.c \
	common-hal/_bleio/bonding.c \
	nrfx/mdk/system_$(MCU_SUB_VARIANT).c \
	sd_mutex.c \

SRC_PERIPHERALS := \
	peripherals/nrf/cache.c \
	peripherals/nrf/clocks.c \
	peripherals/nrf/$(MCU_CHIP)/pins.c \
	peripherals/nrf/$(MCU_CHIP)/power.c \
        peripherals/nrf/nvm.c \
	peripherals/nrf/timers.c \

$(patsubst %.c,$(BUILD)/%.o,$(SRC_PERIPHERALS)): CFLAGS += -Wno-missing-prototypes

SRC_C += $(SRC_PERIPHERALS)
ifneq ($(CIRCUITPY_USB_DEVICE),0)
# USB source files for nrf52840
ifeq ($(MCU_SUB_VARIANT),nrf52840)
SRC_DCD = \
	lib/tinyusb/src/portable/nordic/nrf5x/dcd_nrf5x.c
endif

ifeq ($(MCU_SUB_VARIANT),nrf52833)
SRC_DCD += \
	lib/tinyusb/src/portable/nordic/nrf5x/dcd_nrf5x.c
endif
SRC_C += $(SRC_DCD)
$(patsubst %.c,$(BUILD)/%.o,$(SRC_DCD)): CFLAGS += -Wno-missing-prototypes
endif # CIRCUITPY_USB_DEVICE

SRC_S = supervisor/cpu.s

OBJ += $(PY_O) $(SUPERVISOR_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_NRFX:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMMON_HAL_SHARED_MODULE_EXPANDED:.c=.o))
ifeq ($(INTERNAL_LIBM),1)
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM:.c=.o))
endif
OBJ += $(addprefix $(BUILD)/, $(SRC_CIRCUITPY_COMMON:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))

# nrfx uses undefined preprocessor variables quite casually, so we can't do
# warning checks for these.  Happily, we've confined the offenders to the NRFX
# source files themselves.
$(addprefix $(BUILD)/, $(SRC_NRFX:.c=.o)): CFLAGS += -Wno-undef

$(BUILD)/$(FATFS_DIR)/ff.o: COPT += -Os
$(filter $(PY_BUILD)/../extmod/vfs_fat_%.o, $(PY_O)): COPT += -Os

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_MOD) $(SRC_COMMON_HAL_SHARED_MODULE_EXPANDED) $(SRC_CIRCUITPY_COMMON)
# Sources that only hold QSTRs after pre-processing.
SRC_QSTR_PREPROCESSOR +=

UF2_FAMILY_ID_nrf52840 = 0xADA52840
UF2_FAMILY_ID_nrf52833 = 0x621E937A


all: $(BUILD)/firmware.bin $(BUILD)/firmware.uf2 $(BUILD)/firmware.combined.hex

ifeq ($(VALID_BOARD),)
$(BUILD)/firmware.elf: invalid-board
else
$(BUILD)/firmware.elf: $(OBJ) $(GENERATED_LD_FILE)
	$(STEPECHO) "LINK $@"
	$(Q)echo $(OBJ) > $(BUILD)/firmware.objs
	$(Q)$(CC) -o $@ $(LDFLAGS) @$(BUILD)/firmware.objs -Wl,--print-memory-usage -Wl,--start-group $(LIBS) -Wl,--end-group
	$(Q)$(SIZE) $@ | $(PYTHON) $(TOP)/tools/build_memory_info.py $(GENERATED_LD_FILE) $(BUILD)
endif

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(STEPECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary $^ $@
#	$(Q)$(OBJCOPY) -O binary -j .vectors -j .text -j .data $^ $@

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(STEPECHO) "Create $@"
	$(Q)$(OBJCOPY) -O ihex $^ $@
#	$(Q)$(OBJCOPY) -O ihex -j .vectors -j .text -j .data $^ $@

$(BUILD)/firmware.combined.hex: $(BUILD)/firmware.hex $(SOFTDEV_HEX)
	$(STEPECHO) "Create $@"
	$(Q)hexmerge.py -o $@ $^

$(BUILD)/firmware.uf2: $(BUILD)/firmware.hex
	$(ECHO) "Create $@"
	$(PYTHON) $(TOP)/tools/uf2/utils/uf2conv.py -f $(UF2_FAMILY_ID_$(MCU_CHIP)) -c -o "$(BUILD)/firmware.uf2" $^



#####################
# Flash with debugger
#####################
FLASHER ?=

ifeq ($(FLASHER),)

# Also update to bootloader setting to validate application and skip checksum ( app valid = 0x0001, crc = 0x0000 )
flash: $(BUILD)/firmware.hex
	nrfjprog --program $< --sectorerase -f $(MCU_VARIANT)
	nrfjprog --erasepage $(BOOT_SETTING_ADDR) -f $(MCU_VARIANT)
	nrfjprog --memwr $(BOOT_SETTING_ADDR) --val 0x00000001 -f $(MCU_VARIANT)
	nrfjprog --reset -f $(MCU_VARIANT)

sd: $(BUILD)/firmware.hex
	nrfjprog --eraseall -f $(MCU_VARIANT)
	nrfjprog --program $(SOFTDEV_HEX) -f $(MCU_VARIANT)
	nrfjprog --program $< --sectorerase -f $(MCU_VARIANT)
	nrfjprog --reset -f $(MCU_VARIANT)

else ifeq ($(FLASHER), pyocd)

flash: $(BUILD)/firmware.hex
	pyocd-flashtool -t $(MCU_VARIANT) $< --sector_erase
#	pyocd-tool -t $(MCU_VARIANT) erase $(BOOT_SETTING_ADDR)
	pyocd-tool -t $(MCU_VARIANT) write32 $(BOOT_SETTING_ADDR) 0x00000001
	pyocd-tool -t $(MCU_VARIANT) reset

sd: $(BUILD)/firmware.hex
	pyocd-flashtool -t $(MCU_VARIANT) --chip_erase
	pyocd-flashtool -t $(MCU_VARIANT) $(SOFTDEV_HEX)
	pyocd-flashtool -t $(MCU_VARIANT) $< --sector_erase
	pyocd-tool -t $(MCU_VARIANT) reset $(BOOT_SETTING_ADDR)

endif

#####################
# Flash with DFU
#####################
.phony: dfu-gen dfu-flash

NRFUTIL = nrfutil
ADAFRUIT_NRFUTIL = adafruit-nrfutil

ifeq ($(MCU_SUB_VARIANT),nrf52840)
	DFU_TOUCH = --touch 1200
else
	DFU_TOUCH =
endif

check_defined = \
    $(strip $(foreach 1,$1, \
    $(call __check_defined,$1,$(strip $(value 2)))))
__check_defined = \
    $(if $(value $1),, \
    $(error Undefined make flag: $1$(if $2, ($2))))

## Flash with DFU serial
dfu-flash: $(BUILD)/dfu-package.zip
	@:$(call check_defined, SERIAL, example: SERIAL=/dev/ttyUSB0)
	$(ADAFRUIT_NRFUTIL) --verbose dfu serial --package $^ -p $(SERIAL) -b 115200 --singlebank $(DFU_TOUCH)

## Create DFU package file
dfu-gen: $(BUILD)/dfu-package.zip

$(BUILD)/dfu-package.zip: $(BUILD)/firmware.hex
	$(ADAFRUIT_NRFUTIL) dfu genpkg --sd-req 0xFFFE --dev-type 0x0052 --application $^ $(BUILD)/dfu-package.zip

# Espruino DFU
$(BUILD)/firmware.espruino.zip: $(BUILD)/firmware.hex
	$(Q)$(NRFUTIL) pkg generate $(BUILD)/firmware.espruino.zip --application $^ --application-version 0xff --hw-version 52 --sd-req 0xa9,0xae,0xb6 --key-file espruino_dfu_private_key.pem

espruino-dfu-gen: $(BUILD)/firmware.espruino.zip

include $(TOP)/py/mkrules.mk
