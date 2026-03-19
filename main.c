#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <cjson/cJSON.h>

#include "antenna.h"
#include "segment.h"



int main(void)
{
   // int six_seven = edit_antenna();
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 800;

    int n;
    Segment* Antenna = init_antenna(&n);
    if(Antenna == NULL){printf("ERROR: ANTENNA CONFIGURATION INCORRECT. PLEASE CHECK JSON FILE");return 1;}

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera free");
    

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 10.0f, 10.0f, 10.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };

    DisableCursor();                  

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        UpdateCamera(&camera, CAMERA_ORBITAL);
        //----------------------------------------------------------------------------------
        
    //    SetCameraMoveControls(KEY_W, KEY_S, KEY_D, KEY_A, KEY_SPACE, KEY_LEFT_SHIFT);

        if (IsKeyPressed(KEY_Z)) camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
            
           // printf("%ld", sizeof(Antenna));

           if (Antenna != NULL){
           //printf("antenna succesffuly rendered");
                for (int i = 0; i < n; ++i) {
                    DrawLine3D(
                        (Vector3){
                            Antenna[i].start_line[0],
                            Antenna[i].start_line[1],
                            Antenna[i].start_line[2]
                        },
                        (Vector3){
                            Antenna[i].end_line[0],
                            Antenna[i].end_line[1],
                            Antenna[i].end_line[2]
                        },
                        BLACK
                    );
                    printf("antenna succesffuly rendered");
                }
            } else{printf("antenna unsuccesffuly rendered.\n ABORTED LOADING"); return 1;}

                DrawLine3D((Vector3){0.0f,0.0f,0.0f},(Vector3){10.0f,0.0f,0.0f},ORANGE);
                DrawLine3D((Vector3){0.0f,0.0f,0.0f},(Vector3){0.0f,10.0f,0.0f},BLUE);
                DrawLine3D((Vector3){0.0f,0.0f,0.0f},(Vector3){0.0f,0.0f,10.0f},GREEN);

            
            EndMode3D();

            DrawRectangle(10, 10, 320, 93, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines( 10, 10, 320, 93, BLUE);

            DrawText("Free camera default controls:", 20, 20, 10, BLACK);
            DrawText("- Mouse Wheel to Zoom in-out", 40, 40, 10, DARKGRAY);
            DrawText("- Mouse Wheel Pressed to Pan", 40, 60, 10, DARKGRAY);
            DrawText("- Z to zoom to (0, 0, 0)", 40, 80, 10, DARKGRAY);

            
            
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    
    return 0;
}


