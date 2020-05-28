linux设置热插拔检测脚本方法：
1、kernel 中要打开 CONFIG_UEVENT_HELPER 


2、rcS 需要设置下，设置示例如下：

#!/bin/sh

mount -a
mkdir -p /dev/shm
mkdir -p /dev/pts
mount devpts
echo /sbin/mdev > /proc/sys/kernel/hotplug
/sbin/mdev -s
telnetd -l sh

3、vi /etc/mdev.conf
mmcblk[0-9]p[0-9] 0:0 666 @/ext/sd_card_inserting

mmcblk[0-9] 0:0 666 $/ext/sd_card_removing
@后面加sd卡插入的脚本   $后面加sd卡拔出的脚本
脚本示例：
sd_card_inserting：
#!/bin/sh
    echo "sd card insert!" > /dev/console

    if [ -e "/dev/mmcblk0p1"  ]; then
        mount -t vfat /dev/mmcblk0p1 /mnt
    elif [ -e "/dev/mmcblk0"  ]; then
        mount -t vfat /dev/mmcblk0 /mnt
    else
        echo "mount sd card failed!" > /dev/console
    fi
    ;;