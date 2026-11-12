# NICOLA for TC52 = the current TS52K NICOLA (qmk_userspace @b7d0ccf6):
#   keymap.c                    <- keyboards/ts52k/keymaps/nicola/keymap.c   (unchanged)
#   nicola.[ch], nicola_table.[ch] <- users/nicola/                          (unchanged)
# Copied into the keymap so that the build never depends on which userspace (if any) is
# overlaid; USER_NAME points at a name that does not exist so no users/<name> is pulled in.
# build.sh checks that these copies still match qmk_userspace and warns if not.
USER_NAME := tc52_bundled_nicola
SRC += nicola.c \
       nicola_table.c

# from users/nicola/rules.mk: timeouts use defer_exec()
DEFERRED_EXEC_ENABLE = yes
