insmod /config/modules/4.9.84/scsi_mod.ko
insmod /config/modules/4.9.84/nls_utf8.ko
#kernel_mod_list
insmod /config/modules/4.9.84/mhal.ko
#misc_mod_list
insmod /config/modules/4.9.84/mi_common.ko
insmod /config/modules/4.9.84/mi_sys.ko cmdQBufSize=256 logBufSize=256
insmod /config/modules/4.9.84/mi_ai.ko
insmod /config/modules/4.9.84/mi_ao.ko
insmod /config/modules/4.9.84/mi_rgn.ko
insmod /config/modules/4.9.84/mi_divp.ko
insmod /config/modules/4.9.84/mi_gyro.ko
insmod /config/modules/4.9.84/mi_ipu.ko
insmod /config/modules/4.9.84/mi_vpe.ko
insmod /config/modules/4.9.84/mi_sensor.ko
insmod /config/modules/4.9.84/mi_vif.ko
insmod /config/modules/4.9.84/mi_venc.ko
insmod /config/modules/4.9.84/mi_shadow.ko
#mi module
major=`cat /proc/devices | busybox awk "\\$2==\""mi_poll"\" {print \\$1}"`
busybox mknod /dev/mi_poll c $major 0

#misc_mod_list_late
insmod /config/modules/4.9.84/imx307_MIPI.ko chmap=1 lane_num=2 hdr_lane_num=2
mdev -s
