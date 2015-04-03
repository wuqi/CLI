#include "ezOptionParser.hpp"

using namespace ez;

void Usage (ezOptionParser& opt)
{
  std::string usage;
  opt.getUsage (usage);
  std::cout << usage;
};
int main (int argc, const char** argv)
{
  //命令行程序

  ezOptionParser opt;
  //概要说明
  opt.overview = "sbi:Make the Soil brightness index product.";
  //语法,不做说明则将自动生成
  //opt.syntax = "cli [-h] [-e|-f]  [-t modis|fy3a] -i Arg1,[ArgN] -o Arg";
  //实例
  opt.example = "./sbi.exe -i MODIS.1000.2013197031000.H27V07.000000,MODIS.1000.2013197044500.H27V07.000000 -o MuSQ.SBI.1km..2013197000000.H27V05.001.h5\n\n";
  ////设置命令行选项  help 命令将自动设置，无需再设置
  opt.add (
    "10", // 默认值,可以设置为空.
    0, // 是否为必须?是则为1
    1, // 指定参数个数，-1用于传入列表,指定-1时最少需要1个参数.
    "Test short args.", // Usage里显示的信息.
    "-s",     // Flag token.
    "--short", // Flag token.
    EZ_INT16 //指定数据类型，当传入越界时将显示错误，停止运行
  );
  opt.add (
      "77", // 默认值,可以设置为空.
      0, // 是否为必须?是则为1
      2, // 指定参数个数，-1为不指定，用于传入列表.
      "Test max range,10-100.", // Usage里显示的信息.
      "-d",     // Flag token.
      "--double", // Flag token.
      ez::EZ_DOUBLE,
      "10",
      "100"
      );
  //测试指定列表允许值
  std::vector<std::string> validValues;
  validValues.push_back("modis");
  validValues.push_back("fy3a");
  opt.add (
      "", // 默认值,可以设置为空.
      0, // 是否为必须?是则为1
      1, // 指定参数个数，-1为用于传入列表,列表至少有一个参数.
      "Input file type,modis or fy3a, default is modis.", // Usage里显示的信息.
      "-t",     // Flag token.
      "--type", // Flag token.
      ez::EZ_TEXT,//指定类型
      "",//最小值
      "",//最大值
      validValues//允许值
      );
  opt.add (
    "", // Default.
    1, // 必须
    -1, // 指定参数个数
    "Input files arguments,test argument list.", // Help description.
    "-i",     // Flag token.
    "--inputs", // Flag token.
    ez::EZ_TEXT //text可以不指定类型
  );
  opt.add (
    "", // Default.
    1, // 必须
    1, // 输出参数为1个.
    "Output files arguments.", // Help description.
    "-o",     // Flag token.
    "--outputs", // Flag token.
    ez::EZ_TEXT
  );
  //test xor
  opt.xorAdd("-d","-s");
  //解析命令行选项
  opt.parse (argc, argv);
  //显示帮助
  if (opt.isSet("-h"))
  {
    std::string usage;
    opt.getUsage(usage);
    std::cout<<usage;
    return 0;
  }
  std::string out;
  //检测参数输入是否正确，不正确则返回1，同时显示usage
  if (!opt.checkValid(out)) {
    std::cout << out;
    return 1;
  }
  std::cout << out;
  //获取参数,使用get获取单个参数
  //使用getVector获取多个参数，返回vector列表
  //使用getMultiVector获取多个选项所有的vector列表
  std::vector< std::string >  inputfiles;
  opt.get ("-i")->getVector (inputfiles);
  std::string outputfile;
  opt.get ("-o")->get (outputfile);

  //print argument
  for (int i=0;i<(int)inputfiles.size();i++)
  {
    std::cout<<inputfiles[i]<<std::endl;
  }
  std::vector<double> dvalue;
  opt.get("-d")->getVector(dvalue);
  std::cout<<dvalue[0]<<" "<<dvalue[1]<<std::endl;
  short svalue;
  opt.get("-s")->get(svalue);
  std::cout<<svalue<<std::endl;
  std::string strvalue;
  opt.get("-t")->get(strvalue);
  std::cout<<strvalue<<std::endl;
  //processing
  try {
    //start processing
    //end processing
  } catch (...) {
    
  }
  getchar();
  return 0;
}