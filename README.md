# Sylar

## 开发环境

Ubuntu20.04 

gcc 9.3

cmake

## 项目路径

bin --二进制文件

build -- 中间文件路径

cmake -- cmake函数文件夹

CmakeLists.txt --cmake定义文件

lib -- 库的输出路径

Makefile sylar --源代码路径

tests --测试代码路径

## 日志系统

1） 仿造log4j

Logger(定义日志类别)

​     |       Formatter（日志格式）

Appender（日志输出的地方）

## 配置系统

Config --> Yaml文件

```cpp
template<T,FromStr,Tostr>
class ConfigVar;

template<F,T>
LexicalCast;
// 容器偏特化， 支持vector
// list,set,map,unordered_set,unordered_map 仅支持key = std::string
// Config::LookUp定义参数
```

