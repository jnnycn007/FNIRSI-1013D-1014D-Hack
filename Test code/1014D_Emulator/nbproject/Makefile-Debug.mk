#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/armv5tl.o \
	${OBJECTDIR}/armv5tl_thumb.o \
	${OBJECTDIR}/buttons.o \
	${OBJECTDIR}/f1c100s.o \
	${OBJECTDIR}/f1c100s_ccu.o \
	${OBJECTDIR}/f1c100s_debe.o \
	${OBJECTDIR}/f1c100s_dramc.o \
	${OBJECTDIR}/f1c100s_intc.o \
	${OBJECTDIR}/f1c100s_pio.o \
	${OBJECTDIR}/f1c100s_spi.o \
	${OBJECTDIR}/f1c100s_tcon.o \
	${OBJECTDIR}/f1c100s_timer.o \
	${OBJECTDIR}/f1c100s_uart.o \
	${OBJECTDIR}/mousehandling.o \
	${OBJECTDIR}/oscilloscope_emulator.o \
	${OBJECTDIR}/rotary_dial.o \
	${OBJECTDIR}/user_interface_functions.o \
	${OBJECTDIR}/xlibfunctions.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/lib/x86_64-linux-gnu -lm

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/oscilloscope_emulator

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/oscilloscope_emulator: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/oscilloscope_emulator ${OBJECTFILES} ${LDLIBSOPTIONS} -lX11 -lXft -lXrandr -lpthread

${OBJECTDIR}/armv5tl.o: armv5tl.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I/usr/include/freetype2 -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/armv5tl.o armv5tl.c

${OBJECTDIR}/armv5tl_thumb.o: armv5tl_thumb.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I/usr/include/freetype2 -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/armv5tl_thumb.o armv5tl_thumb.c

${OBJECTDIR}/buttons.o: buttons.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I/usr/include/freetype2 -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/buttons.o buttons.c

${OBJECTDIR}/f1c100s.o: f1c100s.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I/usr/include/freetype2 -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/f1c100s.o f1c100s.c

${OBJECTDIR}/f1c100s_ccu.o: f1c100s_ccu.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I/usr/include/freetype2 -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/f1c100s_ccu.o f1c100s_ccu.c

${OBJECTDIR}/f1c100s_debe.o: f1c100s_debe.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I/usr/include/freetype2 -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/f1c100s_debe.o f1c100s_debe.c

${OBJECTDIR}/f1c100s_dramc.o: f1c100s_dramc.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I/usr/include/freetype2 -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/f1c100s_dramc.o f1c100s_dramc.c

${OBJECTDIR}/f1c100s_intc.o: f1c100s_intc.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I/usr/include/freetype2 -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/f1c100s_intc.o f1c100s_intc.c

${OBJECTDIR}/f1c100s_pio.o: f1c100s_pio.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I/usr/include/freetype2 -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/f1c100s_pio.o f1c100s_pio.c

${OBJECTDIR}/f1c100s_spi.o: f1c100s_spi.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I/usr/include/freetype2 -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/f1c100s_spi.o f1c100s_spi.c

${OBJECTDIR}/f1c100s_tcon.o: f1c100s_tcon.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I/usr/include/freetype2 -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/f1c100s_tcon.o f1c100s_tcon.c

${OBJECTDIR}/f1c100s_timer.o: f1c100s_timer.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I/usr/include/freetype2 -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/f1c100s_timer.o f1c100s_timer.c

${OBJECTDIR}/f1c100s_uart.o: f1c100s_uart.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I/usr/include/freetype2 -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/f1c100s_uart.o f1c100s_uart.c

${OBJECTDIR}/mousehandling.o: mousehandling.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I/usr/include/freetype2 -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mousehandling.o mousehandling.c

${OBJECTDIR}/oscilloscope_emulator.o: oscilloscope_emulator.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I/usr/include/freetype2 -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/oscilloscope_emulator.o oscilloscope_emulator.c

${OBJECTDIR}/rotary_dial.o: rotary_dial.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I/usr/include/freetype2 -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/rotary_dial.o rotary_dial.c

${OBJECTDIR}/user_interface_functions.o: user_interface_functions.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I/usr/include/freetype2 -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/user_interface_functions.o user_interface_functions.c

${OBJECTDIR}/xlibfunctions.o: xlibfunctions.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I/usr/include/freetype2 -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/xlibfunctions.o xlibfunctions.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
