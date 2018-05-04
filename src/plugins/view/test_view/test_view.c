#include <HippoPlugin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../../external/rute_cpp/c_api.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HippoMessageAPI* g_message_api;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct TestViewPlugin {
	struct RUPluginUI* ui;
} TestViewPlugin;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* test_view_create(HippoServiceAPI* services) {
    TestViewPlugin* plugin = malloc(sizeof(TestViewPlugin));
    memset(plugin, 0, sizeof(TestViewPlugin));

	g_message_api = HippoServiceAPI_get_message_api(services, 0);

    return plugin;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void test_view_setup_ui(void* user_data, struct RUPluginUI* ui_funcs) {
    TestViewPlugin* plugin = (TestViewPlugin*)user_data;
    plugin->ui = ui_funcs;

    struct RUPushButton button = RU_create_push_button(plugin->ui);

    // Test sending a message

    HippoMessage* message = g_message_api->begin_request(g_message_api->priv_data, "test_request");
    message->write_array_count(message->priv_data, 1);
    message->write_str(message->priv_data, "foo.mod");

	g_message_api->end_request(g_message_api->priv_data, message);

	RUPushButton_set_text(button, "foobar");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int test_view_destroy(void* user_data) {
    TestViewPlugin* plugin = (TestViewPlugin*)user_data;
    free(plugin);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void test_view_event(int event) {
    (void)event;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoViewPlugin s_test_view = {
    HIPPO_VIEW_PLUGIN_API_VERSION,
    "TestView",
    "0.0.1",
    test_view_create,
    test_view_setup_ui,
    test_view_destroy,
    test_view_event,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HIPPO_EXPORT HippoViewPlugin* hippo_view_plugin() {
	return &s_test_view;
}

