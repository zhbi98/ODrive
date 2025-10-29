## Important Note

The firmware in this repository is compatible with the ODrive v3.x (NRND) and is no longer under active development.

Firmware for the new generation of ODrives (ODrive Pro, S1, Micro, etc.) is currently being actively maintained and developed, however its source code is currently not publicly available. Access may be available under NDA, please reach out to us for inquiries.

> 官方声明已经不再开源 ODrive 的新版本代码了，并且这个已经开源的旧硬件配套代码固件也不再进行更新，这个固件版本目前支持到 ODrive v3.x 的硬件，不过这里我会继续进行维护。

## Overview

![ODrive Logo](https://static1.squarespace.com/static/58aff26de4fcb53b5efd2f02/t/59bf2a7959cc6872bd68be7e/1505700483663/Odrive+logo+plus+text+black.png?format=1000w)

This project is all about accurately driving brushless motors, for cheap. The aim is to make it possible to use inexpensive brushless motors in high performance robotics projects, like [this](https://www.youtube.com/watch?v=WT4E5nb3KtY).

| Branch | Build Status                                                                                                        |
| ------ | ------------------------------------------------------------------------------------------------------------------- |
| master | [![Build Status](https://travis-ci.org/madcowswe/ODrive.png?branch=master)](https://travis-ci.org/madcowswe/ODrive) |
| devel  | [![Build Status](https://travis-ci.org/madcowswe/ODrive.png?branch=devel)](https://travis-ci.org/madcowswe/ODrive)  |

[![pip install odrive (nightly)](https://github.com/madcowswe/ODrive/workflows/pip%20install%20odrive%20(nightly)/badge.svg)](https://github.com/madcowswe/ODrive/actions?query=workflow%3A%22pip+install+odrive+%28nightly%29%22)

Please refer to the [Developer Guide](https://docs.odriverobotics.com/v/latest/developer-guide.html#) to get started with ODrive firmware development.

### Repository Structure

* **Firmware**: ODrive firmware
* **tools**: Python library & tools
* **docs**: Documentation

### Other Resources

* [Main Website](https://www.odriverobotics.com/)
* [User Guide](https://docs.odriverobotics.com/)
* [Forum](https://discourse.odriverobotics.com/)
* [Chat](https://discourse.odriverobotics.com/t/come-chat-with-us/281)

### Prompt

我简化了 ODrive 的文件组织方式，方便理解代码，并且将 ODrive 复杂的编译方式改成使用 CMake 进行构建，并且可以搭配 VSCode 进行图形化调试，这有利于理解 ODrive 的程序。 

I simplified the file organization of ODrive to make understanding the code easier, and changed ODrive's complex compilation method to use CMake for building. Additionally, it can be paired with VSCode for graphical debugging, which is beneficial for understanding the ODrive program. 

如果没有 VSCode 的嵌入式开发环境，可以看看这篇文章：https://blog.csdn.net/jf_52001760/article/details/126826393 

If you don't have VSCode's embedded development environment, check out this article: https://blog.csdn.net/jf_52001760/article/details/126826393

### Configuration

ODrive 的配置都是包含默认值的，所以不需要逐个配置参数电机也能正常工作起来的，必须要注意的是根据实际编码器配置 cpr，index 参数，根据电机配置极对数 pole pairs（例如大疆 2312s 电机的极对数为 7），以及按实际调整电流采样电阻，配置一些必须变更的即可。

什么是极对数？

极数是指电机转子（即永磁体部分）磁场中磁极的总数量，磁极总数除以 2 就是极对数，极对数用 P 表示，每个极对包含一个北磁极 N 和一个南磁极 S。

## Start-up

(1) 电机校准（实际就是测量电机的相电阻和相电感）。

(2) 搜索定位编码器 index 信号（在编码器偏移校准之前必须先搜索 index）。

(3) 编码器偏移校准（Offset 校准）。

(4) 使能闭环状态/开环状态。

(5) 设置控制模式（控制模式包含：位置模式，速度模式，电流模式）。和 (4) 结合起来就得到，位置闭环控制模式，速度闭环控制模式，电流闭环控制模式。

(6) 设置控制模式目标值（位置值，速度值，电流值）来调节电机运行状态。

(7) 电机因异常停止后清除错误重新使能进入闭环状态即可。
