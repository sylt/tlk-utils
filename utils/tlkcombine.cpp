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
#include <getopt.h>
#include <iostream>
#include <sstream>

#include "libtlk.h"

static std::vector<std::string> SplitNewlines(const std::string& str)
{
  std::stringstream stream(str);
  std::vector<std::string> lines;

  std::string line;
  while (std::getline(stream, line)) {
    lines.emplace_back(line); // Also include empty lines
  }

  return lines;
}

static void SanitizeLine(std::string& line)
{
  if (line.compare(0, 3, "((^") == 0) { // Remove indexes
    line.clear();
    return;
  }
    
  // Remove printf variables
  size_t pos = 0;
  while (true) {
    pos = line.find("%", pos);
    if (pos == std::string::npos) {
      break;
    }

    if ((pos+1) == line.size()) {
      break;
    }

    switch (line[pos+1]) {
    case 's':
    case 'd':
    case 'i':
      line.replace(pos, 2, "XX");
      break;

    default:
      pos++; // Don't find % again
    }
  }
}

const char USAGE[] =
  "Usage: %s [FLAGS] learn-lang.tlk help-lang.tlk output.tlk\n"
  "\n"
  "Combines/interleaves two TLK language files together. If e.g. the learn \n"
  "language is Spanish, and the help language is English, a line could be: \n"
  "\n"
  "  Nos vemos. (See you around.)\n"
  "\n"
  "This can be handy when learning a second language. Available options are:\n"
  "\n"
  "  -l      Warn when line counts doesn't match. This is handy to know\n"
  "          which lines in the file probably need manual editing\n";

static void PrintUsage(const char* programName)
{
  fprintf(stderr, USAGE, programName);
}

int main(int argc, char* argv[])
{
  bool warnOnLineMismatch = false;

  int opt;
  while ((opt = getopt(argc, argv, "l")) != -1) {
    switch (opt) {
    case 'l':
      warnOnLineMismatch = true;
      break;
    default:
      PrintUsage(argv[0]);
      return -1;
    }
  }

  if (optind >= argc || argc - optind < 3) {
    fprintf(stderr, "Expected argument after options\n");
    PrintUsage(argv[0]);
    return -1;
  }

  tlk::FileView learnLang(argv[optind]);
  tlk::FileView helpLang(argv[optind + 1]);
  tlk::Builder builder(learnLang.GetHeader()->LanguageId);

  if (learnLang.GetStringCount() != helpLang.GetStringCount()) {
    printf("INFO: Not all lines will be translated since the learn language "
           "(%s) has %u string, while the help one (%s) has %u\n",
           tlk::GetLanguage(learnLang.GetHeader()->LanguageId),
           learnLang.GetStringCount(),
           tlk::GetLanguage(helpLang.GetHeader()->LanguageId),
           helpLang.GetStringCount());
  }

  const auto stringCount = learnLang.GetStringCount();
  for (uint32_t i = 0; i < stringCount; i++) {
    auto learnElement = learnLang.GetStringElement(i);
    auto learnText = learnLang.GetString(learnElement);
    if (i >= helpLang.GetStringCount()) {
      builder.AddLine(learnElement, learnText); // Line not translated
      continue;
    }

    auto helpElement = helpLang.GetStringElement(i);
    auto helpText = helpLang.GetString(helpElement);

    if (learnText == helpText) {
      builder.AddLine(learnElement, learnText);
      continue; // Text is the same, no need to modify
    }

    SanitizeLine(helpText);
    auto learnLines = SplitNewlines(learnText);
    auto helpLines = SplitNewlines(helpText);

    // If there's just one line, just put the help language at end of
    // sentence.
    if (learnLines.size() == 1) {
      auto newText = std::move(learnText);
      newText += " (";
      newText += std::move(helpText);
      newText += ')';

      builder.AddLine(learnElement, newText);
      continue;
    }

    // If there are several lines, but their count doesn't match, put
    // at end of line, but with newline as separator.
    if (learnLines.size() != helpLines.size()) {
      if (warnOnLineMismatch) {
        fprintf(stderr, "Warning: Line mismatch for entry #%u\n", i);
      }

      auto newText = std::move(learnText);
      newText += "\n(";
      newText += std::move(helpText);
      newText += ")\n";
      
      builder.AddLine(learnElement, newText);
      continue;
    }

    // Line count match, so try and interleave them
    std::string newText;
    for (uint32_t j = 0; j < learnLines.size(); j++) {
      const auto& learnLine = learnLines[j];
      const auto& helpLine = helpLines[j];

      if (warnOnLineMismatch && (learnLine.empty() ^ helpLine.empty())) {
        fprintf(stderr,
                "Warning: Empty/non-empty line combined for entry #%u\n", i);
      }

      newText += learnLine;
      if (learnLine != helpLine) {
        newText += " (";
        newText += helpLine;
        newText += ')';
      }
      newText += '\n';
    }

    builder.AddLine(learnElement, newText);
  }

  const char* outputFile = argv[optind + 2];
  builder.WriteFile(outputFile);

  printf("Done! The file \"%s\" now contains the combined dialog.\n",
         outputFile);
}
