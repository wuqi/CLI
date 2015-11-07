#include "ezOptionParser.hpp"
#include <iostream>
using namespace ez;

 
int main (int argc, const char** argv)
{
  //�����г���
  ezOptionParser opt;

  //��Ҫ˵��
  opt.overview = "sbi:Make the Soil brightness index product.";
  //�﷨,����˵�����Զ�����
  //opt.syntax = "cli [-h] [-e|-f]  [-t modis|fy3a] -i Arg1,[ArgN] -o Arg";
  //ʵ��
  opt.example = "./sbi.exe -i input1.h5,input2.h5,input3.h5 output.hdf";
  opt.footer = "version:0.9,LIC:See MIT-LICENSE.";
  ////����������ѡ��  -h,--usage,--help ����Զ����ã�����������
  opt.add (
    "10",               // Ĭ��ֵ,��������Ϊ��.
    false,                  // �Ƿ�Ϊ����?����Ϊ1
    1,                  // ָ������������-1���ڴ����б�,ָ��-1ʱ������Ҫ1������.
    "Test short args.", // ����˵��
    "-s,--short",       // ѡ���ǰ�����������ж����ʹ�ö��Ÿ�����
                        //    ���ǰ������û��"-"���� "--"������Ϊ���ޱ�ʶ��ѡ��
                        //    ������Ҫǰ������ֱ�Ӵ���
    EZ_INT16,           //ָ���������ͣ�ָ�����Զ��ж����͵������Сֵ,�˺�Ĳ���Ϊ�Ǳ������
                       
    "1",                //ָ����Сֵ��������Ҫ��Ϊ""
    "",                 //ָ�����ֵ��������Ҫ��Ϊ""
    "1,10,15,20"        //�趨����ֵ��Ĭ��Ϊ�գ�������
  );
  //���double���ͣ�����Ϊ��������
  opt.add ("77,89",false,2,"Test range valid: range:10-100","-d,--double",ez::EZ_DOUBLE,"10.0","100.0");

  opt.add ("modis", false, 1,"Input file type,modis or fy3a, default is modis.","-t,--type",ez::EZ_TEXT,"","","modis,fy3a");

  opt.add ("",true,-1,"Input files arguments,test argument list.","-i,--inputs",EZ_TEXT);
  //��ǰ�����Ĳ���
  opt.add ("",true,1,"Output file argument.","output",ez::EZ_TEXT);
  //�������У�����ʹ��"-bc"ͬʱ���������޲�����ѡ��
  opt.add("",false,0,"test combined arguments","-b");
  opt.add("",false,0,"test combined arguments","-c");
  opt.add("",false,1,"test file arguments","-f,--file",ez::EZ_FILE);
  opt.add("",false,1,"test dir arguments","-r,--dir",ez::EZ_DIR);
  //����������ã�ʹ�ö��Ÿ���һ����һ��flag���ã������������Ϊ��ѡ�����򽫳����߼�����
  opt.xorAdd("-d,-s");
  //����������ѡ��
  opt.parse (argc, argv);
  //��ʾ����
  if (opt.isSet("-h"))
  {
    std::cout<<opt.getUsage();
    return 0;
  }

  //�����������Ƿ���ȷ������ȷ�򷵻�1��ͬʱ��ʾusage
  std::string out;
  if (!opt.checkValid(out)) {
    std::cout << out;
    return 1;
  }
  //��ʾ����
  std::cout<<out;
  //��ȡ����,ʹ��get��ȡ��������
  //ʹ��getVector��ȡ�������������vector�б�
  //ʹ��getMultiVector��ȡ���ѡ�����е�vector�б�
  std::vector< std::string >  inputfiles;
  opt.get ("-i").getVector (inputfiles);
  std::string outputfile;
  opt.get ("output").get (outputfile);
  std::string inputType;
  opt.get("-t").get(inputType);
  std::string inputdir;
  if(opt.get("-r").isSet){opt.get("-r").get(inputdir);};
  //�жϲ����Ƿ����ã�
  short svalue;
  double dvalue;
  bool bset = false,cset = false;
  if(opt.get("-s").isSet){
    opt.get("-s").get(svalue);
  }else{
    opt.get("-d").get(dvalue);
  }
  if(opt.get("-b").isSet){bset = true;}
  if(opt.get("-c").isSet){cset = true;}
  //��ʾ����:
  std::cout<<"Inputs:"<<std::endl;
  for (int i=0;i<(int)inputfiles.size();i++)
  {
    std::cout<<inputfiles[i]<<std::endl;
  }

  std::cout<<"double: "<< dvalue <<std::endl;
  std::cout<<"short: "<<svalue<<std::endl;
  std::cout<<"type: "<<inputType<<std::endl;
  std::cout<<"output: "<<outputfile<<std::endl;
  std::cout<<"inputdir: "<<inputdir<<std::endl;
  std::cout<<"b is set:"<<bset<<std::endl;
  std::cout<<"c is set:"<<cset<<std::endl;
  //processing
  try {
    //start processing
    //end processing
  } catch (...) {
    
  }
  getchar();
  return 0;
}