#include <HippoPlugin.h>
#include <fileref.h>
#include <tag.h>
#include <tpropertymap.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_metadata(TagLib::String str, const char* meta_id, HippoMetadataId index, const HippoMetadataAPI* metadata_api) {
    if (str.isNull()) {
        return;
    }

    printf("updating metadata %s -> %s\n", meta_id, str.toCString());

    HippoMetadata_set_tag(metadata_api, index, meta_id, str.toCString());
}

// TODO: Put this as a service also use IO unctions
extern "C" uint64_t taglib_update_metadata(const char* url, const HippoMetadataAPI* metadata_api) {
    TagLib::FileRef f(url);

    if (f.isNull()) {
        return (uint64_t)~0;
    }

    HippoMetadataId index = HippoMetadata_create_url(metadata_api, url);

    if (f.tag()) {
        TagLib::Tag* tag = f.tag();
        set_metadata(tag->title(), HippoMetadata_TitleTag, index, metadata_api);
        set_metadata(tag->artist(), HippoMetadata_ArtistTag, index, metadata_api);
        set_metadata(tag->album(), HippoMetadata_AlbumTag, index, metadata_api);
        set_metadata(tag->comment(), HippoMetadata_MessageTag, index, metadata_api);
        set_metadata(tag->genre(), HippoMetadata_GenreTag, index, metadata_api);

        if (tag->year() != 0) {
            char temp[16];
            sprintf(temp, "%d", tag->year());
            HippoMetadata_set_tag(metadata_api, index, HippoMetadata_DateTag, temp);
        }
    } else {
        printf("no metadata!\n");
    }

    if (f.audioProperties()) {
        TagLib::AudioProperties* properties = f.audioProperties();
        HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag,
            properties->lengthInMilliseconds() / 1000.0f);
    } else {
        HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag, 0.0f);
    }

    return index;
}
