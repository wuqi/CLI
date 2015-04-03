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
  //�����г���

  ezOptionParser opt;
  //��Ҫ˵��
  opt.overview = "sbi:Make the Soil brightness index product.";
  //�﷨,����˵�����Զ�����
  //opt.syntax = "cli [-h] [-e|-f]  [-t modis|fy3a] -i Arg1,[ArgN] -o Arg";
  //ʵ��
  opt.example = "./sbi.exe -i MODIS.1000.2013197031000.H27V07.000000,MODIS.1000.2013197044500.H27V07.000000 -o MuSQ.SBI.1km..2013197000000.H27V05.001.h5\n\n";
  ////����������ѡ��  help ����Զ����ã�����������
  opt.add (
    "10", // Ĭ��ֵ,��������Ϊ��.
    0, // �Ƿ�Ϊ����?����Ϊ1
    1, // ָ������������-1���ڴ����б�,ָ��-1ʱ������Ҫ1������.
    "Test short args.", // Usage����ʾ����Ϣ.
    "-s",     // Flag token.
    "--short", // Flag token.
    EZ_INT16 //ָ���������ͣ�������Խ��ʱ����ʾ����ֹͣ����
  );
  opt.add (
      "77", // Ĭ��ֵ,��������Ϊ��.
      0, // �Ƿ�Ϊ����?����Ϊ1
      2, // ָ������������-1Ϊ��ָ�������ڴ����б�.
      "Test max range,10-100.", // Usage����ʾ����Ϣ.
      "-d",     // Flag token.
      "--double", // Flag token.
      ez::EZ_DOUBLE,
      "10",
      "100"
      );
  //����ָ���б�����ֵ
  std::vector<std::string> validValues;
  validValues.push_back("modis");
  validValues.push_back("fy3a");
  opt.add (
      "", // Ĭ��ֵ,��������Ϊ��.
      0, // �Ƿ�Ϊ����?����Ϊ1
      1, // ָ������������-1Ϊ���ڴ����б�,�б�������һ������.
      "Input file type,modis or fy3a, default is modis.", // Usage����ʾ����Ϣ.
      "-t",     // Flag token.
      "--type", // Flag token.
      ez::EZ_TEXT,//ָ������
      "",//��Сֵ
      "",//���ֵ
      validValues//����ֵ
      );
  opt.add (
    "", // Default.
    1, // ����
    -1, // ָ����������
    "Input files arguments,test argument list.", // Help description.
    "-i",     // Flag token.
    "--inputs", // Flag token.
    ez::EZ_TEXT //text���Բ�ָ������
  );
  opt.add (
    "", // Default.
    1, // ����
    1, // �������Ϊ1��.
    "Output files arguments.", // Help description.
    "-o",     // Flag token.
    "--outputs", // Flag token.
    ez::EZ_TEXT
  );
  //test xor
  opt.xorAdd("-d","-s");
  //����������ѡ��
  opt.parse (argc, argv);
  //��ʾ����
  if (opt.isSet("-h"))
  {
    std::string usage;
    opt.getUsage(usage);
    std::cout<<usage;
    return 0;
  }
  std::string out;
  //�����������Ƿ���ȷ������ȷ�򷵻�1��ͬʱ��ʾusage
  if (!opt.checkValid(out)) {
    std::cout << out;
    return 1;
  }
  std::cout << out;
  //��ȡ����,ʹ��get��ȡ��������
  //ʹ��getVector��ȡ�������������vector�б�
  //ʹ��getMultiVector��ȡ���ѡ�����е�vector�б�
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