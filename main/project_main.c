/****************************************************************************

****************************************************************************/

#include "project_main.h"
#include "my_ibeacon.h"
#include "my_sd_card.h"
#include "http_download.h"
#include "parse_JSON.h"

xQueueHandle Beacon_Queue_Handle = 0;
xQueueHandle Http_Queue_Handle = 0;
void app_main()
{
	Beacon_Queue_Handle = xQueueCreate(3,sizeof(simple_beacon));
    Http_Queue_Handle = xQueueCreate(3,sizeof(data));
	ESP_ERROR_CHECK(nvs_flash_init());
    ibeacon_init();
    sd_card_init();
    initialise_wifi();
    read_JSON();
    xTaskCreate(&action_inzone,"action_inzone",2048,NULL,7,NULL);
}
