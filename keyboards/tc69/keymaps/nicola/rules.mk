# Legacy NICOLA (TC69 era, sadaoikebe/qmk_firmware capacitive @b3ac8db961 users/nicola).
# Kept inside the keymap so that it can never be confused with the current
# qmk_userspace users/nicola (different API and behaviour).
# Original users/nicola/rules.mk: SRC += nicola.c jtu.c ; BACKLIGHT_ENABLE = no
#   jtu.c is not built: its only entry point process_jtu() is commented out in keymap.c.
USER_NAME := tc69_legacy_nicola
SRC += nicola.c
