# <- this is for comment / total file size must be less than 4KB

tftp 0x21000000 one.bin
sf probe 0
sf erase 0x0 0x800000
sf write 0x21000000 0x000000 $(filesize)

# update kernel start address
mxp t.load
mxp r.info KERNEL
setenv sf_kernel_start $(sf_part_start)
setenv sf_kernel_size $(sf_part_size)
setenv bootcmd ' sf probe 0;sf read 0x21000000 $(sf_kernel_start) $(sf_kernel_size);bootm 0x21000000
saveenv

% <- this is end of file symbol
