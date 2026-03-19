#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cjson/cJSON.h>
#include "segment.h"



int edit_antenna(void){
    // create a cJSON object
    printf("Editing Antenna\n");
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "name", "John Doe");
    cJSON_AddNumberToObject(json, "age", 30);
    cJSON_AddStringToObject(json, "email", "john.doe@example.com");

    // convert the cJSON object to a JSON string
    char *json_str = cJSON_Print(json);

    // write the JSON string to a file
    FILE *fp = fopen("antenna.json", "w");
    if (fp == NULL) {
        printf("Error: Unable to open the file.\n");
        return 1;
    }
    printf("%s\n", json_str);

    fputs(json_str, fp);
    fclose(fp);

    // free the JSON string and cJSON object
    cJSON_free(json_str);
    cJSON_Delete(json);
    printf("Antenna successfully edited\n"); return 0;

}