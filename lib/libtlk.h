#ifndef LIB_TLK_H
#define LIB_TLK_H

#include <cinttypes>
#include <string>
#include <vector>
#include <utility>
#include <tuple>

namespace tlk {

struct Header {
  char FileType[4];
  char FileVersion[4];

  uint32_t LanguageId;
  uint32_t StringCount;
  uint32_t StringEntriesOffset;
} __attribute__((packed));

enum class LanguageId : uint32_t {
  ENGLISH = 0,
  FRENCH = 1,
  GERMAN = 2,
  ITALIAN = 3,
  SPANISH = 4,
  POLISH = 5,
  KOREAN = 128,
  CHINESE_TRADITIONAL = 129,
  CHINESE_SIMPLIFIED = 130,
  JAPANESE = 131,
};

const char* GetLanguage(uint32_t languageId);

struct StringDataElement {
  uint32_t Flags;
  char SoundResRef[16];
  uint32_t VolumeVariance;
  uint32_t PitchVariance;
  uint32_t OffsetToString;
  uint32_t StringSize;
  float SoundLength;
} __attribute__((packed));

const auto STRING_FLAG_TEXT_PRESENT = 0x001;
const auto STRING_FLAG_SND_PRESENT = 0x002;
const auto STRING_FLAG_SNDLENGTH_PRESENT = 0x004;

class FileView
{
public:
  FileView(const std::string& path);
  ~FileView();

  const void* GetBuffer() const { return Data; }
  uint64_t GetSize() const { return FileSize; }

  const Header* GetHeader() const { return static_cast<Header*>(Data); }

  uint32_t GetStringCount() const { return GetHeader()->StringCount; }
  inline const StringDataElement* GetStringElement(uint32_t index) const;
  std::string GetString(const StringDataElement* element) const;
  std::tuple<const char*, uint32_t>
  GetCString(const StringDataElement* element) const;

private:
  uint64_t FileSize = 0;
  void* Data = nullptr;
};

inline const StringDataElement* FileView::GetStringElement(uint32_t index) const
{
  const auto buffer = static_cast<const uint8_t*>(GetBuffer());
  return reinterpret_cast<const StringDataElement*>(
    buffer + sizeof(*GetHeader()) + sizeof(StringDataElement) * index);
}

class Builder
{
public:
  Builder(uint32_t languageId);
  Builder(const std::string& path);

  void AddLine(const StringDataElement* elementTemplate, const std::string& newText);
  void ReplaceLine(uint32_t index, const std::string& newText);
  void WriteFile(const std::string& file);

private:
  struct Line
  {
    StringDataElement Template;
    std::string Text;
  };

  std::vector<Line> Lines;
  uint32_t LanguageId;
};

} // namespace tlk

#endif
