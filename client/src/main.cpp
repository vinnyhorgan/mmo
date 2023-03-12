#include <raylib.h>

int main() {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(800, 600, "MMO Client by Vinny Horgan");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
