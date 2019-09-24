// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_HIPPOMESSAGES_H_
#define FLATBUFFERS_GENERATED_HIPPOMESSAGES_H_

#include "flatbuffers/flatbuffers.h"

struct HippoNextSong;

struct HippoPrevSong;

struct HippoPlaySong;

struct HippoStopSong;

struct HippoRequestSelectSong;

struct HippoRequestAddUrls;

struct HippoUrlEntry;

struct HippoSelectSong;

struct HippoReplyAddedUrls;

struct HippoRequestTrackerData;

struct HippoRowData;

struct HippoTrackerChannel;

struct HippoTrackerData;

struct HippoCurrentPosition;

struct HippoMessage;

enum HippoTrackerType {
  HippoTrackerType_Regular = 0,
  HippoTrackerType_Emulated = 1,
  HippoTrackerType_MIN = HippoTrackerType_Regular,
  HippoTrackerType_MAX = HippoTrackerType_Emulated
};

inline const HippoTrackerType (&EnumValuesHippoTrackerType())[2] {
  static const HippoTrackerType values[] = {
    HippoTrackerType_Regular,
    HippoTrackerType_Emulated
  };
  return values;
}

inline const char * const *EnumNamesHippoTrackerType() {
  static const char * const names[3] = {
    "Regular",
    "Emulated",
    nullptr
  };
  return names;
}

inline const char *EnumNameHippoTrackerType(HippoTrackerType e) {
  if (e < HippoTrackerType_Regular || e > HippoTrackerType_Emulated) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesHippoTrackerType()[index];
}

enum MessageType {
  MessageType_NONE = 0,
  MessageType_next_song = 1,
  MessageType_prev_song = 2,
  MessageType_play_song = 3,
  MessageType_stop_song = 4,
  MessageType_request_select_song = 5,
  MessageType_select_song = 6,
  MessageType_request_add_urls = 7,
  MessageType_reply_added_urls = 8,
  MessageType_request_tracker_data = 9,
  MessageType_tracker_data = 10,
  MessageType_current_position = 11,
  MessageType_MIN = MessageType_NONE,
  MessageType_MAX = MessageType_current_position
};

inline const MessageType (&EnumValuesMessageType())[12] {
  static const MessageType values[] = {
    MessageType_NONE,
    MessageType_next_song,
    MessageType_prev_song,
    MessageType_play_song,
    MessageType_stop_song,
    MessageType_request_select_song,
    MessageType_select_song,
    MessageType_request_add_urls,
    MessageType_reply_added_urls,
    MessageType_request_tracker_data,
    MessageType_tracker_data,
    MessageType_current_position
  };
  return values;
}

inline const char * const *EnumNamesMessageType() {
  static const char * const names[13] = {
    "NONE",
    "next_song",
    "prev_song",
    "play_song",
    "stop_song",
    "request_select_song",
    "select_song",
    "request_add_urls",
    "reply_added_urls",
    "request_tracker_data",
    "tracker_data",
    "current_position",
    nullptr
  };
  return names;
}

inline const char *EnumNameMessageType(MessageType e) {
  if (e < MessageType_NONE || e > MessageType_current_position) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesMessageType()[index];
}

bool VerifyMessageType(flatbuffers::Verifier &verifier, const void *obj, MessageType type);
bool VerifyMessageTypeVector(flatbuffers::Verifier &verifier, const flatbuffers::Vector<flatbuffers::Offset<void>> *values, const flatbuffers::Vector<uint8_t> *types);

struct HippoNextSong FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           verifier.EndTable();
  }
};

struct HippoNextSongBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  explicit HippoNextSongBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  HippoNextSongBuilder &operator=(const HippoNextSongBuilder &);
  flatbuffers::Offset<HippoNextSong> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<HippoNextSong>(end);
    return o;
  }
};

inline flatbuffers::Offset<HippoNextSong> CreateHippoNextSong(
    flatbuffers::FlatBufferBuilder &_fbb) {
  HippoNextSongBuilder builder_(_fbb);
  return builder_.Finish();
}

struct HippoPrevSong FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           verifier.EndTable();
  }
};

struct HippoPrevSongBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  explicit HippoPrevSongBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  HippoPrevSongBuilder &operator=(const HippoPrevSongBuilder &);
  flatbuffers::Offset<HippoPrevSong> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<HippoPrevSong>(end);
    return o;
  }
};

inline flatbuffers::Offset<HippoPrevSong> CreateHippoPrevSong(
    flatbuffers::FlatBufferBuilder &_fbb) {
  HippoPrevSongBuilder builder_(_fbb);
  return builder_.Finish();
}

struct HippoPlaySong FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           verifier.EndTable();
  }
};

struct HippoPlaySongBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  explicit HippoPlaySongBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  HippoPlaySongBuilder &operator=(const HippoPlaySongBuilder &);
  flatbuffers::Offset<HippoPlaySong> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<HippoPlaySong>(end);
    return o;
  }
};

inline flatbuffers::Offset<HippoPlaySong> CreateHippoPlaySong(
    flatbuffers::FlatBufferBuilder &_fbb) {
  HippoPlaySongBuilder builder_(_fbb);
  return builder_.Finish();
}

struct HippoStopSong FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           verifier.EndTable();
  }
};

struct HippoStopSongBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  explicit HippoStopSongBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  HippoStopSongBuilder &operator=(const HippoStopSongBuilder &);
  flatbuffers::Offset<HippoStopSong> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<HippoStopSong>(end);
    return o;
  }
};

inline flatbuffers::Offset<HippoStopSong> CreateHippoStopSong(
    flatbuffers::FlatBufferBuilder &_fbb) {
  HippoStopSongBuilder builder_(_fbb);
  return builder_.Finish();
}

struct HippoRequestSelectSong FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_NAME = 4
  };
  const flatbuffers::String *name() const {
    return GetPointer<const flatbuffers::String *>(VT_NAME);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           verifier.EndTable();
  }
};

struct HippoRequestSelectSongBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_name(flatbuffers::Offset<flatbuffers::String> name) {
    fbb_.AddOffset(HippoRequestSelectSong::VT_NAME, name);
  }
  explicit HippoRequestSelectSongBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  HippoRequestSelectSongBuilder &operator=(const HippoRequestSelectSongBuilder &);
  flatbuffers::Offset<HippoRequestSelectSong> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<HippoRequestSelectSong>(end);
    return o;
  }
};

inline flatbuffers::Offset<HippoRequestSelectSong> CreateHippoRequestSelectSong(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> name = 0) {
  HippoRequestSelectSongBuilder builder_(_fbb);
  builder_.add_name(name);
  return builder_.Finish();
}

inline flatbuffers::Offset<HippoRequestSelectSong> CreateHippoRequestSelectSongDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *name = nullptr) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  return CreateHippoRequestSelectSong(
      _fbb,
      name__);
}

struct HippoRequestAddUrls FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_URLS = 4
  };
  const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *urls() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *>(VT_URLS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_URLS) &&
           verifier.VerifyVector(urls()) &&
           verifier.VerifyVectorOfStrings(urls()) &&
           verifier.EndTable();
  }
};

struct HippoRequestAddUrlsBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_urls(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> urls) {
    fbb_.AddOffset(HippoRequestAddUrls::VT_URLS, urls);
  }
  explicit HippoRequestAddUrlsBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  HippoRequestAddUrlsBuilder &operator=(const HippoRequestAddUrlsBuilder &);
  flatbuffers::Offset<HippoRequestAddUrls> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<HippoRequestAddUrls>(end);
    return o;
  }
};

inline flatbuffers::Offset<HippoRequestAddUrls> CreateHippoRequestAddUrls(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> urls = 0) {
  HippoRequestAddUrlsBuilder builder_(_fbb);
  builder_.add_urls(urls);
  return builder_.Finish();
}

inline flatbuffers::Offset<HippoRequestAddUrls> CreateHippoRequestAddUrlsDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<flatbuffers::Offset<flatbuffers::String>> *urls = nullptr) {
  auto urls__ = urls ? _fbb.CreateVector<flatbuffers::Offset<flatbuffers::String>>(*urls) : 0;
  return CreateHippoRequestAddUrls(
      _fbb,
      urls__);
}

struct HippoUrlEntry FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_PATH = 4,
    VT_TITLE = 6,
    VT_LENGTH = 8
  };
  const flatbuffers::String *path() const {
    return GetPointer<const flatbuffers::String *>(VT_PATH);
  }
  const flatbuffers::String *title() const {
    return GetPointer<const flatbuffers::String *>(VT_TITLE);
  }
  float length() const {
    return GetField<float>(VT_LENGTH, 0.0f);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_PATH) &&
           verifier.VerifyString(path()) &&
           VerifyOffset(verifier, VT_TITLE) &&
           verifier.VerifyString(title()) &&
           VerifyField<float>(verifier, VT_LENGTH) &&
           verifier.EndTable();
  }
};

struct HippoUrlEntryBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_path(flatbuffers::Offset<flatbuffers::String> path) {
    fbb_.AddOffset(HippoUrlEntry::VT_PATH, path);
  }
  void add_title(flatbuffers::Offset<flatbuffers::String> title) {
    fbb_.AddOffset(HippoUrlEntry::VT_TITLE, title);
  }
  void add_length(float length) {
    fbb_.AddElement<float>(HippoUrlEntry::VT_LENGTH, length, 0.0f);
  }
  explicit HippoUrlEntryBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  HippoUrlEntryBuilder &operator=(const HippoUrlEntryBuilder &);
  flatbuffers::Offset<HippoUrlEntry> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<HippoUrlEntry>(end);
    return o;
  }
};

inline flatbuffers::Offset<HippoUrlEntry> CreateHippoUrlEntry(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> path = 0,
    flatbuffers::Offset<flatbuffers::String> title = 0,
    float length = 0.0f) {
  HippoUrlEntryBuilder builder_(_fbb);
  builder_.add_length(length);
  builder_.add_title(title);
  builder_.add_path(path);
  return builder_.Finish();
}

inline flatbuffers::Offset<HippoUrlEntry> CreateHippoUrlEntryDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *path = nullptr,
    const char *title = nullptr,
    float length = 0.0f) {
  auto path__ = path ? _fbb.CreateString(path) : 0;
  auto title__ = title ? _fbb.CreateString(title) : 0;
  return CreateHippoUrlEntry(
      _fbb,
      path__,
      title__,
      length);
}

struct HippoSelectSong FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_TITLE = 4
  };
  const flatbuffers::String *title() const {
    return GetPointer<const flatbuffers::String *>(VT_TITLE);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_TITLE) &&
           verifier.VerifyString(title()) &&
           verifier.EndTable();
  }
};

struct HippoSelectSongBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_title(flatbuffers::Offset<flatbuffers::String> title) {
    fbb_.AddOffset(HippoSelectSong::VT_TITLE, title);
  }
  explicit HippoSelectSongBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  HippoSelectSongBuilder &operator=(const HippoSelectSongBuilder &);
  flatbuffers::Offset<HippoSelectSong> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<HippoSelectSong>(end);
    return o;
  }
};

inline flatbuffers::Offset<HippoSelectSong> CreateHippoSelectSong(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> title = 0) {
  HippoSelectSongBuilder builder_(_fbb);
  builder_.add_title(title);
  return builder_.Finish();
}

inline flatbuffers::Offset<HippoSelectSong> CreateHippoSelectSongDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *title = nullptr) {
  auto title__ = title ? _fbb.CreateString(title) : 0;
  return CreateHippoSelectSong(
      _fbb,
      title__);
}

struct HippoReplyAddedUrls FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_URLS = 4
  };
  const flatbuffers::Vector<flatbuffers::Offset<HippoUrlEntry>> *urls() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<HippoUrlEntry>> *>(VT_URLS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_URLS) &&
           verifier.VerifyVector(urls()) &&
           verifier.VerifyVectorOfTables(urls()) &&
           verifier.EndTable();
  }
};

struct HippoReplyAddedUrlsBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_urls(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<HippoUrlEntry>>> urls) {
    fbb_.AddOffset(HippoReplyAddedUrls::VT_URLS, urls);
  }
  explicit HippoReplyAddedUrlsBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  HippoReplyAddedUrlsBuilder &operator=(const HippoReplyAddedUrlsBuilder &);
  flatbuffers::Offset<HippoReplyAddedUrls> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<HippoReplyAddedUrls>(end);
    return o;
  }
};

inline flatbuffers::Offset<HippoReplyAddedUrls> CreateHippoReplyAddedUrls(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<HippoUrlEntry>>> urls = 0) {
  HippoReplyAddedUrlsBuilder builder_(_fbb);
  builder_.add_urls(urls);
  return builder_.Finish();
}

inline flatbuffers::Offset<HippoReplyAddedUrls> CreateHippoReplyAddedUrlsDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<flatbuffers::Offset<HippoUrlEntry>> *urls = nullptr) {
  auto urls__ = urls ? _fbb.CreateVector<flatbuffers::Offset<HippoUrlEntry>>(*urls) : 0;
  return CreateHippoReplyAddedUrls(
      _fbb,
      urls__);
}

struct HippoRequestTrackerData FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_PATTERN = 4
  };
  int32_t pattern() const {
    return GetField<int32_t>(VT_PATTERN, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_PATTERN) &&
           verifier.EndTable();
  }
};

struct HippoRequestTrackerDataBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_pattern(int32_t pattern) {
    fbb_.AddElement<int32_t>(HippoRequestTrackerData::VT_PATTERN, pattern, 0);
  }
  explicit HippoRequestTrackerDataBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  HippoRequestTrackerDataBuilder &operator=(const HippoRequestTrackerDataBuilder &);
  flatbuffers::Offset<HippoRequestTrackerData> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<HippoRequestTrackerData>(end);
    return o;
  }
};

inline flatbuffers::Offset<HippoRequestTrackerData> CreateHippoRequestTrackerData(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t pattern = 0) {
  HippoRequestTrackerDataBuilder builder_(_fbb);
  builder_.add_pattern(pattern);
  return builder_.Finish();
}

struct HippoRowData FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_NOTE = 4,
    VT_INSTRUMENT = 6,
    VT_VOLUMEFFECT = 8,
    VT_EFFECT = 10,
    VT_VOLUME = 12,
    VT_PARAMETER = 14
  };
  const flatbuffers::String *note() const {
    return GetPointer<const flatbuffers::String *>(VT_NOTE);
  }
  const flatbuffers::String *instrument() const {
    return GetPointer<const flatbuffers::String *>(VT_INSTRUMENT);
  }
  const flatbuffers::String *volumeffect() const {
    return GetPointer<const flatbuffers::String *>(VT_VOLUMEFFECT);
  }
  const flatbuffers::String *effect() const {
    return GetPointer<const flatbuffers::String *>(VT_EFFECT);
  }
  const flatbuffers::String *volume() const {
    return GetPointer<const flatbuffers::String *>(VT_VOLUME);
  }
  const flatbuffers::String *parameter() const {
    return GetPointer<const flatbuffers::String *>(VT_PARAMETER);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_NOTE) &&
           verifier.VerifyString(note()) &&
           VerifyOffset(verifier, VT_INSTRUMENT) &&
           verifier.VerifyString(instrument()) &&
           VerifyOffset(verifier, VT_VOLUMEFFECT) &&
           verifier.VerifyString(volumeffect()) &&
           VerifyOffset(verifier, VT_EFFECT) &&
           verifier.VerifyString(effect()) &&
           VerifyOffset(verifier, VT_VOLUME) &&
           verifier.VerifyString(volume()) &&
           VerifyOffset(verifier, VT_PARAMETER) &&
           verifier.VerifyString(parameter()) &&
           verifier.EndTable();
  }
};

struct HippoRowDataBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_note(flatbuffers::Offset<flatbuffers::String> note) {
    fbb_.AddOffset(HippoRowData::VT_NOTE, note);
  }
  void add_instrument(flatbuffers::Offset<flatbuffers::String> instrument) {
    fbb_.AddOffset(HippoRowData::VT_INSTRUMENT, instrument);
  }
  void add_volumeffect(flatbuffers::Offset<flatbuffers::String> volumeffect) {
    fbb_.AddOffset(HippoRowData::VT_VOLUMEFFECT, volumeffect);
  }
  void add_effect(flatbuffers::Offset<flatbuffers::String> effect) {
    fbb_.AddOffset(HippoRowData::VT_EFFECT, effect);
  }
  void add_volume(flatbuffers::Offset<flatbuffers::String> volume) {
    fbb_.AddOffset(HippoRowData::VT_VOLUME, volume);
  }
  void add_parameter(flatbuffers::Offset<flatbuffers::String> parameter) {
    fbb_.AddOffset(HippoRowData::VT_PARAMETER, parameter);
  }
  explicit HippoRowDataBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  HippoRowDataBuilder &operator=(const HippoRowDataBuilder &);
  flatbuffers::Offset<HippoRowData> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<HippoRowData>(end);
    return o;
  }
};

inline flatbuffers::Offset<HippoRowData> CreateHippoRowData(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> note = 0,
    flatbuffers::Offset<flatbuffers::String> instrument = 0,
    flatbuffers::Offset<flatbuffers::String> volumeffect = 0,
    flatbuffers::Offset<flatbuffers::String> effect = 0,
    flatbuffers::Offset<flatbuffers::String> volume = 0,
    flatbuffers::Offset<flatbuffers::String> parameter = 0) {
  HippoRowDataBuilder builder_(_fbb);
  builder_.add_parameter(parameter);
  builder_.add_volume(volume);
  builder_.add_effect(effect);
  builder_.add_volumeffect(volumeffect);
  builder_.add_instrument(instrument);
  builder_.add_note(note);
  return builder_.Finish();
}

inline flatbuffers::Offset<HippoRowData> CreateHippoRowDataDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *note = nullptr,
    const char *instrument = nullptr,
    const char *volumeffect = nullptr,
    const char *effect = nullptr,
    const char *volume = nullptr,
    const char *parameter = nullptr) {
  auto note__ = note ? _fbb.CreateString(note) : 0;
  auto instrument__ = instrument ? _fbb.CreateString(instrument) : 0;
  auto volumeffect__ = volumeffect ? _fbb.CreateString(volumeffect) : 0;
  auto effect__ = effect ? _fbb.CreateString(effect) : 0;
  auto volume__ = volume ? _fbb.CreateString(volume) : 0;
  auto parameter__ = parameter ? _fbb.CreateString(parameter) : 0;
  return CreateHippoRowData(
      _fbb,
      note__,
      instrument__,
      volumeffect__,
      effect__,
      volume__,
      parameter__);
}

struct HippoTrackerChannel FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ROW_DATA = 4
  };
  const flatbuffers::Vector<flatbuffers::Offset<HippoRowData>> *row_data() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<HippoRowData>> *>(VT_ROW_DATA);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_ROW_DATA) &&
           verifier.VerifyVector(row_data()) &&
           verifier.VerifyVectorOfTables(row_data()) &&
           verifier.EndTable();
  }
};

struct HippoTrackerChannelBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_row_data(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<HippoRowData>>> row_data) {
    fbb_.AddOffset(HippoTrackerChannel::VT_ROW_DATA, row_data);
  }
  explicit HippoTrackerChannelBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  HippoTrackerChannelBuilder &operator=(const HippoTrackerChannelBuilder &);
  flatbuffers::Offset<HippoTrackerChannel> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<HippoTrackerChannel>(end);
    return o;
  }
};

inline flatbuffers::Offset<HippoTrackerChannel> CreateHippoTrackerChannel(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<HippoRowData>>> row_data = 0) {
  HippoTrackerChannelBuilder builder_(_fbb);
  builder_.add_row_data(row_data);
  return builder_.Finish();
}

inline flatbuffers::Offset<HippoTrackerChannel> CreateHippoTrackerChannelDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<flatbuffers::Offset<HippoRowData>> *row_data = nullptr) {
  auto row_data__ = row_data ? _fbb.CreateVector<flatbuffers::Offset<HippoRowData>>(*row_data) : 0;
  return CreateHippoTrackerChannel(
      _fbb,
      row_data__);
}

struct HippoTrackerData FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_TYPE = 4,
    VT_PATTERN = 6,
    VT_CURRENT_ROW = 8,
    VT_CHANNELS = 10
  };
  HippoTrackerType type() const {
    return static_cast<HippoTrackerType>(GetField<int8_t>(VT_TYPE, 0));
  }
  int32_t pattern() const {
    return GetField<int32_t>(VT_PATTERN, 0);
  }
  int32_t current_row() const {
    return GetField<int32_t>(VT_CURRENT_ROW, 0);
  }
  const flatbuffers::Vector<flatbuffers::Offset<HippoTrackerChannel>> *channels() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<HippoTrackerChannel>> *>(VT_CHANNELS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int8_t>(verifier, VT_TYPE) &&
           VerifyField<int32_t>(verifier, VT_PATTERN) &&
           VerifyField<int32_t>(verifier, VT_CURRENT_ROW) &&
           VerifyOffset(verifier, VT_CHANNELS) &&
           verifier.VerifyVector(channels()) &&
           verifier.VerifyVectorOfTables(channels()) &&
           verifier.EndTable();
  }
};

struct HippoTrackerDataBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_type(HippoTrackerType type) {
    fbb_.AddElement<int8_t>(HippoTrackerData::VT_TYPE, static_cast<int8_t>(type), 0);
  }
  void add_pattern(int32_t pattern) {
    fbb_.AddElement<int32_t>(HippoTrackerData::VT_PATTERN, pattern, 0);
  }
  void add_current_row(int32_t current_row) {
    fbb_.AddElement<int32_t>(HippoTrackerData::VT_CURRENT_ROW, current_row, 0);
  }
  void add_channels(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<HippoTrackerChannel>>> channels) {
    fbb_.AddOffset(HippoTrackerData::VT_CHANNELS, channels);
  }
  explicit HippoTrackerDataBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  HippoTrackerDataBuilder &operator=(const HippoTrackerDataBuilder &);
  flatbuffers::Offset<HippoTrackerData> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<HippoTrackerData>(end);
    return o;
  }
};

inline flatbuffers::Offset<HippoTrackerData> CreateHippoTrackerData(
    flatbuffers::FlatBufferBuilder &_fbb,
    HippoTrackerType type = HippoTrackerType_Regular,
    int32_t pattern = 0,
    int32_t current_row = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<HippoTrackerChannel>>> channels = 0) {
  HippoTrackerDataBuilder builder_(_fbb);
  builder_.add_channels(channels);
  builder_.add_current_row(current_row);
  builder_.add_pattern(pattern);
  builder_.add_type(type);
  return builder_.Finish();
}

inline flatbuffers::Offset<HippoTrackerData> CreateHippoTrackerDataDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    HippoTrackerType type = HippoTrackerType_Regular,
    int32_t pattern = 0,
    int32_t current_row = 0,
    const std::vector<flatbuffers::Offset<HippoTrackerChannel>> *channels = nullptr) {
  auto channels__ = channels ? _fbb.CreateVector<flatbuffers::Offset<HippoTrackerChannel>>(*channels) : 0;
  return CreateHippoTrackerData(
      _fbb,
      type,
      pattern,
      current_row,
      channels__);
}

struct HippoCurrentPosition FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_POSITION = 4,
    VT_CURRENT_PATTERN = 6,
    VT_CURRENT_ROW = 8,
    VT_CURRENT_SPEED = 10,
    VT_DURATION = 12
  };
  float position() const {
    return GetField<float>(VT_POSITION, 0.0f);
  }
  int32_t current_pattern() const {
    return GetField<int32_t>(VT_CURRENT_PATTERN, 0);
  }
  int32_t current_row() const {
    return GetField<int32_t>(VT_CURRENT_ROW, 0);
  }
  int32_t current_speed() const {
    return GetField<int32_t>(VT_CURRENT_SPEED, 0);
  }
  float duration() const {
    return GetField<float>(VT_DURATION, 0.0f);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<float>(verifier, VT_POSITION) &&
           VerifyField<int32_t>(verifier, VT_CURRENT_PATTERN) &&
           VerifyField<int32_t>(verifier, VT_CURRENT_ROW) &&
           VerifyField<int32_t>(verifier, VT_CURRENT_SPEED) &&
           VerifyField<float>(verifier, VT_DURATION) &&
           verifier.EndTable();
  }
};

struct HippoCurrentPositionBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_position(float position) {
    fbb_.AddElement<float>(HippoCurrentPosition::VT_POSITION, position, 0.0f);
  }
  void add_current_pattern(int32_t current_pattern) {
    fbb_.AddElement<int32_t>(HippoCurrentPosition::VT_CURRENT_PATTERN, current_pattern, 0);
  }
  void add_current_row(int32_t current_row) {
    fbb_.AddElement<int32_t>(HippoCurrentPosition::VT_CURRENT_ROW, current_row, 0);
  }
  void add_current_speed(int32_t current_speed) {
    fbb_.AddElement<int32_t>(HippoCurrentPosition::VT_CURRENT_SPEED, current_speed, 0);
  }
  void add_duration(float duration) {
    fbb_.AddElement<float>(HippoCurrentPosition::VT_DURATION, duration, 0.0f);
  }
  explicit HippoCurrentPositionBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  HippoCurrentPositionBuilder &operator=(const HippoCurrentPositionBuilder &);
  flatbuffers::Offset<HippoCurrentPosition> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<HippoCurrentPosition>(end);
    return o;
  }
};

inline flatbuffers::Offset<HippoCurrentPosition> CreateHippoCurrentPosition(
    flatbuffers::FlatBufferBuilder &_fbb,
    float position = 0.0f,
    int32_t current_pattern = 0,
    int32_t current_row = 0,
    int32_t current_speed = 0,
    float duration = 0.0f) {
  HippoCurrentPositionBuilder builder_(_fbb);
  builder_.add_duration(duration);
  builder_.add_current_speed(current_speed);
  builder_.add_current_row(current_row);
  builder_.add_current_pattern(current_pattern);
  builder_.add_position(position);
  return builder_.Finish();
}

struct HippoMessage FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_MESSAGE_TYPE = 4,
    VT_MESSAGE = 6,
    VT_USER_DATA = 8
  };
  MessageType message_type() const {
    return static_cast<MessageType>(GetField<uint8_t>(VT_MESSAGE_TYPE, 0));
  }
  const void *message() const {
    return GetPointer<const void *>(VT_MESSAGE);
  }
  const HippoNextSong *message_as_next_song() const {
    return message_type() == MessageType_next_song ? static_cast<const HippoNextSong *>(message()) : nullptr;
  }
  const HippoPrevSong *message_as_prev_song() const {
    return message_type() == MessageType_prev_song ? static_cast<const HippoPrevSong *>(message()) : nullptr;
  }
  const HippoPlaySong *message_as_play_song() const {
    return message_type() == MessageType_play_song ? static_cast<const HippoPlaySong *>(message()) : nullptr;
  }
  const HippoStopSong *message_as_stop_song() const {
    return message_type() == MessageType_stop_song ? static_cast<const HippoStopSong *>(message()) : nullptr;
  }
  const HippoRequestSelectSong *message_as_request_select_song() const {
    return message_type() == MessageType_request_select_song ? static_cast<const HippoRequestSelectSong *>(message()) : nullptr;
  }
  const HippoSelectSong *message_as_select_song() const {
    return message_type() == MessageType_select_song ? static_cast<const HippoSelectSong *>(message()) : nullptr;
  }
  const HippoRequestAddUrls *message_as_request_add_urls() const {
    return message_type() == MessageType_request_add_urls ? static_cast<const HippoRequestAddUrls *>(message()) : nullptr;
  }
  const HippoReplyAddedUrls *message_as_reply_added_urls() const {
    return message_type() == MessageType_reply_added_urls ? static_cast<const HippoReplyAddedUrls *>(message()) : nullptr;
  }
  const HippoRequestSelectSong *message_as_request_tracker_data() const {
    return message_type() == MessageType_request_tracker_data ? static_cast<const HippoRequestSelectSong *>(message()) : nullptr;
  }
  const HippoTrackerData *message_as_tracker_data() const {
    return message_type() == MessageType_tracker_data ? static_cast<const HippoTrackerData *>(message()) : nullptr;
  }
  const HippoCurrentPosition *message_as_current_position() const {
    return message_type() == MessageType_current_position ? static_cast<const HippoCurrentPosition *>(message()) : nullptr;
  }
  const flatbuffers::String *user_data() const {
    return GetPointer<const flatbuffers::String *>(VT_USER_DATA);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint8_t>(verifier, VT_MESSAGE_TYPE) &&
           VerifyOffset(verifier, VT_MESSAGE) &&
           VerifyMessageType(verifier, message(), message_type()) &&
           VerifyOffset(verifier, VT_USER_DATA) &&
           verifier.VerifyString(user_data()) &&
           verifier.EndTable();
  }
};

struct HippoMessageBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_message_type(MessageType message_type) {
    fbb_.AddElement<uint8_t>(HippoMessage::VT_MESSAGE_TYPE, static_cast<uint8_t>(message_type), 0);
  }
  void add_message(flatbuffers::Offset<void> message) {
    fbb_.AddOffset(HippoMessage::VT_MESSAGE, message);
  }
  void add_user_data(flatbuffers::Offset<flatbuffers::String> user_data) {
    fbb_.AddOffset(HippoMessage::VT_USER_DATA, user_data);
  }
  explicit HippoMessageBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  HippoMessageBuilder &operator=(const HippoMessageBuilder &);
  flatbuffers::Offset<HippoMessage> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<HippoMessage>(end);
    return o;
  }
};

inline flatbuffers::Offset<HippoMessage> CreateHippoMessage(
    flatbuffers::FlatBufferBuilder &_fbb,
    MessageType message_type = MessageType_NONE,
    flatbuffers::Offset<void> message = 0,
    flatbuffers::Offset<flatbuffers::String> user_data = 0) {
  HippoMessageBuilder builder_(_fbb);
  builder_.add_user_data(user_data);
  builder_.add_message(message);
  builder_.add_message_type(message_type);
  return builder_.Finish();
}

inline flatbuffers::Offset<HippoMessage> CreateHippoMessageDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    MessageType message_type = MessageType_NONE,
    flatbuffers::Offset<void> message = 0,
    const char *user_data = nullptr) {
  auto user_data__ = user_data ? _fbb.CreateString(user_data) : 0;
  return CreateHippoMessage(
      _fbb,
      message_type,
      message,
      user_data__);
}

inline bool VerifyMessageType(flatbuffers::Verifier &verifier, const void *obj, MessageType type) {
  switch (type) {
    case MessageType_NONE: {
      return true;
    }
    case MessageType_next_song: {
      auto ptr = reinterpret_cast<const HippoNextSong *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case MessageType_prev_song: {
      auto ptr = reinterpret_cast<const HippoPrevSong *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case MessageType_play_song: {
      auto ptr = reinterpret_cast<const HippoPlaySong *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case MessageType_stop_song: {
      auto ptr = reinterpret_cast<const HippoStopSong *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case MessageType_request_select_song: {
      auto ptr = reinterpret_cast<const HippoRequestSelectSong *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case MessageType_select_song: {
      auto ptr = reinterpret_cast<const HippoSelectSong *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case MessageType_request_add_urls: {
      auto ptr = reinterpret_cast<const HippoRequestAddUrls *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case MessageType_reply_added_urls: {
      auto ptr = reinterpret_cast<const HippoReplyAddedUrls *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case MessageType_request_tracker_data: {
      auto ptr = reinterpret_cast<const HippoRequestSelectSong *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case MessageType_tracker_data: {
      auto ptr = reinterpret_cast<const HippoTrackerData *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case MessageType_current_position: {
      auto ptr = reinterpret_cast<const HippoCurrentPosition *>(obj);
      return verifier.VerifyTable(ptr);
    }
    default: return false;
  }
}

inline bool VerifyMessageTypeVector(flatbuffers::Verifier &verifier, const flatbuffers::Vector<flatbuffers::Offset<void>> *values, const flatbuffers::Vector<uint8_t> *types) {
  if (!values || !types) return !values && !types;
  if (values->size() != types->size()) return false;
  for (flatbuffers::uoffset_t i = 0; i < values->size(); ++i) {
    if (!VerifyMessageType(
        verifier,  values->Get(i), types->GetEnum<MessageType>(i))) {
      return false;
    }
  }
  return true;
}

inline const HippoMessage *GetHippoMessage(const void *buf) {
  return flatbuffers::GetRoot<HippoMessage>(buf);
}

inline const HippoMessage *GetSizePrefixedHippoMessage(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<HippoMessage>(buf);
}

inline bool VerifyHippoMessageBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<HippoMessage>(nullptr);
}

inline bool VerifySizePrefixedHippoMessageBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<HippoMessage>(nullptr);
}

inline void FinishHippoMessageBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<HippoMessage> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedHippoMessageBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<HippoMessage> root) {
  fbb.FinishSizePrefixed(root);
}

#endif  // FLATBUFFERS_GENERATED_HIPPOMESSAGES_H_
