#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cjson/cJSON.h>

#include "segment.h" 

#define C 3e8
#define SAMPLE_RATE 10

Segment* init_sub_segments(Segment*, int num_segments, int frequency);

Segment* init_antenna(int *n) {

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
    //

    cJSON *number_of_segments = cJSON_GetObjectItem(json, "number_of_segments");  

    if(number_of_segments == NULL){printf("failed to find number of segments"); return NULL;} 

    *n = number_of_segments->valueint;

    Segment *Antenna = malloc((*n) * sizeof(Segment));
    cJSON *segments = cJSON_GetObjectItem(json, "segments");

    cJSON *frequency = cJSON_GetObjectItem(json, "frequency");
    int frequency_value = frequency->valueint; 

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
    }

    /*--------------------------------------------------------------------------------------------*/
    
    init_sub_segments(Antenna, *n, frequency_value);
    
    return Antenna;
}

Segment* init_sub_segments(Segment* Antenna, int num_segments, int frequency){
    
    printf("this wave is running at %d, Hz", frequency);
    
    float wavelength = C / frequency;
    float sample_length = wavelength/10;

    printf("%f\n", wavelength);
    printf("%f\n", sample_length);

    //find magnitude of line
    for(int i = 0; i < num_segments; ++i){
        
        
        for(int k = 0; k < 3; ++k){
           // Antenna[i].start_line[k]     
               
        }
    }

    //divide magnitude of line by SAMPLE_RATE
    //
    //REBUILD ANTENNA WITH NEW LINES

    return Antenna;
}



