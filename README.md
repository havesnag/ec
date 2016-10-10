ec是基于libevet的C++11封装。

# 编译

### 依赖条件

- C\++版本需要支持C++11
- libevent需要2.0以上版本 

### 编译方法
没有提供CMake和安装脚本。

编译成动态链接库：
```
make -f makefile
```

编译测试用例:
```
make -f ExampleMakefile
```

# 整体架构

![输入图片说明](http://git.oschina.net/uploads/images/2016/0713/162327_b73c47c3_428019.jpeg "在这里输入图片标题")

ec的的命名空间为ec，主要包含如下几个类：
- Loop 事件循环和线程的结合，对libevent中struct event_base的封装；
- Timer 定时器类，依赖Loop，支持按周期定时、延迟定时、永久定时；
- TcpServer TCP服务器类，线程安全，可设置IO线程数量；
- HttpServer HTTP服务器类，仅实现了部分简单功能；


### Loop

Loop封装了事件循环和线程管理，建议一个线程只拥有一个Loop；
Timer、TcpSocket必需关联到Loop，相应行为都将在Loop的线程触发。


```
#include "ec/loop.h"
int main(int argc, char *argv[])
{
    ec::Loop loop;
    loop.start(false); //false当前线程阻塞，true则以新线程运行
    loop.wait(); //等待线程结束，仅对以新线程运行方式有效
}
```

### TcpServer

多线程的TCP服务器，可设置IO线程的数量


# 测试示例

先用make -f ExampleMakefile编译生成test后：

```
./test timer
```

./test timer 测试定时器用法。

./test tcp 测试TCP的用法。

./test http 将启动HTTP服务器，访问http://127.0.0.1:4567/。

### 测试代码
test.cpp 为主入口，具体功能实现在example目录中

\/\/定时器示例

void timerExample();

\/\/TCP示例

void tcpExample();

\/\/HTTP示例

void httpExample();

