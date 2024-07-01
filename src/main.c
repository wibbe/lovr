#include "api/api.h"
#include "event/event.h"
#include "core/os.h"
#include "util.h"
#include "modules/graphics/graphics.h"
#include "modules/system/system.h"
#include "modules/event/event.h"
#include "modules/filesystem/filesystem.h"
#include <lualib.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    os_init();
    lovrFilesystemInit();

    char bundle_path[LOVR_PATH_MAX];
    const char* bundle_root;

    char exe_path[LOVR_PATH_MAX];
    const char* exe_root;

    // Try and mount any bundled zip
    if (lovrFilesystemGetBundlePath(bundle_path, sizeof(bundle_path), &bundle_root)) {
        printf("Mounting '%s' / '%s'\n", bundle_path, bundle_root);
        bool mounted = lovrFilesystemMount(bundle_path, NULL, true, bundle_root);
        if (mounted) {
            printf("  Mounted!\n");
        }
    }

    // Mount 
    lovrFilesystemGetWorkingDirectory(exe_path, sizeof(exe_path));
    lovrFilesystemMount(exe_path, NULL, true, NULL);
    printf("Exe Path: '%s'\n", exe_path);

    size_t content_size;
    char* content = lovrFilesystemRead("arg.lua", &content_size);
    printf("%s", content);



    GraphicsConfig config = {
        .debug = false,
        .vsync = false,
        .stencil = false,
        .antialias = true
    };
    lovrGraphicsInit(&config);
    lovrEventInit();

    os_window_config window = {
        .width = 480 * 3,
        .height = 270 * 3,
        .fullscreen = false,
        .resizable = true,
        .title = "Tiny Wars",
    };

    lovrSystemOpenWindow(&window);

    float color[4] = { 0.3f, 0.4f, 0.7f, 1.0f };
    lovrGraphicsSetBackgroundColor(color);


    bool running = true;
    while (lovrSystemIsWindowOpen() && running) {
        lovrSystemPollEvents();

        Event event;
        while (lovrEventPoll(&event)) {
            if (event.type == EVENT_QUIT) {
                running = false;
            }
        }

        Pass *stack[8];
        Pass* pass = lovrGraphicsGetWindowPass();
        stack[0] = pass;

        lovrGraphicsSubmit(stack, 1);
        lovrGraphicsPresent();
    }


    lovrEventDestroy();
    lovrGraphicsDestroy();
    lovrFilesystemDestroy();
    os_destroy();
}
