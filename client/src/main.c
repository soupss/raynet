#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <emscripten/websocket.h>
#include "communication.h"

#define PADDLE_SPACING 20
#define CAMERA_DISTANCE 50

void update(Vector3 *p_pos, Camera3D camera) {
    Ray mouse = GetMouseRay(GetMousePosition(), camera);
    float t = (PADDLE_SPACING - mouse.position.z) / mouse.direction.z;
    p_pos->x = mouse.position.x + mouse.direction.x * t;
    p_pos->y = mouse.position.y + mouse.direction.y * t;
}

void draw(Vector3 player_pos, Vector3 opponent_pos, Camera3D camera) {
    Vector3 paddle_size = { 10, 10, 0 };
    BeginDrawing();
    ClearBackground(GRAY);
    BeginMode3D(camera);
    DrawCubeWiresV(player_pos, paddle_size, GREEN);
    DrawCubeWiresV(opponent_pos, paddle_size, GREEN);
    EndMode3D();
    EndDrawing();
}

int main() {
    InitWindow(1, 1, "hlkjlkjj");
    SetWindowSize(GetMonitorWidth(GetCurrentMonitor()) * 0.6,
            GetMonitorHeight(GetCurrentMonitor()) * 0.6);
    SetTargetFPS(60);

    Vector3 player_pos = { 0, 0, PADDLE_SPACING };
    Vector3 opponent_pos = { 0, 0, -PADDLE_SPACING };

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0, 0, CAMERA_DISTANCE };
    camera.target = (Vector3){ 0, 0, 0 };
    camera.up = (Vector3){ 0, 1, 0 };
    camera.fovy = 90;
    camera.projection = CAMERA_PERSPECTIVE;

    initSocket();
    while(!WindowShouldClose()) {
        update(&player_pos, camera);
        draw(player_pos, opponent_pos, camera);
    }
    CloseWindow();
    return 0;
}
