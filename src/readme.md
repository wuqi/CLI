# ezOptionParser  修改版

这是一个简单的命令行解析库，仅只有一个hpp文件，修改自sourceforge上的ezOptionParser(http://ezoptionparser.sourceforge.net/ )库

原来的库使用ansi C实现，现在使用STL和模板函数，对功能进行了增删，方便使用。

##使用说明：

可以参见main.cpp中的示例，下面是详细的步骤说明：

###初始化

先初始化ez::OptionParser，设置示例、语法、概要说明，以及版权等。

```cpp
  ez::OptionParser opt;
  opt.overview = "sbi:Make the Soil brightness index product.";/*概要说明,一般需要填写，简要介绍程序的作用*/
  opt.example = "./sbi.exe -i input1.h5,input2.h5,input3.h5 output.hdf";/*示例,可以不填*/
  opt.footer = "version:0.9,LIC:See MIT-LICENSE.";/*脚注,可以不填,一般可以在其中包含版权,开源协议等信息*/
  
  /*  语法说明，一般不必填写,会自动生成语法说明,填写将自动覆盖语法说明*/
  /*opt.syntax = "cli [-h] [-e|-f]  [-t modis|fy3a] -i Arg1,[ArgN] -o Arg";*/
```

###添加option

然后使用add函数，添加option(选项)，函数说明如下:

```cpp
  /**
  * @brief 添加参数
  * @param flags 参数标识,可有多个,get时使用,用逗号分隔
                 没有带-或者--的,作为无标签的参数
                 传入时不需要带前导符,比如input,可以直接写"input",传的时候直接传文件名
  * @param required 是否必须填写
  * @param optType 参数类型
  * @param expectArgs 期望传入数量
                      0表示不带参数,
                      -1表示任意多个,
                      1表示一个,2表示两个,以此类推
                      参数紧跟标志后，用逗号分隔)
  * @param help 帮助信息,最终显示在help中
  * @param defaults 默认值
  * @param minValue 最小值,若不需要则为""
  * @param maxValue 最大值,若不需要则为""
  * @param validListStr 有效值列表,设置后最大最小值无效,用逗号分隔,可以是数字或者单词,不需要则不指定或为""
  */
  inline void add( const char *flags,
                   bool required = true,
                   int expectArgs = 1, 
                   const char *help = "", 
                   EZ_TYPE optType = EZ_NOTYPE,
                   const char *defaults = "",
                   const std::string &minValue = std::string(),
                   const std::string &maxValue = std::string(),
                   const  char *validListStr = "" )
```

  选项类型有以下几类:
  
* 简单无参选项，例如 `cli -b` ,多个无参选项可以像bash命令一样叠加使用,如  `cli -bc` 等同于 `cli -b -c`
```cpp
  opt.add ("-b", false, 0, "test combined arguments");
  opt.add ("-c", false, 0, "test combined arguments");
```

* 普通选项，例如 `cli -s 100`,普通选项中参数个数必须指定,使用`-1`可表示跟多于一个的任意个参数，例如`cli -s 100,200,300,400`
```cpp
  /**带两个double类型参数的选项,默认值为77,89的参数,非必须指定,值范围为10-100*/
  opt.add("-d,--double",false ,2, "Test range valid: range:10-100" ,ez::EZ_DOUBLE,"77,89","10.0", "100.0");
  /**带一个text类型参数的选项,默认值为`modis`的参数,非必须指定,值为modis或者fy3a*/
  opt.add("-t,--type",false,2, "Input file type,modis or fy3a, default is modis.",ez::EZ_TEXT,"modis","", "", "modis,fy3a");
```

* 不带flag的选项，在设置flags时，不带前导的`-`即为不带flag的选项，例如有些只需要输入，输出的选项，不带flag的参数必须按照添加的顺序传入
```cpp
  /**一个输出选项，不需要带flag，*/
  opt.add ("output", true, 1, "Output file argument." );
```

###指定互斥选项(可选)

设置完成后，指定互斥选项，即哪些选项不能同时开启（若没有可以跳过），互斥选项的required必须为false，否则将出现逻辑问题。选项可以使用任意flag指定,不过显示只显示第一个.
```cpp
  opt.xorAdd ("-d,--short,-c");
  opt.xorAdd ("-b,-f");
```

###开始解析

全部设置完成后，即可开始解析,判断解析是否失败，失败则直接返回
```cpp
    if(!opt.parse (argc, argv)){
	  return -1;
    }
```

###获取选项参数

  首先获取选项
```cpp
    /*直接使用get即可*/
    opt.get("-s")；
```

  获取参数有四个相关函数和属性，分别对应不同用途。

  * `isSet`属性,获取参数是否设置，对于无参列表中的参数和非必须参数是否设置比较有用,例如`cli -i`
  * `get (T& out)`获取单个参数,只有一个参数的选项可以使用此函数，例如`cli -i 100`
  * `getVector (std::vector<T>& out)` 获取单个选项的多参数时使用此函数,例如`cli -i 100,200`
  * `getMultiVector (std::vector< std::vector<T> >& out)` 获取某选项多次设置的参数时使用此选项,例如`cli -i 1 -i 100 -i 400`

  示例如下:
```cpp
  bool bset = opt.get("-b").isSet;
  std::string inputdir;
  if(opt.get("-r").isSet){opt.get("-r").get(inputdir);}
  std::vector< std::string >  inputfiles;
  opt.get ("-i").getVector (inputfiles);
```

  获取完成后，即可开始运行程序

##使用注意：

* 使用语法如果没有设置，将自动生成
* Flag使用逗号分隔，前导`-`或`--`为有参,前导为`-`的必须为单个字母,区分大小写
* 设置了validValues后，min和max无效
* 设置了选项类型后，将自动判断输入参数最大最小范围
* 不限制选项出现的次数，如果带参数的选项出现多次，get将选择第一个值，或者使用getMultiVector获取其他值
* 无需添加help选项，默认会添加，使用 `-h --usage --help` 调用
* 可以多个无参数选项叠加
* EZ_FILE、EZ_DIR类型将检测文件和目录是否存在，如果不存在检测将报错，一般用于输入类型，所以输出时请用EZ_TEXT类型

##主要修改：

2015-11-17

* 多组互斥选项实现,格式化代码

2015-11-15

* 简化流程，自动进行参数检查以及help输出，不需要手动再次设置

2015-11-08

* 添加说明文件

2015-11-07

* 增加ez_dir类型
* 增加函数注释说明

2015-10-19

* stringstream clear bug fix

2015-04-07

* 增加ez_file类型
* vc6编译通过
* no warnings（vc6 level 3,vs2010 level 3,mingw -Wall）
* 增加了unlabeled flag
* 增加互斥选项
* 删除prettyPrint
* 删除importFile
* 删除ezOptionValidator，使用validate函数
* 增加checkValid，删除gotExpected等几个函数
* optiongroup中使用模板函数代替原来get**
* 统一使用stringstream转换类型
