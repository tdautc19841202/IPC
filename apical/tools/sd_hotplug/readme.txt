linux�����Ȳ�μ��ű�������
1��kernel ��Ҫ�� CONFIG_UEVENT_HELPER 


2��rcS ��Ҫ�����£�����ʾ�����£�

#!/bin/sh

mount -a
mkdir -p /dev/shm
mkdir -p /dev/pts
mount devpts
echo /sbin/mdev > /proc/sys/kernel/hotplug
/sbin/mdev -s
telnetd -l sh

3��vi /etc/mdev.conf
mmcblk[0-9]p[0-9] 0:0 666 @/ext/sd_card_inserting

mmcblk[0-9] 0:0 666 $/ext/sd_card_removing
@�����sd������Ľű�   $�����sd���γ��Ľű�
�ű�ʾ����
sd_card_inserting��
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