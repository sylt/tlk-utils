#include <cassert>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "libtlk.h"

template<typename T>
static T AlignUp(T n, T alignment)
{
  return ((n + alignment - 1) / alignment) * alignment;
}

namespace tlk {

const char* GetLanguage(uint32_t languageId)
{
  auto id = static_cast<LanguageId>(languageId);
  switch (id) {
    case LanguageId::ENGLISH:
      return "English";
    case LanguageId::FRENCH:
      return "French";
    case LanguageId::GERMAN:
      return "German";
    case LanguageId::ITALIAN:
      return "Italian";
    case LanguageId::SPANISH:
      return "Spanish";
    case LanguageId::POLISH:
      return "Polish";
    case LanguageId::KOREAN:
      return "Korean";
    case LanguageId::CHINESE_TRADITIONAL:
      return "Chinese traditional";
    case LanguageId::CHINESE_SIMPLIFIED:
      return "Chinese simplified";
    case LanguageId::JAPANESE:
      return "Japanese";
  }

  return "unknown";
}

FileView::FileView(const std::string& path)
{
  int fd = open(path.c_str(), O_RDONLY);
  if (fd == -1) {
    throw std::runtime_error(
      "Couldn't open file \"" + path + "\": " + strerror(errno));
  }

  struct stat buf;
  fstat(fd, &buf);
  FileSize = AlignUp(buf.st_size, sysconf(_SC_PAGESIZE));

  Data = mmap(nullptr, FileSize, PROT_READ, MAP_SHARED, fd, 0);
  int savedErrno = errno;
  close(fd);

  if (Data == MAP_FAILED) {
    throw std::runtime_error(
      "Couldn't mmap file \"" + path + "\": " + strerror(savedErrno));
  }
}

FileView::~FileView()
{
  munmap(Data, FileSize);
}

std::string FileView::GetString(const StringDataElement* element) const
{
  auto tuple = GetCString(element);
  return {std::get<0>(tuple), std::get<0>(tuple) + std::get<1>(tuple)};
}

std::tuple<const char*, uint32_t>
FileView::GetCString(const StringDataElement* element) const
{
  auto entriesOffset =
    static_cast<const char*>(Data) + GetHeader()->StringEntriesOffset;
  return std::make_tuple(entriesOffset + element->OffsetToString,
                         element->StringSize);
}

Builder::Builder(uint32_t languageId) : LanguageId(languageId) {}

Builder::Builder(const std::string& path)
{
  FileView view(path);
  LanguageId = view.GetHeader()->LanguageId;
  for (uint i = 0; i < view.GetStringCount(); i++) {
    auto element = view.GetStringElement(i);
    AddLine(element, view.GetString(element).c_str());
  }
}

void Builder::ReplaceLine(uint32_t index, const std::string& newText)
{
  Lines.at(index).Text = newText;
}

void Builder::AddLine(const StringDataElement* elementTemplate,
                      const std::string& newText)
{
  assert(elementTemplate != nullptr);
  Lines.push_back({*elementTemplate, newText});
}

void Builder::WriteFile(const std::string& file)
{
  int fd = open(file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0755);
  if (fd == -1) {
    throw std::runtime_error(
      "Couldn't open file \"" + file + "\" for writing: " + strerror(errno));
  }

  std::vector<StringDataElement> elements;
  std::vector<char> text;
  for (auto& line : Lines) {
    elements.emplace_back(line.Template);
    elements.back().OffsetToString = text.size();
    elements.back().StringSize = line.Text.size();
    text.insert(text.end(), line.Text.begin(), line.Text.end());
  }

  Header header = {
    .FileType = {'T', 'L', 'K', ' '},
    .FileVersion = {'V', '3', '.', '0'},
    .LanguageId = LanguageId,
    .StringCount = static_cast<uint32_t>(elements.size()),
    .StringEntriesOffset = static_cast<uint32_t>(
      elements.size() * sizeof(StringDataElement) + sizeof(header)),
  };

  iovec iov[3];
  iov[0].iov_base = &header;
  iov[0].iov_len = sizeof(header);
  iov[1].iov_base = elements.data();
  iov[1].iov_len = elements.size() * sizeof(*elements.data());
  iov[2].iov_base = text.data();
  iov[2].iov_len = text.size();

  ssize_t bytesWritten = writev(fd, iov, 3);
  int savedErrno = errno;
  close(fd);

  if (bytesWritten == -1) {
    throw std::runtime_error(
      "Couldn't write to file \"" + file + "\": " + strerror(savedErrno));
  }

  auto expectedBytesWritten = iov[0].iov_len + iov[1].iov_len + iov[2].iov_len;
  if (bytesWritten != static_cast<ssize_t>(expectedBytesWritten)) {
    throw std::runtime_error(
      "Only wrote " + std::to_string(bytesWritten) + "/" + std::to_string(expectedBytesWritten));
  }
}

} // namespace tlk
