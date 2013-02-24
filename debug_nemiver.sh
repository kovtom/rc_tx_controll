#!/bin/bash

# location of OpenOCD Board .cfg files
OPENOCD_BOARD_DIR=/usr/share/openocd/scripts/board

# start xterm with openocd in the background
xterm -e openocd -f $OPENOCD_BOARD_DIR/ek-lm4f120xl.cfg &

# save the PID of the background process
XTERM_PID=$!

# wait a bit to be sure the hardware is ready
sleep 2

# execute some initialisation commands via gdb
arm-none-eabi-gdb --batch --command=init.gdb gcc/rc_tx_controll.axf

# start the gdb gui
nemiver --remote=localhost:3333 --gdb-binary="$(which arm-none-eabi-gdb)" gcc/rc_tx_controll.axf

# close xterm when the user has exited nemiver
kill $XTERM_PID
