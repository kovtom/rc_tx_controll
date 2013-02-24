#
# Defines the part type that this project uses.
#
PART=LM4F120H5QR

#
# Set the processor variant.
#
VARIANT=cm4f

#
# The base directory for StellarisWare.
#
ROOT=/home/tomi/src/ti/

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find source files that do not live in this directory.
#
VPATH=home/tomi/src/ti/boards/ek-lm4f120xl/drivers/
VPATH+=/home/tomi/src/ti/utils/

#
# Where to find header files that do not live in the source directory.
#
IPATH=..
IPATH+=/home/tomi/src/ti/

#
# The default rule, which causes the Project Zero Example to be built.
#
all: ${COMPILER}
all: ${COMPILER}/rc_tx_controll.axf

#
# The rule to flash the program to the chip
#
flash:
	lm4flash gcc/rc_tx_controll.bin

#
# The rule to rebuild the project and debug it with Nemiver.
#

debug: clean
debug: CFLAGS+=-g -D DEBUG
debug: ${COMPILER}
debug: ${COMPILER}/rc_tx_controll.axf
debug:
	./debug_nemiver.sh



#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir -p ${COMPILER}

#
# Rules for building the Project Zero Example.
#
${COMPILER}/rc_tx_controll.axf: ${COMPILER}/rc_tx_controll.o
${COMPILER}/rc_tx_controll.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/rc_tx_controll.axf: ${ROOT}/driverlib/${COMPILER}-cm4f/libdriver-cm4f.a
${COMPILER}/rc_tx_controll.axf: rc_tx_controll.ld
SCATTERgcc_rc_tx_controll=rc_tx_controll.ld
ENTRY_rc_tx_controll=ResetISR
CFLAGSgcc=-DTARGET_IS_BLIZZARD_RA2

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
