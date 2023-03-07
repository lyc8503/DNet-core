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

#### 如何运行该项目
经过测试的系统环境: Ubuntu 22.04 LTS, 其他系统应该也可以运行

```
# 安装相关依赖
sudo apt install build-essential git cmake

# Clone 项目
git clone https://github.com/lyc8503/DNet-core

# 使用 CMake 编译
cd DNet-core && mkdir build && cd build && cmake .. && make

# 创建 /dev/net/tap
sudo mknod /dev/net/tap c 10 200

# 运行 DNet
sudo ./DNet
```

运行后会创建 TAP 设备, 可使用 ifconfig 或 ip a 查看, 默认会使用 10.0.0.0/24 段, 可以测试 arping/ping/UDP Echo, 如和局域网有冲突可修改 main.cpp 中的 IP 段.

#### To-Dos
- 实现 TCP 状态机
- 实现常见的一些应用层协议
- 设计 HTTP 接口供外部访问修改
- 整理相关代码(目前在设计上比较杂乱)
- 关于 Web 前端的部分将在 https://github.com/NJU-uFFFD/DNet-webui 中实现
