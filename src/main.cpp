#include "ezOptionParser.hpp"
#include <iostream>

int main (int argc, const char** argv)
{
  //命令行程序
  ez::OptionParser opt;

  //概要说明
  opt.overview = "sbi:Make the Soil brightness index product.";
  //语法,不做说明则将自动生成
  //opt.syntax = "cli [-h] [-e|-f]  [-t modis|fy3a] -i Arg1,[ArgN] -o Arg";
  //实例
  opt.example = "./sbi.exe -i input1.h5,input2.h5,input3.h5 output.hdf";
  opt.footer = "version:0.9,LIC:See MIT-LICENSE.";
  ////设置命令行选项  -h,--usage,--help 命令将自动设置，无需再设置
  opt.add (
    "-s,--short",       // 选项的前导符，可以有多个，使用逗号隔开，
    //    如果前导符中没有"-"或者 "--"，则认为是无标识的选项
    //    即不需要前导符，直接传入
    false,                  // 是否为必须?是则为1
    1,                  // 指定参数个数，-1用于传入列表,指定-1时最少需要1个参数.
    "Test short args.", // 帮助说明
    ez::EZ_INT16,           //指定数据类型，指定后将自动判断类型的最大最小值,此后的参数为非必需参数
    "10",               // 默认值,可以设置为空.
    "1",                //指定最小值，若不需要则为""
    "",                 //指定最大值，若不需要则为""
    "1,10,15,20"        //设定允许值，默认为空，都允许
  );
  //添加double类型，必须为两个参数
  opt.add("-d,--double",false ,2, "Test range valid: range:10-100" ,ez::EZ_DOUBLE,"77,89","10.0", "100.0");
  opt.add("-t,--type",false,2, "Input file type,modis or fy3a, default is modis.",ez::EZ_TEXT,"modis","", "", "modis,fy3a");
  opt.add("-i,--inputs",true,-1,"Input files arguments,test argument list.");

  //无前导符的参数
  opt.add ("output", true, 1, "Output file argument." );
  //命令行中，可以使用"-bc"同时设置两个无参数的选项
  opt.add ("-b", false, 0, "test combined arguments");
  opt.add ("-c", false, 0, "test combined arguments");
  opt.add ("-C", true, 1, "test combined arguments");
  opt.add ("-f,--file", false, 1, "test file arguments", ez::EZ_FILE);
  opt.add ("-D,--dir", false, 1, "test dir arguments", ez::EZ_DIR);
  //互斥参数设置：使用逗号隔开一串第一个flag设置，互斥参数必须为可选，否则将出现逻辑问题
  opt.xorAdd ("-d,--short,-c");
  opt.xorAdd ("-b,-f");

  //解析命令行选项,如果检测失败，直接退出
  if (!opt.parse (argc, argv)) {
    return -1;
  }

  //获取参数,使用get获取单个参数
  //使用getVector获取多个参数，返回vector列表
  //使用getMultiVector获取多个选项所有的vector列表
  std::vector< std::string >  inputfiles;
  opt.get ("-i").getVector (inputfiles);
  std::string outputfile;
  opt.get ("output").get (outputfile);
  std::string inputType;
  opt.get ("-t").get (inputType);
  std::string inputdir;

  if (opt.get ("-r").isSet) {
    opt.get ("-r").get (inputdir);
  }

  //判断参数是否设置：
  short svalue = 0;
  double dvalue = 0;
  bool bset = false, cset = false;

  if (opt.get ("-s").isSet) {
    opt.get ("-s").get (svalue);
  } else {
    opt.get ("-d").get (dvalue);
  }

  if (opt.get ("-b").isSet) {
    bset = true;
  }

  if (opt.get ("-c").isSet) {
    cset = true;
  }

  //显示参数:
  std::cout << "Inputs:" << std::endl;

  for (int i = 0; i < (int)inputfiles.size(); i++) {
    std::cout << inputfiles[i] << std::endl;
  }

  std::cout << "double: " << dvalue << std::endl;
  std::cout << "short: " << svalue << std::endl;
  std::cout << "type: " << inputType << std::endl;
  std::cout << "output: " << outputfile << std::endl;
  std::cout << "inputdir: " << inputdir << std::endl;
  std::cout << "b is set:" << bset << std::endl;
  std::cout << "c is set:" << cset << std::endl;

  //processing
  try {
    //start processing
    //end processing
  } catch (...) {

  }

  getchar();
  return 0;
}
