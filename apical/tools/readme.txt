工具说明
--------

findipcam - 设备搜索工具



一些有用的命令
--------------
ffmpeg 转换 pcm alaw (g711a) 命令：

转换并推送到 TCP 服务器：
ffmpeg -re -i test.mp3 -f s16le -ar 8000 -ac 1 -acodec pcm_alaw tcp://192.168.2.140:7001

转换并保持到文件：
ffmpeg -i test.mp3 -f s16le -ar 8000 -ac 1 -acodec pcm_alaw -y out.alaw

MP3 转换为 AAC
ffmpeg -i test.mp3 -ar 16000 -ac 1 -ab 64k -y out.aac
ffmpeg -i in.mp3 -ar 16000 -ac 1 -ab 8k -y -ss 00:00:0.0 -t 00:00:0.2 out.aac



iqserver 工具使用说明
---------------------
登录机器管理页面 device settings 修改：

standby   为 1
iqserv_en 为 1

submit 后重启设备即可




-------------------
chenk@apical.com.cn
2018-12-21

