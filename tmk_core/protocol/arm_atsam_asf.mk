ARM_ATSAM_DIR = $(TMK_DIR)/$(PROTOCOL_DIR)/arm_atsam_asf

SRC += $(ARM_ATSAM_DIR)/main.c
SRC += $(ARM_ATSAM_DIR)/usb.c
SRC += $(ARM_ATSAM_DIR)/uart.c

#
# COMMON drivers
#
SRC += $(SDK)/common/utils/interrupt/interrupt_sam_nvic.c
SRC += $(SDK)/sam0/drivers/system/system.c
SRC += $(SDK)/sam0/utils/cmsis/$(SAMD)/source/gcc/startup_$(SAMD).c
SRC += $(SDK)/sam0/utils/cmsis/$(SAMD)/source/system_$(SAMD).c
SRC += $(SDK)/sam0/utils/syscalls/gcc/syscalls.c
SRC += $(SDK)/sam0/drivers/system/clock/clock_$(SAMD_CLOCK)/clock.c
SRC += $(SDK)/sam0/drivers/system/clock/clock_$(SAMD_CLOCK)/gclk.c
SRC += $(SDK)/sam0/drivers/system/pinmux/pinmux.c
SRC += $(SDK)/sam0/drivers/port/port.c
SRC += $(SDK)/sam0/drivers/sercom/sercom.c
SRC += $(SDK)/sam0/drivers/sercom/sercom_interrupt.c
SRC += $(SDK)/sam0/drivers/sercom/usart/usart.c
SRC += $(SDK)/sam0/drivers/wdt/wdt.c

INCS += keyboards/$(KEYBOARD)/configs
INCS += $(ARM_ATSAM_DIR)/configs
INCS += $(TMK_DIR)/$(PROTOCOL_DIR)
INCS += $(SDK)/
INCS += $(SDK)/common/utils
INCS += $(SDK)/common2/services
INCS += $(SDK)/common2/services/delay
INCS += $(SDK)/sam0/drivers
INCS += $(SDK)/sam0/drivers/port
INCS += $(SDK)/sam0/drivers/system
INCS += $(SDK)/sam0/drivers/system/clock
INCS += $(SDK)/sam0/drivers/system/clock/clock_$(SAMD_CLOCK)
INCS += $(SDK)/sam0/drivers/sercom
INCS += $(SDK)/sam0/drivers/system/interrupt
INCS += $(SDK)/sam0/drivers/system/interrupt/system_interrupt_$(SAMD)
INCS += $(SDK)/sam0/drivers/system/pinmux
INCS += $(SDK)/sam0/drivers/system/power/power_sam_d_r_h
INCS += $(SDK)/sam0/drivers/system/reset/reset_sam_d_r_h
INCS += $(SDK)/sam0/utils
INCS += $(SDK)/sam0/utils/cmsis/$(SAMD)/include
INCS += $(SDK)/sam0/utils/cmsis/$(SAMD)/source
INCS += $(SDK)/sam0/utils/header_files
INCS += $(SDK)/sam0/utils/preprocessor
INCS += $(SDK)/thirdparty/CMSIS/Include

#
# DELAY driver
#
SRC += $(SDK)/common2/services/delay/sam0/cycle_counter.c
OPT_DEFS += -DCYCLE_MODE

#
# USB driver
#
SRC += $(SDK)/common/services/sleepmgr/samd/sleepmgr.c
SRC += $(ARM_ATSAM_DIR)/usb/udi_hid_kbd.c
SRC += $(SDK)/common/services/usb/class/hid/device/kbd/udi_hid_kbd_desc.c
SRC += $(SDK)/common/services/usb/class/hid/device/udi_hid.c
SRC += $(SDK)/common/services/usb/udc/udc.c
SRC += $(SDK)/sam0/drivers/usb/usb_sam_d_r/usb.c
SRC += $(SDK)/sam0/drivers/usb/stack_interface/usb_device_udd.c
SRC += $(SDK)/sam0/drivers/usb/stack_interface/usb_dual.c

INCS += $(SDK)/common/boards
INCS += $(SDK)/common/boards/user_board
INCS += $(SDK)/common/services/sleepmgr
INCS += $(SDK)/common/services/usb
INCS += $(SDK)/common/services/usb/udc
INCS += $(SDK)/common/services/usb/class/hid
INCS += $(SDK)/common/services/usb/class/hid/device
INCS += $(SDK)/common/services/sleepmgr/samd
INCS += $(SDK)/sam0/drivers/extint
INCS += $(SDK)/sam0/drivers/usb

OPT_DEFS += -DBOARD=USER_BOARD
