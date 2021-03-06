/*
 * parse_JSON.c
 *
 *  Created on: Jul 17, 2018
 *      Author: hai_dotcom
 */

#include "parse_JSON.h"
#include "my_sd_card.h"
#include "http_download.h"

//extern xQueueHandle HttpUpdate_Queue_Handle;
//extern TaskHandle_t xhttp_update_Handle;
extern xQueueHandle HttpDownload_Queue_Handle;
extern xTaskHandle xhttp_download_Handle;

void read_JSON()
{
	char line[64];
	char * json_string;
	FILE * json_file;
	if(check_JSON() == true){
		json_file = fopen("/sdcard/json.txt","r");
		fseek(json_file,0L,SEEK_END);
		long size = ftell(json_file);
		printf("size: %ld \n",size);
		fseek(json_file,0L,SEEK_SET);
		json_string =(char*) calloc(size,sizeof(char*));
		if(json_file == NULL){
			printf("Failed to open json for reading \n");
		}
		while(fgets(line,sizeof(line),json_file)!=NULL){
			strcat(json_string,line);
		}
		printf(json_string);
		bool status = check_update(json_string);
		free(json_string);
	}
}

bool check_update(const char * const json)
{
	data data;
	const cJSON *update = NULL;
    const cJSON *artifacts = NULL;

    bool status = false;
    cJSON *muse_json = cJSON_Parse(json);
    if (muse_json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = 0;
        goto end;
    }
    update = cJSON_GetObjectItemCaseSensitive(muse_json, "update");
    if (cJSON_IsString(update) && (update->valuestring != NULL))
    {
        printf("Checking update \"%s\"\n", update->valuestring);
        data.update =update->valuestring;
    }
    data.url = "http://www.stream.esy.es/database/data/hcm_fine_arts_museum/overview/hcm_fine_arts_museum.txt";
   	data.name = "/sdcard/json.txt";
   	data.request= "GET http://www.stream.esy.es/database/data/hcm_fine_arts_museum/overview/hcm_fine_arts_museum.txt HTTP/1.0\r\n"
   					"Host: "WEB_SERVER"\r\n"
					"User-Agent: esp-idf/1.0 esp32\r\n"
					"\r\n";
//   	xTaskCreate(&http_check_update_task,"http_check_update_task",2048,NULL,5,xhttp_update_Handle);
//   	if(!xQueueSend(HttpUpdate_Queue_Handle,&data,portMAX_DELAY)){
//   		printf("Parse JSON: Failed to send request to http_check_update_task \n");
//   	}
//   	else
//   		printf("Parse JSON: Successfully send request to http_check_update_task \n");
   	xTaskCreate(&http_download_task,"http_download_task",2048,NULL,6,xhttp_download_Handle);
   	if(!xQueueSend(HttpDownload_Queue_Handle,&data,portMAX_DELAY)){
   		printf("Parse JSON: Failed to send request to http_check_update_task \n");
   	}
   	else
   		printf("Parse JSON: Successfully send request to http_check_update_task \n");
	end:
		cJSON_Delete(muse_json);
	    return status;
}
