#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <cjson/cJSON.h>

typedef struct{
        float start_line[3];
        float end_line[3];
    } Segment;


Segment* init_antenna(int *n)
{
    


    //JSON MUMBO JUMBO
        FILE *fp = fopen("antenna.json", "r");
        if (fp == NULL) {
            printf("Error: Unable to open the file.\n");
        }

        // read the file contents into a string
        char buffer[1024];
        int len = fread(buffer, 1, sizeof(buffer), fp);
        buffer[len] = '\0'; // null-terminate the string
        fclose(fp);
        

        // parse the JSON data
        cJSON *json = cJSON_Parse(buffer);

        if (json == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) {
                printf("Error: %s\n", error_ptr);
            }
            cJSON_Delete(json);
        }

    
        /*--------------------------------------------------------------------------------------------*/
    
    
        /*important!!!!! this code parses through to get the first value of the first segment. 
    For next time, recure throuhg this in order to grab all six values for the one segment. 
    This will allow us to store them in a segment struct, 
    and then add that segment to the antenan struct, and thn we are abe to display it on the screen.*/

    cJSON *number_of_segments = cJSON_GetObjectItem(json, "number_of_segments");    
    *n = number_of_segments->valueint;

    Segment *Antenna = malloc((*n) * sizeof(Segment));
    
        
    for(int i = 0; i < *n; ++i){
        cJSON *segments = cJSON_GetObjectItem(json, "segments");
        cJSON *first_segment = cJSON_GetArrayItem(segments,  i);

        for(int k = 0; k < 3; k++){
            cJSON *start_line = cJSON_GetObjectItem(first_segment, "start_line");
            cJSON *start_line_value = cJSON_GetArrayItem(start_line, k);
             
            cJSON *end_line = cJSON_GetObjectItem(first_segment, "end_line");
            cJSON *end_line_value = cJSON_GetArrayItem(end_line, k);
            
            //printf("%f ", start_line_value->valuedouble);

            Antenna[i].start_line[k] = start_line_value->valuedouble;
            Antenna[i].end_line[k] = end_line_value->valuedouble;
        }
        printf("\n");
    }


    
    /*--------------------------------------------------------------------------------------------*/
    return Antenna;
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 800;

    int n;
    Segment* Antenna = init_antenna(&n);

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
            }
           
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
    free(Antenna);
    return 0;
}


