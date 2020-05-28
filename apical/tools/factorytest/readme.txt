factorytest.ini 配置说明

null:null:Apical001_2.4G:apicalgood:

第一个 null 可以配置为 smt rtsp uvc aging monly mtest wfdp
null  对应正常使用模式
smt   对应 smt 测试和 full 测试
rtsp  用于 focus 测试 rtsp 出图
uvc   用于 focus 测试 uvc  出图
aging 用于老化测试，闪灯 + 声音播放 + 自动重启
monly 用于摇头机马达转动测试，只转马达
mtest 用于摇头机马达转动测试，转动马达 + IRCUT 切换 + 声音播放
wfdp  用于 wifi 定频测试

第二个 null 对应产测模式的 ft_uid，可以配置为测试 pc 的 ip 地址，这样当设备的 ft_uid 跟测试电脑的 ip 匹配时才能测试

后面两个参数为 wifi 路由器名称和密码

