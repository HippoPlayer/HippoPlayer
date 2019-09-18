// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_HIPPOMESSAGES_H_
#define FLATBUFFERS_GENERATED_HIPPOMESSAGES_H_

#include "flatbuffers/flatbuffers.h"

struct HippoNextSong;

struct HippoSelectSong;

struct HippoMessage;

enum MessageType {
  MessageType_NONE = 0,
  MessageType_next_song = 1,
  MessageType_select_song = 2,
  MessageType_MIN = MessageType_NONE,
  MessageType_MAX = MessageType_select_song
};

inline const MessageType (&EnumValuesMessageType())[3] {
  static const MessageType values[] = {
    MessageType_NONE,
    MessageType_next_song,
    MessageType_select_song
  };
  return values;
}

inline const char * const *EnumNamesMessageType() {
  static const char * const names[4] = {
    "NONE",
    "next_song",
    "select_song",
    nullptr
  };
  return names;
}

inline const char *EnumNameMessageType(MessageType e) {
  if (e < MessageType_NONE || e > MessageType_select_song) return "";
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

struct HippoSelectSong FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
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

struct HippoSelectSongBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_name(flatbuffers::Offset<flatbuffers::String> name) {
    fbb_.AddOffset(HippoSelectSong::VT_NAME, name);
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
    flatbuffers::Offset<flatbuffers::String> name = 0) {
  HippoSelectSongBuilder builder_(_fbb);
  builder_.add_name(name);
  return builder_.Finish();
}

inline flatbuffers::Offset<HippoSelectSong> CreateHippoSelectSongDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *name = nullptr) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  return CreateHippoSelectSong(
      _fbb,
      name__);
}

struct HippoMessage FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_MESSAGE_TYPE = 6,
    VT_MESSAGE = 8,
    VT_USER_DATA = 10
  };
  int8_t id() const {
    return GetField<int8_t>(VT_ID, 0);
  }
  MessageType message_type() const {
    return static_cast<MessageType>(GetField<uint8_t>(VT_MESSAGE_TYPE, 0));
  }
  const void *message() const {
    return GetPointer<const void *>(VT_MESSAGE);
  }
  const HippoNextSong *message_as_next_song() const {
    return message_type() == MessageType_next_song ? static_cast<const HippoNextSong *>(message()) : nullptr;
  }
  const HippoNextSong *message_as_select_song() const {
    return message_type() == MessageType_select_song ? static_cast<const HippoNextSong *>(message()) : nullptr;
  }
  const flatbuffers::String *user_data() const {
    return GetPointer<const flatbuffers::String *>(VT_USER_DATA);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int8_t>(verifier, VT_ID) &&
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
  void add_id(int8_t id) {
    fbb_.AddElement<int8_t>(HippoMessage::VT_ID, id, 0);
  }
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
    int8_t id = 0,
    MessageType message_type = MessageType_NONE,
    flatbuffers::Offset<void> message = 0,
    flatbuffers::Offset<flatbuffers::String> user_data = 0) {
  HippoMessageBuilder builder_(_fbb);
  builder_.add_user_data(user_data);
  builder_.add_message(message);
  builder_.add_message_type(message_type);
  builder_.add_id(id);
  return builder_.Finish();
}

inline flatbuffers::Offset<HippoMessage> CreateHippoMessageDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    int8_t id = 0,
    MessageType message_type = MessageType_NONE,
    flatbuffers::Offset<void> message = 0,
    const char *user_data = nullptr) {
  auto user_data__ = user_data ? _fbb.CreateString(user_data) : 0;
  return CreateHippoMessage(
      _fbb,
      id,
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
    case MessageType_select_song: {
      auto ptr = reinterpret_cast<const HippoNextSong *>(obj);
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
