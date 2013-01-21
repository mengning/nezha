Nezha
=====

Nezha is code name of this project.

哪吒（Nezha）是中国神话故事里的少年战神，我们以其作为基于Tokyo Cabinet的简单分布式KV存储系统原型项目的代号。

It contains configdb lib(configdb.h/libconfigdb.so) and a command line test program(Nezha)

执行make configdb和sudo make install编译安装configdb，然后可以开始使用configdb.h提供的接口进行开发了

Software Architecture of Nezha
------------------------------

主要源文件及其依赖关系

Makefile
nezha
    client.c 
        |-> cmdline.h/cmdline.c 
                |-> configdb.h/libconfigdb.so
libconfigdb.so
    configdb.h/configdb.c
            |-> nodes.h/nodes.c
            |-> engine.h/engine.c
            |-> msgq.h/msgq.c
            |-> event.h/event.c
            |-> remotedbapi.h/remotedbapi.c
            |-> socketwrapper.h/socketwrapper.c
            |-> protocol.h/protocol.c
            |-> dbapi.h/dbapi.c

test* files for unit testing
doc/
见doc目录下的图片等资料