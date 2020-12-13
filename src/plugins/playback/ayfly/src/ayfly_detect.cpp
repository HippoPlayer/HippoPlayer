#include "libayfly/ayfly.h"

extern bool ay_sys_can_play(void* data, int len);

bool ay_can_play(void* data, int len) {
    return ay_sys_can_play(data, len);
}
