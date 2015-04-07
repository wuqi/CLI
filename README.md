# CLI

这是一个简单的命令行解析程序，源代码在src文件夹下，其他两个文件夹是工程文件

用了ezOptionParser(http://ezoptionparser.sourceforge.net/)，不过是重写的，原来那个基本是用C写的，现在换成STL了，增加了一些功能，把一些用不到的也删减了。

主要修改：
* vc6编译通过
* warnings（vc6 level 3,vs2010 level 3,mingw -Wall）
* 增加了unlabeled flag
* 增加异或flag
* 增加ez_file类型检测
* 删除prettyPrint
* 删除importFile
* 删除ezOptionValidator，使用validate函数
* 增加checkValid，删除gotExpected等几个函数
* optiongroup中使用模板函数代替原来get**
* 统一使用stringstream转换类型
