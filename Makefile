TARGET :=
INCLUDES := ../include ../../../common/include/mm/
SUBPROJECTS := arch/arch/source common/source/kernel utils/mtools
SHARED_LIBS := arch/arch/source/libArchSpecific.a common/source/kernel/libKernel.a
PROJECT_ROOT := .

include ./make-support/common.mk

all: kernel install

#make kernel doesn't work yet, because there is no rule kernel in common.mk
#use just "make" instead
kernel: $(SUBPROJECTS)
ifeq ($(V),1)
	@echo "$(KERNELLDCOMMAND) $(SHARED_LIBS) -u entry -T arch/arch/utils/kernel-ld-script.ld -o $(OBJECTDIR)/kernel.x -Map $(OBJECTDIR)/kernel.map"
else
	@echo "LD $(OBJECTDIR)/kernel.x"
endif
	@mkdir -p $(OBJECTDIR)
	@$(KERNELLDCOMMAND) $(SHARED_LIBS) -u entry -T arch/arch/utils/kernel-ld-script.ld -o $(OBJECTDIR)/kernel.x -Map $(OBJECTDIR)/kernel.map

#make install doesn't work yet, because there is no rule install in common.mk
#use just "make" instead
install: kernel
	@echo "Starting with install"
	test -e ./images/boot.img && cp ./images/boot.img $(OBJECTDIR)/boot.img
	test -e $(OBJECTDIR)/boot.img || (echo ERROR boot.img nowhere found; exit 1) 
	MTOOLS_SKIP_CHECK=1 $(OBJECTDIR)/utils/mtools/mtools -c mcopy -i $(OBJECTDIR)/boot.img $(OBJECTDIR)/kernel.x ::/boot/
	@echo INSTALL: $(OBJECTDIR)/boot.img is ready

