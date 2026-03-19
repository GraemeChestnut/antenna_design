#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cjson/cJSON.h>


#include "segment.h"

Segment* init_antenna(int *n)
{

    //JSON MUMBO JUMBO
        FILE *fp = fopen("antenna.json", "r");
        if (fp == NULL) {
            printf("Error: Unable to open the file.\n");
            return NULL;
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
                return NULL;
            }
            cJSON_Delete(json);
        }

    
        /*--------------------------------------------------------------------------------------------*/
    
    
        /*important!!!!! this code parses through to get the first value of the first segment. 
    For next time, recure throuhg this in order to grab all six values for the one segment. 
    This will allow us to store them in a segment struct, 
    and then add that segment to the antenan struct, and thn we are abe to display it on the screen.*/

    cJSON *number_of_segments = cJSON_GetObjectItem(json, "number_of_segments");    
   // if (!number_of_segments) return NULL;
    *n = number_of_segments->valueint;

    Segment *Antenna = malloc((*n) * sizeof(Segment));
    
        cJSON *segments = cJSON_GetObjectItem(json, "segments");
    for(int i = 0; i < *n; ++i){
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

    printf("antenna intialized properly");
    /*--------------------------------------------------------------------------------------------*/
    return Antenna;
}