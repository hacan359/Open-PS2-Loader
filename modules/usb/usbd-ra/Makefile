# Our usbd_mini.irx, replacing the one shipped inside ps2sdk.
#
# ps2sdk rewrote its USB host driver in September 2024 ("USBD feature
# update"), and with that driver a CD-era game run off a USB stick loses
# its controller and its sound at its first IOP reboot. The source here is
# ps2sdk iop/usb/usbd as of 314d87e7 (01.04.2024), untouched; built with
# today's toolchain it plays. Only this Makefile is ours: exports.o first
# for srxfixup, and no gp switching, as in today's module.
IOP_BIN  = usbd_mini.irx
IOP_OBJS = exports.o hcd.o hub.o interface.o mem.o usbd.o usbio.o driver.o  imports.o
IOP_PREFER_GPOPT = 16384
IOP_CFLAGS += -DMINI_DRIVER -DUSE_GP_REGISTER=0
IOP_INCS := -I./src -I./include -I$(PS2SDK)/common/include -I$(PS2SDK)/iop/include
include $(PS2SDK)/Defs.make
include ../../Rules.bin.make
# after Rules.bin.make, which sets it to ./: the sources keep ps2sdk's layout
IOP_SRC_DIR = ./src/
include $(PS2SDK)/samples/Makefile.iopglobal
