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
    
    
    float wavelength = C / frequency; // wavelength of electron is c/f
    float sample_length = wavelength/10; //sample_length is 1/10 of the wavelength

    float magnitude;

    int total_segments;
    float num_sub_segments[num_segments];
    Segment antenna_start_end_values[num_segments]; //storing start and end values in array in order to use them later for calacultaion when we overide previous values in Antenna
    

    //find magnitude of line
    for(int i = 0; i < num_segments; ++i){
        magnitude = sqrt(pow(Antenna[i].start_line[1] - Antenna[i].end_line[1],2) + pow((Antenna[i].start_line[2] - Antenna[i].end_line[2]), 2) + pow((Antenna[i].start_line[3] - Antenna[i].end_line[3]) ,2));

        num_sub_segments[i] = ceil(magnitude / sample_length);
        magnitude *= sample_length; //divide magnitude of line by SAMPLE_RATE
 
        total_segments += (int)num_sub_segments[i];//FIND TOTAL NUMBER OF SEGMENTS NEEDED SO WE CAN REALLOC
        
        for(int k; k < 3; ++k){
            antenna_start_end_values[i].start_line[k] = Antenna[i].start_line[k];
            antenna_start_end_values[i].end_line[k] = Antenna[i].end_line[k];
        }
     }
    
    Antenna = realloc(Antenna, (sizeof(Segment) * total_segments)); //reallocate memory for the total number of segments. This code looks like ass

    //REBUILD ANTENNA WITH NEW LINES
    
    for(int i = 1; i < (num_segments+1); ++i){
        for(int k = 0; k < num_sub_segments[i]; ++k){
            for(int j = 0; j < 3; ++j){
        //          Antenna[(i*k)].start_line[j] =  
          //        Antenna[(i*k)].end_line[j] =
            }
        }
    }

    return Antenna;
}



