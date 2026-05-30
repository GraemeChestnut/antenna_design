#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <cjson/cJSON.h>

#include "antenna.h"
#include "segment.h"  

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main(void)
{
      // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 800;
    
    //setting antenna varibles
    int n;
    float voltage;
    double wavelength;
    float radius;

    Segment* Antenna = init_antenna(&n, &voltage, &wavelength, &radius); 

    current_distribution(Antenna, n, &voltage, wavelength);

    if(Antenna == NULL){printf("ERROR: ANTENNA CONFIGURATION FAILED. PLEASE CHECK JSON FILE CONFIGURATION\n");return 1;}
    
    //split each segments into more segments in order to do more math //


    InitWindow(screenWidth, screenHeight, "Ant");
    

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 10.0f, 10.0f, 10.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };
                 
    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        Vector2 mouse_position = GetMousePosition();
        // Update
        UpdateCamera(&camera, CAMERA_ORBITAL);
        //----------------------------------------------------------------------------------
        
    //    SetCameraMoveControls(KEY_W, KEY_S, KEY_D, KEY_A, KEY_SPACE, KEY_LEFT_SHIFT);

        if (IsKeyPressed(KEY_Z)) camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(BLACK);

            BeginMode3D(camera);

           //printf("%ld", sizeof(Antenna));
            //drawing antenna
            if (Antenna != NULL) {
              for (int i = 0; i < n; ++i) {
                /*
                printf(
                  "Segment %d | "
                  "Start: (%f, %f, %f) -> "
                  "End: (%f, %f, %f) | "
                  "Midpoint: (%f, %f, %f) "
                  "Voltage: %f\n",

                    i,

                      Antenna[i].start_line[0],
                      Antenna[i].start_line[1],
                      Antenna[i].start_line[2],

                      Antenna[i].end_line[0],
                      Antenna[i].end_line[1],
                      Antenna[i].end_line[2],

                      Antenna[i].midpoint[0],
                      Antenna[i].midpoint[1],
                      Antenna[i].midpoint[2],

                      Antenna[i].voltage
                );*/

                //void DrawCylinderEx(Vector3 startPos, Vector3 endPos, float startRadius, float endRadius, int sides, Color color);
                
                DrawCylinderEx(
                    (Vector3){
                      Antenna[i].start_line[0],
                      Antenna[i].start_line[1],
                      Antenna[i].start_line[2]
                      },
                    (Vector3){
                      Antenna[i].end_line[0],
                      Antenna[i].end_line[1],
                      Antenna[i].end_line[2]
                      }, 0.01, 0.01, 100, RAYWHITE
                );
              }

        } else {
            printf("antenna unsuccessfully rendered.\nABORTED LOADING\n");
            return 1;
            } 
 
                DrawLine3D((Vector3){0.0f,0.0f,0.0f},(Vector3){7.5f,0.0f,0.0f},ORANGE);
                DrawLine3D((Vector3){0.0f,0.0f,0.0f},(Vector3){0.0f,7.5f,0.0f},BLUE);
                DrawLine3D((Vector3){0.0f,0.0f,0.0f},(Vector3){0.0f,0.0f,7.5f},GREEN);

            EndMode3D();

            Rectangle button_size = {700, 0, 100, 50};

            int edit_antenna_button = GuiButton(button_size, "Edit Antenna");
            static int show_input = 0;
            static int close_input = -1;
            char message[10];

            if (edit_antenna_button == 1)
            {
                show_input = 1;
                close_input = -1;
                message[0] = '\0';
            }
            
            
            if((show_input == 1 && close_input == -1 )){

                int is_input_box_open = GuiTextInputBox((Rectangle){300, 300, 200, 200}, "Edit Antenna","Please input a float in \norder to choose antenna's x value", "hello", message, 100, false); 
                
                close_input = is_input_box_open;

                if(IsKeyPressed(KEY_ENTER)){
                    close_input = 1;
                }
                if(close_input == 1 || IsKeyPressed(KEY_ENTER)){
                    printf("%s\n", message);
                }
            }
            
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    
    return 0;
}


