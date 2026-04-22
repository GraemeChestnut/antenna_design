#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cjson/cJSON.h>

#include "segment.h" 

#define C 3e8

Segment* init_antenna(int *n) {

    const int sample_rate = 10;

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

    cJSON *segments = cJSON_GetObjectItem(json, "segments");

    //grab frequency data from JSON
    cJSON *frequency = cJSON_GetObjectItem(json, "frequency");
    int frequency_value = frequency->valueint; 

    //SET ANTENNA_CONSTANTS
    float wavelength = C / frequency_value; // wavelength of electron is c/f
    float sample_length = wavelength/sample_rate; //sample_length is 1/10 of the wavelength
    int total_segments;
    float num_sub_segments[*n];
    
    float magnitude; 

    Segment *Antenna = malloc((*n * sample_rate) * (sizeof(Segment))); //allocated alloted 
    Segment plc_antenna[1]; //create placeholder values for json coordinates. Needed to find total number of sub segments within each segment, since you need the magnitude of each line.

    for(int i = 0; i < *n; ++i){
        cJSON *first_segment = cJSON_GetArrayItem(segments,  i);
        
            for(int k = 0; k < 3; k++){
                cJSON *start_line = cJSON_GetObjectItem(first_segment, "start_line");
                cJSON *start_line_value = cJSON_GetArrayItem(start_line, k);
             
                cJSON *end_line = cJSON_GetObjectItem(first_segment, "end_line");
                cJSON *end_line_value = cJSON_GetArrayItem(end_line, k);
            
                //store i segment insdie of plc holder array
                plc_antenna[i].start_line[k] = start_line_value->valuedouble;
                plc_antenna[i].end_line[k] = end_line_value->valuedouble;
            }
            
            //find magnitude of i segment
            magnitude = sqrt(
                pow(plc_antenna[0].start_line[0] - plc_antenna[0].end_line[0] , 2) +
                pow(plc_antenna[0].start_line[1] - plc_antenna[0].end_line[1] , 2) +
                pow(plc_antenna[0].start_line[2] - plc_antenna[0].end_line[2] , 2));
            
                
            //compute number of necceasry sub segments inside of each segment, absed on the magnitune of that segmnet. ie, more sub_semgent sfor large line
            int sub_segments = ceil( magnitude / sample_length);

            //interate over number of sub_segments, and then knowning the sample_legnthfinally add true subsemgne initalization to main Antenna variabel
            
            for(int m = 0; m < sub_segments; ++m){
              //l looks too much like an i, we're using m cuz i can
            
              for(int o = 0; o < 3; ++o){
                  //insert code calaculating individual start/end points for each new line segment, its place should be building off of the previous line semgnet; segment[m].start_line = segment[m-1].end_line
                  //
                  //
              }
            }
                                        
    

    }

    /*--------------------------------------------------------------------------------------------*/

     return Antenna;
}


