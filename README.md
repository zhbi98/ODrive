![ODrive Logo](https://static1.squarespace.com/static/58aff26de4fcb53b5efd2f02/t/59bf2a7959cc6872bd68be7e/1505700483663/Odrive+logo+plus+text+black.png?format=1000w)

This project is all about accurately driving brushless motors, for cheap. The aim is to make it possible to use inexpensive brushless motors in high performance robotics projects, like [this](https://www.youtube.com/watch?v=WT4E5nb3KtY).

| Branch | Build Status |
|--------|--------------|
| master | [![Build Status](https://travis-ci.org/madcowswe/ODrive.png?branch=master)](https://travis-ci.org/madcowswe/ODrive) |
| devel  | [![Build Status](https://travis-ci.org/madcowswe/ODrive.png?branch=devel)](https://travis-ci.org/madcowswe/ODrive) |

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
