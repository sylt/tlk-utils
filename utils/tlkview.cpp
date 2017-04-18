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

#include <cstdio>
#include <getopt.h>
#include <limits>

#include "libtlk.h"

const uint32_t NO_INDEX_SELECTED = std::numeric_limits<uint32_t>::max();

const char USAGE[] =
  "Usage: %s [OPTION]... tlkfile\n"
  "\n"
  "List entry information of a TLK file. Available options are:\n"
  "\n"
  "  -e,--entry=INDEX    Print text of specific entry.\n";

void PrintUsage(const char* programName)
{
  fprintf(stderr, USAGE, programName);
}

int main(int argc, char* argv[])
{
  uint32_t indexToPrint = NO_INDEX_SELECTED;
  int opt;
  while ((opt = getopt(argc, argv, "e:")) != -1) {
    switch (opt) {
    case 'e':
      indexToPrint = std::stoul(optarg);
      break;
    default:
      PrintUsage(argv[0]);
      return -1;
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "Expected argument after options\n");
    PrintUsage(argv[0]);
    return -1;
  }

  tlk::FileView tlkFile(argv[optind]);
  const auto header = tlkFile.GetHeader();

  if (indexToPrint != NO_INDEX_SELECTED) {
    if (indexToPrint >= header->StringCount) {
      fprintf(stderr, "Index is out-of-bounds\n");
      return -1;
    }
    
    auto element = tlkFile.GetStringElement(indexToPrint);
    printf("%s", tlkFile.GetString(element).c_str());
    return 0;
  }

  printf("Header: %.4s\n", header->FileType);
  printf("Version: %.4s\n", header->FileVersion);
  printf("Language ID: %u\n", header->LanguageId);
  printf("String Count: %u\n", header->StringCount);
  printf("String Entries Offset: %u\n", header->StringEntriesOffset);

  for (uint i = 0; i < tlkFile.GetStringCount(); i++) {
    auto element = tlkFile.GetStringElement(i);
    printf("Element #%d:\n", i);
    printf("....... Flags: 0x%x\n", element->Flags);
    printf("....... SoundResRef: %.16s\n", element->SoundResRef);
    printf("....... VolumeVariance: %u\n", element->VolumeVariance);
    printf("....... PitchVariance: %u\n", element->PitchVariance);
    printf("....... OffsetToString: %u\n", element->OffsetToString);
    printf("....... StringSize: 0x%x\n", element->StringSize);
    printf("....... SoundLength: %f\n", element->SoundLength);
    printf("....... Content: \"%s\"\n\n", tlkFile.GetString(element).c_str());
  }
}
