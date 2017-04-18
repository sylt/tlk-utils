// MIT License
//
// Copyright (c) 2017 sylt
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
