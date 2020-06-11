# <- this is for comment / total file size must be less than 4KB

mxp r.info KERNEL
sf probe 0
sf erase $(sf_part_start) $(sf_part_size)
tftp 0x21000000 kernel_xip.img
sf write 0x21000000 $(sf_part_start) $(filesize)
setenv bootcmd ' bootm 0x14060000
saveenv


% <- this is end of file symbol
