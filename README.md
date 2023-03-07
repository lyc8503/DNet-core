# DNet-core
An implementation of TCP/IP network stack in user space. DNet stands for DisconNet, inspired by diaoda and Cierra.

#### 目前已经实现的内容
- Linux 下虚拟网卡的创建与配置
- Ethernet II 帧的解析
- ICMP Echo Request / Reply
- ARP Resulotion / ARP Reply
- IPv4 Packet 的解析与构造
- UDP 包的接收与发送 (简易的 UDP Echo)

#### 项目文件结构
```
- driver
  Linux TAP 设备初始化 & 读取写入相关代码
- layers
  TCP/IP 协议栈各层相关的代码
- util
  实现的一些通用工具函数
- docs
  相关配置方法的说明文档
```

#### To-Dos
- 实现 TCP 状态机
- 实现常见的一些应用层协议
- 设计 HTTP 接口供外部访问修改
- 整理相关代码(目前在设计上比较杂乱)
- 关于 Web 前端的部分将在 https://github.com/NJU-uFFFD/DNet-webui 中实现
