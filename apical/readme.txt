文件说明
--------

8188ftv - wifi 驱动和相关脚本程序
www     - http 服务器主目录
cgi     - http 服务器 cgi 程序源代码
ipcam   - ip camera 的主程序
res     - 存放音频，摄像头 IQ 文件，等资源
apkapi  - apkapi 是一个 c 静态库，实现了常用函数
tools   - 工具程序
docs    - 文档
release - 编译生成目录（可以删掉）



spi flash 烧录文件制作方法
--------------------------
执行命令：
./build.sh flash kpj|ytj|deng
即可生成 one.bin，可用于 spi flash 烧录和 sd 卡升级
（注意 sd 卡升级不会更新 uboot 的环境变量）


-------------------
chenk@apical.com.cn
2018-12-21

