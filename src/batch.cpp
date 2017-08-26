#include <iostream>
#include "batch.hpp"

int main(int argc, char **argv) {
  batch::Glob glob("D:/Foobar2000/Equalizer Presets/S*.feq");
  while (glob) {
    std::cout << glob.GetFileName() << std::endl;
    glob.Next();
  }

  return 0;
}