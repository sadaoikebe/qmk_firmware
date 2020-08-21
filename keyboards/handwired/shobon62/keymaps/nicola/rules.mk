COMBO_ENABLE = no

# If you want to change the display of OLED, you need to change here
SRC +=  nicola.c \
		jtu.c \
        key_duration.c

# 16-bit timer (TIMER1) を使用したタイムアウト
# 一定時間経過すると、key down 状態のままでもキーが出力される
OPT_DEFS = -DTIMEOUT_INTERRUPT
