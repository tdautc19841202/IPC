# <- this is for comment / total file size must be less than 4KB

mxp  t.init
tftp 0x21000000 MXP_SF_8M.bin
mxp  t.update 0x21000000
mxp  t.load

% <- this is end of file symbol
