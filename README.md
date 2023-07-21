# libevent
1如果您只需要交叉编译libevent库而不进行安装，您可以按照以下步骤进行操作：

2下载libevent源代码：从libevent的官方网站（https://libevent.org/）下载最新的稳定版本的源代码，并将其解压到您选择的目录中。

3进入源代码目录：打开终端，使用cd命令进入解压后的libevent源代码目录。

4设置交叉编译环境变量：根据您的需求，设置以下环境变量，以便使用交叉编译工具链进行编译：
export ARCH=arm
export CROSS_COMPILE=/usr/local/arm_linux_4.8/bin/arm-nuvoton-linux-uclibceabi-
export PATH=/usr/local/arm_linux_4.8/bin:$PATH
这些环境变量将告诉编译器和链接器使用交叉编译工具链。

5配置编译选项：运行以下命令以配置编译选项：
./configure --host=arm-linux
这将告诉配置脚本使用交叉编译工具链。

6编译：运行以下命令开始编译libevent库：
make
这将使用交叉编译工具链进行编译，并生成库文件和示例程序。
