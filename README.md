# CLI

这是一个简单的命令行解析程序，源代码在src文件夹下，其他两个文件夹是工程文件

用了ezOptionParser，不过是重写的，原来的名字和其他的没动，删了很多,原来那个基本是用C写的，现在换成STL了，自己用着舒服就行，把一些用不到的也删减了（reset 输出样式选择等）。

主要修改：
* vc6编译通过
* warnings为0（vc6 level 3,vs2010 level 3,mingw -Wall）
* 增加了unlabeled flag
* 增加异或flag
* 增加ez_file类型检测
