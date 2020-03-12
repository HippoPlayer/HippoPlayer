#![allow(dead_code)]
#![allow(non_snake_case)]
#![allow(unused_imports)]

extern crate flatbuffers;

pub mod HippoMessages_generated;
pub use flatbuffers::*;
pub use HippoMessages_generated::*;

impl<'a> HippoMessage<'a> {
    pub fn create_def(
        mut builder: flatbuffers::FlatBufferBuilder,
        message_type: MessageType,
        message: flatbuffers::WIPOffset<flatbuffers::UnionWIPOffset>,
    ) -> Box<[u8]> {
        let message = HippoMessage::create(
            &mut builder,
            &HippoMessageArgs {
                message_type,
                message: Some(message),
                user_data: None,
            },
        );

        builder.finish(message, None);
        builder.finished_data().to_vec().into_boxed_slice()
    }
}
