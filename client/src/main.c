#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <emscripten/websocket.h>
#include "communication.h"


int main() {
    InitWindow(1, 1, "hlkjlkjj");
    SetWindowSize(GetMonitorWidth(GetCurrentMonitor()) * 0.6,
            GetMonitorHeight(GetCurrentMonitor()) * 0.6);
    SetTargetFPS(60);

    Vector3 cube_pos = { 0, 0, 0 };
    int cube_dir = 1;
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 10, 10, 10 };
    camera.target = cube_pos;
    camera.up = (Vector3){ 0, 1, 0 };
    camera.fovy = 45;
    camera.projection = CAMERA_PERSPECTIVE;
    initSocket();

    while(!WindowShouldClose()) {
        cube_pos.y += cube_dir * 0.1f;
        if (cube_pos.y > 5 || cube_pos.y < -5) {
            cube_dir *= -1;
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);
        DrawCube(cube_pos, 2, 2, 2, GREEN);
        DrawCubeWires(cube_pos, 2, 2, 2, SKYBLUE);
        EndMode3D();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
