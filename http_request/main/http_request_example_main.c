/* HTTP GET Example using plain POSIX sockets

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "esp_netif.h"


/* Constants that aren't configurable in menuconfig */
#define WEB_SERVER "192.168.137.52"
#define WEB_PORT "80"
#define WEB_PATH "/1KB"

// #define WEB_SERVER "192.168.0.220"
// #define WEB_PORT "80"
// #define WEB_PATH "/hello"

static const char *TAG = "example";

static const char *REQUEST = "GET " WEB_PATH " HTTP/1.1\r\n"
    "Host: "WEB_SERVER":"WEB_PORT"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n"
    "\r\n";

static void http_get_task(void *pvParameters)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    char recv_buf[1500];

    while(1) {
        int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

        if(err != 0 || res == NULL) {
            ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        /* Code to print the resolved IP.

           Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
        addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
        ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));
        printf(REQUEST);
        s = socket(res->ai_family, res->ai_socktype, 0);
        if(s < 0) {
            ESP_LOGE(TAG, "... Failed to allocate socket.");
            freeaddrinfo(res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... allocated socket");

        if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
            ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
            close(s);
            freeaddrinfo(res);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }

        ESP_LOGI(TAG, "... connected");
        freeaddrinfo(res);
        

        // if (write(s, REQUEST, strlen(REQUEST)) < 0) {
        //     ESP_LOGE(TAG, "... socket send failed");
        //     close(s);
        //     vTaskDelay(4000 / portTICK_PERIOD_MS);
        //     continue;
        // }
        // ESP_LOGI(TAG, "... socket send success");
        int i;
        int c = 1;
        while (c != 11){

        
            for (i = 1; i <= c; ++i) // send c requests 
            {
                if (write(s, REQUEST, strlen(REQUEST)) < 0) {
                ESP_LOGE(TAG, "... socket send failed");
                close(s);
                vTaskDelay(0 / portTICK_PERIOD_MS);
                continue;
                }
                ESP_LOGI(TAG, "... socket send success");
                vTaskDelay(0 / portTICK_PERIOD_MS); //sleep timer in ms
            
        
        struct timeval receiving_timeout;
        receiving_timeout.tv_sec = 1;
        receiving_timeout.tv_usec = 0;
        if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                sizeof(receiving_timeout)) < 0) {
            ESP_LOGE(TAG, "... failed to set socket receiving timeout");
            close(s);
            vTaskDelay(0 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... set socket receiving timeout success");

            }

            c++;
            vTaskDelay(0 / portTICK_PERIOD_MS);
        }

        /* Read HTTP response */
        do {
            bzero(recv_buf, sizeof(recv_buf));
            r = read(s, recv_buf, sizeof(recv_buf)-1);
            for(int i = 0; i < r; i++) {
                putchar(recv_buf[i]);
            }
        } while(r > 0);

        //     }

        //     c++;
        //     vTaskDelay(0 / portTICK_PERIOD_MS);
        // }

        ESP_LOGI(TAG, "... done reading from socket. errno=%d.", errno);
        close(s);
        for(int countdown = 15; countdown >= 0; countdown--) {
            ESP_LOGI(TAG, "%d... ", countdown);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "Starting again!");
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    // VVV
    //esp_netif_t netif;
    //esp_err_t esp_netif_get_netif_impl_name(&netif, "STA");
    //esp_netif_dhcpc_stop(&netif);
    // esp_netif_ip_info_t ip_info;
    // esp_netif_get_ip_info(&netif, &ip_info);
    // esp_netif_set_ip4_addr(&ip_info.netmask, 255,255,255,255);
    // esp_netif_set_ip_info(&netif, &ip_info);
    // esp_netif_destroy(&netif);
    // ^^^
    esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
     
    if (netif) {
        esp_netif_ip_info_t ip_info;
        esp_netif_get_ip_info(netif, &ip_info);
        esp_netif_dhcpc_stop(netif);
        esp_netif_set_ip4_addr(&ip_info.netmask, 255,255,255,255);
        esp_netif_set_ip_info(netif, &ip_info);
    }
    
    
    ESP_LOGI(TAG, "IP4 address set successfuly");
    //ESP_LOGI(TAG, "- IPv4 address: " IPSTR, IP2STR(&ip_info.ip));
    
   


    xTaskCreate(&http_get_task, "http_get_task", 4096, NULL, 5, NULL);
}
