#include "Network.h"

#include <raylib.h>

int main() {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(800, 600, "MMO Client by Vinny Horgan");
    SetTargetFPS(60);

    connect();

    while (!WindowShouldClose()) {
        update();

        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    disconnect();

    CloseWindow();

    return 0;
}
