#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include "libtlk.h"

static std::string ReadFileContents(const char* path)
{
  std::ifstream file(path);
  std::string newText((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  return newText;
}

int main(int argc, char* argv[])
{
  if (argc != 4) {
    fprintf(stderr,
            "Usage: %s tlkfile index-to-replace file-with-new-text\n",
            argv[0]);
    return -1;
  }

  tlk::Builder builder(argv[1]);
  auto index = std::stoul(argv[2]);
  builder.ReplaceLine(index, ReadFileContents(argv[3]));
  builder.WriteFile(argv[1]);

  printf("Line updated! Ensure update was correct by using tlkview.\n");
}
