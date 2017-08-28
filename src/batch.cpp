#include <iostream>
#include "batch.hpp"

int main(int argc, char **argv) {
  //example
  batch::Glob glob("D:/Foobar2000/Equalizer Presets/S*.feq");
  while (glob) {
    //glob.GetFileName(false)只返回文件名，不带路径 
    std::cout << glob.GetFileName() << std::endl;
    glob.Next();
  }

  return 0;
}