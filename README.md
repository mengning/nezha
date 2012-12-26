Nezha
=====

Nezha is code name of this project.

哪吒（Nezha）是中国神话故事里的少年战神，我们以其作为基于Tokyo Cabinet的简单分布式KV存储系统原型项目的代号。

Software Architecture of Nezha
------------------------------

主要源文件及其依赖关系

Makefile
client.c 
    |-> cmdline.h/cmdline.c 
                |-> dbapi.h/remotedbapi.c 
                            |-> socketwrapper.h/socketwrapper.c
                            |-> protocol.h/protocol.c
dbserver.c 
    |-> dbapi.h/dbapi.c
    |-> socketwrapper.h/socketwrapper.c
    |-> protocol.h/protocol.c
    |-> msgq.h/msgq.c
    |-> event.h/event.c
doc/
见doc目录下的图片等资料