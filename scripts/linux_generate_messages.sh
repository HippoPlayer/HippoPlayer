#/bin/sh
t2-output/linux-gcc-debug-default/flatc --cpp -o src/plugin_api src/plugin_api/HippoMessages.fbs
t2-output/linux-gcc-debug-default/flatc --rust -o src/hippo_core/messages/src src/plugin_api/HippoMessages.fbs

