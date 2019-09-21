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

struct HippoMessage;

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
  MessageType_MIN = MessageType_NONE,
  MessageType_MAX = MessageType_reply_added_urls
};

inline const MessageType (&EnumValuesMessageType())[9] {
  static const MessageType values[] = {
    MessageType_NONE,
    MessageType_next_song,
    MessageType_prev_song,
    MessageType_play_song,
    MessageType_stop_song,
    MessageType_request_select_song,
    MessageType_select_song,
    MessageType_request_add_urls,
    MessageType_reply_added_urls
  };
  return values;
}

inline const char * const *EnumNamesMessageType() {
  static const char * const names[10] = {
    "NONE",
    "next_song",
    "prev_song",
    "play_song",
    "stop_song",
    "request_select_song",
    "select_song",
    "request_add_urls",
    "reply_added_urls",
    nullptr
  };
  return names;
}

inline const char *EnumNameMessageType(MessageType e) {
  if (e < MessageType_NONE || e > MessageType_reply_added_urls) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesMessageType()[index];
}

template<typename T> struct MessageTypeTraits {
  static const MessageType enum_value = MessageType_NONE;
};

template<> struct MessageTypeTraits<HippoNextSong> {
  static const MessageType enum_value = MessageType_next_song;
};

template<> struct MessageTypeTraits<HippoPrevSong> {
  static const MessageType enum_value = MessageType_prev_song;
};

template<> struct MessageTypeTraits<HippoPlaySong> {
  static const MessageType enum_value = MessageType_play_song;
};

template<> struct MessageTypeTraits<HippoStopSong> {
  static const MessageType enum_value = MessageType_stop_song;
};

template<> struct MessageTypeTraits<HippoRequestSelectSong> {
  static const MessageType enum_value = MessageType_request_select_song;
};

template<> struct MessageTypeTraits<HippoSelectSong> {
  static const MessageType enum_value = MessageType_select_song;
};

template<> struct MessageTypeTraits<HippoRequestAddUrls> {
  static const MessageType enum_value = MessageType_request_add_urls;
};

template<> struct MessageTypeTraits<HippoReplyAddedUrls> {
  static const MessageType enum_value = MessageType_reply_added_urls;
};

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
  template<typename T> const T *message_as() const;
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

template<> inline const HippoNextSong *HippoMessage::message_as<HippoNextSong>() const {
  return message_as_next_song();
}

template<> inline const HippoPrevSong *HippoMessage::message_as<HippoPrevSong>() const {
  return message_as_prev_song();
}

template<> inline const HippoPlaySong *HippoMessage::message_as<HippoPlaySong>() const {
  return message_as_play_song();
}

template<> inline const HippoStopSong *HippoMessage::message_as<HippoStopSong>() const {
  return message_as_stop_song();
}

template<> inline const HippoRequestSelectSong *HippoMessage::message_as<HippoRequestSelectSong>() const {
  return message_as_request_select_song();
}

template<> inline const HippoSelectSong *HippoMessage::message_as<HippoSelectSong>() const {
  return message_as_select_song();
}

template<> inline const HippoRequestAddUrls *HippoMessage::message_as<HippoRequestAddUrls>() const {
  return message_as_request_add_urls();
}

template<> inline const HippoReplyAddedUrls *HippoMessage::message_as<HippoReplyAddedUrls>() const {
  return message_as_reply_added_urls();
}

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
