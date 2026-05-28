#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cjson/cJSON.h>

#include "segment.h" 

#define C 3e8

Segment* init_antenna(int *n, float *voltage, double *wavelength) {

    const int sample_rate = 1; 

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
    
    cJSON *voltage_json = cJSON_GetObjectItem(json, "voltage");
    *voltage = voltage_json->valuedouble;

    //SET ANTENNA_CONSTANTS
    *wavelength = (double)(C / frequency_value); // wavelength of electron is c/f
    float sample_length = *wavelength/(double)(sample_rate); //sample_length is 1/10 of the wavelength
    int total_segments = 0;
    float num_sub_segments[*n];
    int sub_segments;
    
    float magnitude; 

    Segment plc_antenna[*n+1]; //create placeholder values for json coordinates. Needed to find total number of sub segments within each segment, since you need the magnitude of each line.

    for(int i = 0; i < *n; ++i){ //iterate over each segment that is in JSON file
        cJSON *first_segment = cJSON_GetArrayItem(segments,  i);
        
            for(int k = 0; k < 3; k++){ // make placeholder values for each segment in JSON
                cJSON *start_line = cJSON_GetObjectItem(first_segment, "start_line");
                cJSON *start_line_value = cJSON_GetArrayItem(start_line, k);
             
                cJSON *end_line = cJSON_GetObjectItem(first_segment, "end_line");
                cJSON *end_line_value = cJSON_GetArrayItem(end_line, k);
            
                //store i segment insdie of plc holder array
                plc_antenna[i].start_line[k] = start_line_value->valuedouble;
                plc_antenna[i].end_line[k] = end_line_value->valuedouble;
            }
            
            //find magnitude of i segment
            magnitude = sqrt( //calculate magnitude to see how many sub semgnet
                pow(plc_antenna[i].start_line[0] - plc_antenna[i].end_line[0] , 2) +
                pow(plc_antenna[i].start_line[1] - plc_antenna[i].end_line[1] , 2) +
                pow(plc_antenna[i].start_line[2] - plc_antenna[i].end_line[2] , 2));
            
                
            //compute number of necceasry sub segments inside of each segment, absed on the magnitune of that segmnet. ie, more sub_semgent sfor large line
            sub_segments = ceil(magnitude / sample_length);
            total_segments += sub_segments;
            //interate over number of sub_segments, and then knowning the sample_legnthfinally add true subsemgne initalization to main Antenna variabel
      }
  
      Segment *Antenna = malloc(total_segments * sizeof(Segment)); //allocate Antenna based on total number of segments
      

      int idx = 0; //index across all the for loops
      for(int i = 0; i < *n; ++i){

          
            for(int m = 0; m < sub_segments; ++m){ // iterate over each sub segment within the segemnt
              //l looks too much like an i, we're using m cuz i can
            
              for(int o = 0; o < 3; ++o){// iteatee over each coordinate within each segment

                 //insert code calaculating individual start/end points for each new line segment, its place should be building off of the previous line semgnet; segment[m].start_line = segment[m-1].end_line
                  
                  //if m = 0, then just add the startlines diretctly in
                  if(m == 0){
                    Antenna[idx].start_line[o] = plc_antenna[i].start_line[o];
                  } 
                  else{
                    Antenna[idx].start_line[o] = Antenna[idx -1].end_line[o];
                  }
                  
                  //if m = sub_semgnets-1, the just add the endline directly line
                  
                  if(m == (sub_segments-1)){
                    Antenna[idx].end_line[o] = plc_antenna[i].end_line[o];
                  }
                  else{

                    float cartesian_distance = (plc_antenna[i].end_line[o] - plc_antenna[i].start_line[o]) / sub_segments;
                    Antenna[idx].end_line[o] = Antenna[idx].start_line[o] + (cartesian_distance);
                  }
              }

              //calculate the midpoint of each subsegment

              Antenna[idx].midpoint[0] = (Antenna[idx].start_line[0] + Antenna[idx].end_line[0]) / 2;
              Antenna[idx].midpoint[1] = (Antenna[idx].start_line[1] + Antenna[idx].end_line[1]) / 2;
              Antenna[idx].midpoint[2] = (Antenna[idx].start_line[2] + Antenna[idx].end_line[2]) / 2;



              idx++;

            }
    }

    /*--------------------------------------------------------------------------------------------*/


    *n = total_segments; //set *n to new number of semgnts, since this is necceary for displaying
    return Antenna;
}


