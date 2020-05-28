TFTPDOWNLOADADDR:=0x21000000
TFTPDOWNLOADADDR_PART_PNI:=0x21800000
KERNELBOOTADDR:=0x22000000
RTOSBOOTADDR:=0x27C08000

TARGET_SCRIPT:=$(foreach n,$(IMAGE_LIST),$(n)_$(FLASH_TYPE)_$($(n)$(FSTYPE))_script) $(PAT_TABLE)_$(FLASH_TYPE)_partition_script $(PAT_TABLE)_$(FLASH_TYPE)_config_script
TARGET_FS:=$(filter-out $(patsubst %_fs__,%,$(filter %_fs__, $(foreach n,$(IMAGE_LIST),$(n)_fs_$($(n)$(FSTYPE))_))), $(IMAGE_LIST))
TARGET_UBIFS := $(patsubst %_fs_ubifs_, %, $(filter %_fs_ubifs_, $(foreach n,$(TARGET_FS),$(n)_fs_$($(n)$(FSTYPE))_)))
TARGET_SQUAFS := $(patsubst %_fs_squashfs_, %,$(filter %_fs_squashfs_, $(foreach n,$(TARGET_FS),$(n)_fs_$($(n)$(FSTYPE))_)))
TARGET_JIFFS2 := $(patsubst %_fs_jffs2_, %, $(filter %_fs_jffs2_, $(foreach n,$(TARGET_FS),$(n)_fs_$($(n)$(FSTYPE))_)))
TARGET_NONEFS := $(filter-out $(TARGET_FS), $(filter-out $(patsubst %_fs__sz__, %, $(filter %_fs__sz__, $(foreach n,$(IMAGE_LIST),$(n)_fs_$($(n)$(FSTYPE))_sz_$($(n)$(PATSIZE))_))), $(IMAGE_LIST)))
TARGET_IMAGE_LIST := $(filter-out cis, $(IMAGE_LIST))
SCRIPTDIR:=$(IMAGEDIR)/scripts

scripts:
	mkdir -p $(SCRIPTDIR) 
	$(MAKE) set_partition
	$(MAKE) $(TARGET_SCRIPT)
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(IMAGEDIR)/auto_update.txt
	if [ "$(filter cis, $(IMAGE_LIST))" != "" ]; then	\
		echo estar scripts/[[cis.es >> $(IMAGEDIR)/auto_update.txt;	\
	fi;
	@echo estar scripts/[[set_partition.es >> $(IMAGEDIR)/auto_update.txt
	@echo -e $(foreach n,$(TARGET_IMAGE_LIST),estar scripts/[[$(n)\.es\\n) >> $(IMAGEDIR)/auto_update.txt
	@echo estar scripts/set_config >> $(IMAGEDIR)/auto_update.txt
	@echo saveenv >> $(IMAGEDIR)/auto_update.txt
	@echo printenv >> $(IMAGEDIR)/auto_update.txt
	@echo reset >> $(IMAGEDIR)/auto_update.txt
	@echo "% <- this is end of file symbol" >> $(IMAGEDIR)/auto_update.txt

set_partition:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[set_partition.es
ifneq ($(MTDPARTS), )
	@echo setenv mtdparts \' $(MTDPARTS) >> $(SCRIPTDIR)/[[set_partition.es
else
	@echo mtdparts del CIS >> $(SCRIPTDIR)/[[set_partition.es
	@echo setenv mtdparts "\$$(mtdparts),$(cis$(SYSTAB))" >> $(SCRIPTDIR)/[[set_partition.es
endif
	@echo saveenv >> $(SCRIPTDIR)/[[set_partition.es
	@echo nand erase.part UBI >> $(SCRIPTDIR)/[[set_partition.es
	@echo ubi part UBI >> $(SCRIPTDIR)/[[set_partition.es
	@echo -e $(foreach n,$(TARGET_UBIFS),ubi create $(n) $($(n)$(PATSIZE))\\n) >> $(SCRIPTDIR)/[[set_partition.es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[set_partition.es

cis_$(FLASH_TYPE)__script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[cis.es
	@echo tftp $(TFTPDOWNLOADADDR) boot/SPINANDINFO.sni >> $(SCRIPTDIR)/[[cis.es
	@echo tftp $(TFTPDOWNLOADADDR_PART_PNI) boot/PARTINFO.pni >> $(SCRIPTDIR)/[[cis.es
	@echo writecis $(TFTPDOWNLOADADDR) $(TFTPDOWNLOADADDR_PART_PNI) $(CIS_PBAs) $(CIS_COPIES) >> $(SCRIPTDIR)/[[cis.es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[cis.es

ipl_$(FLASH_TYPE)__script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[ipl.es
	@echo tftp $(TFTPDOWNLOADADDR) ipl_s.bin >> $(SCRIPTDIR)/[[ipl.es
	@echo nand erase.part IPL0 >> $(SCRIPTDIR)/[[ipl.es
	@echo nand write.e $(TFTPDOWNLOADADDR) IPL0 \$${filesize} >> $(SCRIPTDIR)/[[ipl.es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[ipl.es

ipl_cust_$(FLASH_TYPE)__script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[ipl_cust.es
	@echo tftp $(TFTPDOWNLOADADDR) ipl_cust_s.bin > $(SCRIPTDIR)/[[ipl_cust.es
	@echo nand erase.part IPL_CUST0 >> $(SCRIPTDIR)/[[ipl_cust.es
	@echo nand write.e $(TFTPDOWNLOADADDR) IPL_CUST0 \$${filesize} >> $(SCRIPTDIR)/[[ipl_cust.es
	@echo nand erase.part IPL_CUST1 >> $(SCRIPTDIR)/[[ipl_cust.es
	@echo nand write.e $(TFTPDOWNLOADADDR) IPL_CUST0 \$${filesize} >> $(SCRIPTDIR)/[[ipl_cust.es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[ipl_cust.es

misc_$(FLASH_TYPE)__script:
	@echo "# <- this is for comment / isp & iqfile total file size must be less than 1MB in MISC partition" > $(SCRIPTDIR)/[[misc.es
	@echo nand erase.part MISC >> $(SCRIPTDIR)/[[misc.es
	@echo tftp $(TFTPDOWNLOADADDR) misc.bin >> $(SCRIPTDIR)/[[misc.es
	@echo nand write.e $(TFTPDOWNLOADADDR) MISC \$${filesize} >> $(SCRIPTDIR)/[[misc.es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[misc.es

kernel_$(FLASH_TYPE)__script:
	@echo "aaaabbbaaaaaaa $@"
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[kernel.es
	@echo tftp $(TFTPDOWNLOADADDR) kernel >> $(SCRIPTDIR)/[[kernel.es
	@echo nand erase.part KERNEL >> $(SCRIPTDIR)/[[kernel.es
	@echo nand write.e $(TFTPDOWNLOADADDR) KERNEL \$${filesize} >> $(SCRIPTDIR)/[[kernel.es
	@echo nand erase.part RECOVERY >> $(SCRIPTDIR)/[[kernel.es
	@echo nand write.e $(TFTPDOWNLOADADDR) RECOVERY \$${filesize} >> $(SCRIPTDIR)/[[kernel.es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[kernel.es
	@echo kernel-image done!!!

rtos_$(FLASH_TYPE)__script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[rtos.es
	@echo tftp $(TFTPDOWNLOADADDR) rtos >> $(SCRIPTDIR)/[[rtos.es
	@echo nand erase.part RTOS >> $(SCRIPTDIR)/[[rtos.es
	@echo nand write.e $(TFTPDOWNLOADADDR) RTOS \$${filesize} >> $(SCRIPTDIR)/[[rtos.es
	@echo nand erase.part RTOS_BACKUP >> $(SCRIPTDIR)/[[rtos.es
	@echo nand write.e $(TFTPDOWNLOADADDR) RTOS_BACKUP \$${filesize} >> $(SCRIPTDIR)/[[rtos.es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[rtos.es

uboot_$(FLASH_TYPE)__script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[uboot.es
	@echo tftp $(TFTPDOWNLOADADDR) uboot_s.bin >> $(SCRIPTDIR)/[[uboot.es
	@echo nand erase.part UBOOT0 >> $(SCRIPTDIR)/[[uboot.es
	@echo nand write.e $(TFTPDOWNLOADADDR) UBOOT0 \$${filesize} >> $(SCRIPTDIR)/[[uboot.es
	@echo nand erase.part UBOOT1 >> $(SCRIPTDIR)/[[uboot.es
	@echo nand write.e $(TFTPDOWNLOADADDR) UBOOT0 \$${filesize} >> $(SCRIPTDIR)/[[uboot.es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[uboot.es

boot_$(FLASH_TYPE)__script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[boot.es
	@echo tftp $(TFTPDOWNLOADADDR) boot.bin >> $(SCRIPTDIR)/[[boot.es
	@echo nand erase.part BOOT >> $(SCRIPTDIR)/[[boot.es
	@echo nand write.e $(TFTPDOWNLOADADDR) BOOT \$${filesize} >> $(SCRIPTDIR)/[[boot.es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[boot.es

logo_$(FLASH_TYPE)__script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[logo.es
	@echo  tftp $(TFTPDOWNLOADADDR) logo >> $(SCRIPTDIR)/[[logo.es
	@echo nand erase.part LOGO >> $(SCRIPTDIR)/[[logo.es
	@echo nand write.e $(TFTPDOWNLOADADDR) LOGO \$${filesize} >> $(SCRIPTDIR)/[[logo.es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[logo.es

%_$(FLASH_TYPE)_squashfs_script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@).es
	@echo tftp $(TFTPDOWNLOADADDR) $(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@).sqfs >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@).es
	@echo nand erase.part $(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@) >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@).es
	@echo nand write.e $(TFTPDOWNLOADADDR) $(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@) \$${filesize} >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@).es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@).es

%_$(FLASH_TYPE)_ubifs_script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).es
	@echo ubi part UBI >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).es
	#@echo -e $(foreach n,$(TARGET_FS),ubi create $(n) $($(n)$(PATSIZE))\\n) >> $(SCRIPTDIR)/[[ipl.es
	#@echo ubi create $(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@) $($(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@)$(PATSIZE)) >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).es
	@echo tftp $(TFTPDOWNLOADADDR) $(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).ubifs >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).es
	@echo ubi write $(TFTPDOWNLOADADDR) $(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@) \$${filesize} >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).es
	@echo "% <- this is end of file symbol" >>  $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).es

bootconfig_$(FLASH_TYPE)_ubifs_script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).es
	@echo printenv >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).es
	@echo "% <- this is end of file symbol" >>  $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).es

ubi_$(FLASH_TYPE)_partition_script:

ubi_$(FLASH_TYPE)_config_script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/set_config
	@echo setenv bootargs  $(rootfs$(BOOTENV)) $(kernel$(BOOTENV)) $(EXBOOTARGS) \$$\(mtdparts\) >> $(SCRIPTDIR)/set_config
ifeq ($(DUAL_OS), on)
	@echo setenv bootcmd \' nand read.e $(RTOSBOOTADDR) RTOS $(rtos$(PATSIZE))\; go $(RTOSBOOTADDR) >> $(SCRIPTDIR)/set_config
else
	@echo setenv bootcmd \' nand read.e $(KERNELBOOTADDR) KERNEL $(kernel$(PATSIZE))\; bootm $(KERNELBOOTADDR)\;nand read.e $(KERNELBOOTADDR) RECOVERY $(kernel$(PATSIZE))\; bootm $(KERNELBOOTADDR) >> $(SCRIPTDIR)/set_config
endif
	@echo saveenv >> $(SCRIPTDIR)/set_config
	@echo reset >> $(SCRIPTDIR)/set_config
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/set_config

spi_$(FLASH_TYPE)_partition_script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/set_partition
	@echo nand erase.chip >> $(SCRIPTDIR)/set_partition
	@echo ubi part UBI >> $(SCRIPTDIR)/set_partition
	@echo -e $(foreach n,$(TARGET_FS),ubi create $(n) $($(n)$(PATSIZE))\\n) >> $(SCRIPTDIR)/set_partition
	@echo saveenv >> $(SCRIPTDIR)/set_partition
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/set_partition

spi_$(FLASH_TYPE)_config_script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/set_config
	@echo setenv bootargs \' $(rootfs$(BOOTENV)) $(kernel$(BOOTENV)) $(EXBOOTARGS) >> $(SCRIPTDIR)/set_config
	@echo setenv bootcmd \' nand read.e $(KERNELBOOTADDR) KERNEL $(kernel$(PATSIZE))\; bootm $(KERNELBOOTADDR)\;nand read.e $(KERNELBOOTADDR) RECOVERY $(kernel$(PATSIZE))\; bootm $(KERNELBOOTADDR) >> $(SCRIPTDIR)/set_config
	@echo saveenv >> $(SCRIPTDIR)/set_config
	@echo reset >> $(SCRIPTDIR)/set_config
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/set_config

