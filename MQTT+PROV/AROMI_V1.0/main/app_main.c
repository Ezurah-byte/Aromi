#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include "esp_system.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "mqtt_client.h"

#include "driver/i2c.h"
#include "driver/gpio.h"
#include "driver/rmt.h"
#include "led_strip.h"
#include <math.h>

#include <wifi_provisioning/manager.h>

#include "pn532.h"

#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
#include <wifi_provisioning/scheme_ble.h>
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */
#include "qrcode.h"

static const char *TAG = "app";

//#define IR_SENSOR_PIN GPIO_NUM_4  // Define the GPIO pin connected to the IR sensor
#define RMT_TX_CHANNEL RMT_CHANNEL_0

#define NUM_LEDS 15
#define LED_STRIP_1_PIN GPIO_NUM_15
#define LED_STRIP_2_PIN GPIO_NUM_21
#define LED_STRIP_3_PIN GPIO_NUM_22
#define LED_STRIP_4_PIN GPIO_NUM_12

//Define SPI configurations for each PN532 reader
#define PN532_SCK_1 18
#define PN532_MOSI_1 23
#define PN532_SS_1 25
#define PN532_MISO_1 19

#define PN532_SCK_2 18
#define PN532_MOSI_2 23
#define PN532_SS_2 26
#define PN532_MISO_2 19

#define PN532_SCK_3 18
#define PN532_MOSI_3 23
#define PN532_SS_3 27
#define PN532_MISO_3 19

#define PN532_SCK_4 18
#define PN532_MOSI_4 23
#define PN532_SS_4 13
#define PN532_MISO_4 19

#define LED_STRIP_COUNT 4

static led_strip_t *strips[LED_STRIP_COUNT];

typedef enum {
    SPICE_LEVEL_MILD,
    SPICE_LEVEL_MEDIUM,
    SPICE_LEVEL_HOT,
    SPICE_LEVEL_EXTRA_HOT
} spice_level_t;


bool flag2=true;
#define NUM_READINGS 4
float weighttoserver[NUM_READINGS] = {0};
#define TAG2 "WIFI"
int32_t reading ;
float weight ;
float weight1;
float weight2;
float weight3;
float weight4;
char buffer1[40];
char buffer2[40];
char buffer3[40];
char buffer4[40];
uint8_t receiver_macs[][6] = {
    {0xD0, 0xEF, 0x76, 0x5D, 0x94, 0x18},
    {0xC8, 0xF0, 0x9E, 0xF2, 0x73, 0x44},
    {0x40, 0x22, 0xD8, 0xDE, 0xC2, 0x30},
    {0xD4, 0x8A, 0xFC, 0xA0, 0x23, 0x28}
};

static pn532_t nfc1, nfc2 ,nfc3 ,nfc4; // Define separate instances for each NFC reader

// Define the GPIO pins for SCK and DOUT
#define HX711_SCK_PIN1 GPIO_NUM_33
#define HX711_DOUT_PIN1 GPIO_NUM_32
#define HX711_SCK_PIN2 GPIO_NUM_16
#define HX711_DOUT_PIN2 GPIO_NUM_17
#define HX711_SCK_PIN3 GPIO_NUM_5
#define HX711_DOUT_PIN3 GPIO_NUM_4
#define HX711_SCK_PIN4 GPIO_NUM_0
#define HX711_DOUT_PIN4 GPIO_NUM_2
static void hx711_init() {
    gpio_set_direction(HX711_SCK_PIN1, GPIO_MODE_OUTPUT);
    gpio_set_direction(HX711_DOUT_PIN1, GPIO_MODE_INPUT);
    gpio_set_direction(HX711_SCK_PIN2, GPIO_MODE_OUTPUT);
    gpio_set_direction(HX711_DOUT_PIN2, GPIO_MODE_INPUT);
    gpio_set_direction(HX711_SCK_PIN3, GPIO_MODE_OUTPUT);
    gpio_set_direction(HX711_DOUT_PIN3, GPIO_MODE_INPUT);
    gpio_set_direction(HX711_SCK_PIN4, GPIO_MODE_OUTPUT);
    gpio_set_direction(HX711_DOUT_PIN4, GPIO_MODE_INPUT);
}

#if CONFIG_EXAMPLE_PROV_SECURITY_VERSION_2
#if CONFIG_EXAMPLE_PROV_SEC2_DEV_MODE
#define EXAMPLE_PROV_SEC2_USERNAME          "wifiprov"
#define EXAMPLE_PROV_SEC2_PWD               "abcd1234"

/* This salt,verifier has been generated for username = "wifiprov" and password = "abcd1234"
 * IMPORTANT NOTE: For production cases, this must be unique to every device
 * and should come from device manufacturing partition.*/
static const char sec2_salt[] = {
    0x03, 0x6e, 0xe0, 0xc7, 0xbc, 0xb9, 0xed, 0xa8, 0x4c, 0x9e, 0xac, 0x97, 0xd9, 0x3d, 0xec, 0xf4
};

static const char sec2_verifier[] = {
    0x7c, 0x7c, 0x85, 0x47, 0x65, 0x08, 0x94, 0x6d, 0xd6, 0x36, 0xaf, 0x37, 0xd7, 0xe8, 0x91, 0x43,
    0x78, 0xcf, 0xfd, 0x61, 0x6c, 0x59, 0xd2, 0xf8, 0x39, 0x08, 0x12, 0x72, 0x38, 0xde, 0x9e, 0x24,
    0xa4, 0x70, 0x26, 0x1c, 0xdf, 0xa9, 0x03, 0xc2, 0xb2, 0x70, 0xe7, 0xb1, 0x32, 0x24, 0xda, 0x11,
    0x1d, 0x97, 0x18, 0xdc, 0x60, 0x72, 0x08, 0xcc, 0x9a, 0xc9, 0x0c, 0x48, 0x27, 0xe2, 0xae, 0x89,
    0xaa, 0x16, 0x25, 0xb8, 0x04, 0xd2, 0x1a, 0x9b, 0x3a, 0x8f, 0x37, 0xf6, 0xe4, 0x3a, 0x71, 0x2e,
    0xe1, 0x27, 0x86, 0x6e, 0xad, 0xce, 0x28, 0xff, 0x54, 0x46, 0x60, 0x1f, 0xb9, 0x96, 0x87, 0xdc,
    0x57, 0x40, 0xa7, 0xd4, 0x6c, 0xc9, 0x77, 0x54, 0xdc, 0x16, 0x82, 0xf0, 0xed, 0x35, 0x6a, 0xc4,
    0x70, 0xad, 0x3d, 0x90, 0xb5, 0x81, 0x94, 0x70, 0xd7, 0xbc, 0x65, 0xb2, 0xd5, 0x18, 0xe0, 0x2e,
    0xc3, 0xa5, 0xf9, 0x68, 0xdd, 0x64, 0x7b, 0xb8, 0xb7, 0x3c, 0x9c, 0xfc, 0x00, 0xd8, 0x71, 0x7e,
    0xb7, 0x9a, 0x7c, 0xb1, 0xb7, 0xc2, 0xc3, 0x18, 0x34, 0x29, 0x32, 0x43, 0x3e, 0x00, 0x99, 0xe9,
    0x82, 0x94, 0xe3, 0xd8, 0x2a, 0xb0, 0x96, 0x29, 0xb7, 0xdf, 0x0e, 0x5f, 0x08, 0x33, 0x40, 0x76,
    0x52, 0x91, 0x32, 0x00, 0x9f, 0x97, 0x2c, 0x89, 0x6c, 0x39, 0x1e, 0xc8, 0x28, 0x05, 0x44, 0x17,
    0x3f, 0x68, 0x02, 0x8a, 0x9f, 0x44, 0x61, 0xd1, 0xf5, 0xa1, 0x7e, 0x5a, 0x70, 0xd2, 0xc7, 0x23,
    0x81, 0xcb, 0x38, 0x68, 0xe4, 0x2c, 0x20, 0xbc, 0x40, 0x57, 0x76, 0x17, 0xbd, 0x08, 0xb8, 0x96,
    0xbc, 0x26, 0xeb, 0x32, 0x46, 0x69, 0x35, 0x05, 0x8c, 0x15, 0x70, 0xd9, 0x1b, 0xe9, 0xbe, 0xcc,
    0xa9, 0x38, 0xa6, 0x67, 0xf0, 0xad, 0x50, 0x13, 0x19, 0x72, 0x64, 0xbf, 0x52, 0xc2, 0x34, 0xe2,
    0x1b, 0x11, 0x79, 0x74, 0x72, 0xbd, 0x34, 0x5b, 0xb1, 0xe2, 0xfd, 0x66, 0x73, 0xfe, 0x71, 0x64,
    0x74, 0xd0, 0x4e, 0xbc, 0x51, 0x24, 0x19, 0x40, 0x87, 0x0e, 0x92, 0x40, 0xe6, 0x21, 0xe7, 0x2d,
    0x4e, 0x37, 0x76, 0x2f, 0x2e, 0xe2, 0x68, 0xc7, 0x89, 0xe8, 0x32, 0x13, 0x42, 0x06, 0x84, 0x84,
    0x53, 0x4a, 0xb3, 0x0c, 0x1b, 0x4c, 0x8d, 0x1c, 0x51, 0x97, 0x19, 0xab, 0xae, 0x77, 0xff, 0xdb,
    0xec, 0xf0, 0x10, 0x95, 0x34, 0x33, 0x6b, 0xcb, 0x3e, 0x84, 0x0f, 0xb9, 0xd8, 0x5f, 0xb8, 0xa0,
    0xb8, 0x55, 0x53, 0x3e, 0x70, 0xf7, 0x18, 0xf5, 0xce, 0x7b, 0x4e, 0xbf, 0x27, 0xce, 0xce, 0xa8,
    0xb3, 0xbe, 0x40, 0xc5, 0xc5, 0x32, 0x29, 0x3e, 0x71, 0x64, 0x9e, 0xde, 0x8c, 0xf6, 0x75, 0xa1,
    0xe6, 0xf6, 0x53, 0xc8, 0x31, 0xa8, 0x78, 0xde, 0x50, 0x40, 0xf7, 0x62, 0xde, 0x36, 0xb2, 0xba
};
#endif

static esp_err_t example_get_sec2_salt(const char **salt, uint16_t *salt_len) {
#if CONFIG_EXAMPLE_PROV_SEC2_DEV_MODE
    ESP_LOGI(TAG, "Development mode: using hard coded salt");
    *salt = sec2_salt;
    *salt_len = sizeof(sec2_salt);
    return ESP_OK;
#elif CONFIG_EXAMPLE_PROV_SEC2_PROD_MODE
    ESP_LOGE(TAG, "Not implemented!");
    return ESP_FAIL;
#endif
}

static esp_err_t example_get_sec2_verifier(const char **verifier, uint16_t *verifier_len) {
#if CONFIG_EXAMPLE_PROV_SEC2_DEV_MODE
    ESP_LOGI(TAG, "Development mode: using hard coded verifier");
    *verifier = sec2_verifier;
    *verifier_len = sizeof(sec2_verifier);
    return ESP_OK;
#elif CONFIG_EXAMPLE_PROV_SEC2_PROD_MODE
    /* This code needs to be updated with appropriate implementation to provide verifier */
    ESP_LOGE(TAG, "Not implemented!");
    return ESP_FAIL;
#endif
}
#endif

/* Signal Wi-Fi events on this event-group */
const int WIFI_CONNECTED_EVENT = BIT0;
static EventGroupHandle_t wifi_event_group;

#define PROV_QR_VERSION         "v1"
#define PROV_TRANSPORT_BLE      "ble"
#define QRCODE_BASE_URL         "https://espressif.github.io/esp-jumpstart/qrcode.html"

/* Event handler for catching system events */
static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
#ifdef CONFIG_EXAMPLE_RESET_PROV_MGR_ON_FAILURE
    static int retries;
#endif
    if (event_base == WIFI_PROV_EVENT) {
        switch (event_id) {
            case WIFI_PROV_START:
                ESP_LOGI(TAG, "Provisioning started");
                break;
            case WIFI_PROV_CRED_RECV: {
                wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *)event_data;
                ESP_LOGI(TAG, "Received Wi-Fi credentials"
                         "\n\tSSID     : %s\n\tPassword : %s",
                         (const char *) wifi_sta_cfg->ssid,
                         (const char *) wifi_sta_cfg->password);
                break;
            }
            case WIFI_PROV_CRED_FAIL: {
                wifi_prov_sta_fail_reason_t *reason = (wifi_prov_sta_fail_reason_t *)event_data;
                ESP_LOGE(TAG, "Provisioning failed!\n\tReason : %s"
                         "\n\tPlease reset to factory and retry provisioning",
                         (*reason == WIFI_PROV_STA_AUTH_ERROR) ?
                         "Wi-Fi station authentication failed" : "Wi-Fi access-point not found");
#ifdef CONFIG_EXAMPLE_RESET_PROV_MGR_ON_FAILURE
                retries++;
                if (retries >= CONFIG_EXAMPLE_PROV_MGR_MAX_RETRY_CNT) {
                    ESP_LOGI(TAG, "Failed to connect with provisioned AP, reseting provisioned credentials");
                    wifi_prov_mgr_reset_sm_state_on_failure();
                    retries = 0;
                }
#endif
                break;
            }
            case WIFI_PROV_CRED_SUCCESS:
                ESP_LOGI(TAG, "Provisioning successful");
#ifdef CONFIG_EXAMPLE_RESET_PROV_MGR_ON_FAILURE
                retries = 0;
#endif
                break;
            case WIFI_PROV_END:
                /* De-initialize manager once provisioning is finished */
                wifi_prov_mgr_deinit();
                break;
            default:
                break;
        }
    } else if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGI(TAG, "Disconnected. Connecting to the AP again...");
                esp_wifi_connect();
                break;
            default:
                break;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Connected with IP Address:" IPSTR, IP2STR(&event->ip_info.ip));
        /* Signal main application to continue execution */
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_EVENT);
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
    } else if (event_base == PROTOCOMM_TRANSPORT_BLE_EVENT) {
        switch (event_id) {
            case PROTOCOMM_TRANSPORT_BLE_CONNECTED:
                ESP_LOGI(TAG, "BLE transport: Connected!");
                break;
            case PROTOCOMM_TRANSPORT_BLE_DISCONNECTED:
                ESP_LOGI(TAG, "BLE transport: Disconnected!");
                break;
            default:
                break;
        }
#endif
    } else if (event_base == PROTOCOMM_SECURITY_SESSION_EVENT) {
        switch (event_id) {
            case PROTOCOMM_SECURITY_SESSION_SETUP_OK:
                ESP_LOGI(TAG, "Secured session established!");
                break;
            case PROTOCOMM_SECURITY_SESSION_INVALID_SECURITY_PARAMS:
                ESP_LOGE(TAG, "Received invalid security parameters for establishing secure session!");
                break;
            case PROTOCOMM_SECURITY_SESSION_CREDENTIALS_MISMATCH:
                ESP_LOGE(TAG, "Received incorrect username and/or PoP for establishing secure session!");
                break;
            default:
                break;
        }
    }
}

static void wifi_init_sta(void)
{
    /* Start Wi-Fi in station mode */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void get_device_service_name(char *service_name, size_t max)
{
    uint8_t eth_mac[6];
    const char *ssid_prefix = "AROMI_";
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    snprintf(service_name, max, "%s%02X%02X%02X",
             ssid_prefix, eth_mac[3], eth_mac[4], eth_mac[5]);
}

/* Handler for the optional provisioning endpoint registered by the application.
 * The data format can be chosen by applications. Here, we are using plain ascii text.
 * Applications can choose to use other formats like protobuf, JSON, XML, etc.
 */
esp_err_t custom_prov_data_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
                                          uint8_t **outbuf, ssize_t *outlen, void *priv_data)
{
    if (inbuf) {
        ESP_LOGI(TAG, "Received data: %.*s", inlen, (char *)inbuf);
    }
    char response[] = "SUCCESS";
    *outbuf = (uint8_t *)strdup(response);
    if (*outbuf == NULL) {
        ESP_LOGE(TAG, "System out of memory");
        return ESP_ERR_NO_MEM;
    }
    *outlen = strlen(response) + 1; // +1 for NULL terminating byte */

    return ESP_OK;
}

static void wifi_prov_print_qr(const char *name, const char *username, const char *pop, const char *transport)
{
    if (!name || !transport) {
        ESP_LOGW(TAG, "Cannot generate QR code payload. Data missing.");
        return;
    }
    char payload[150] = {0};
    if (pop) {
#if CONFIG_EXAMPLE_PROV_SECURITY_VERSION_1
        snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\"" \
                    ",\"pop\":\"%s\",\"transport\":\"%s\"}",
                    PROV_QR_VERSION, name, pop, transport);
#elif CONFIG_EXAMPLE_PROV_SECURITY_VERSION_2
        snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\"" \
                    ",\"username\":\"%s\",\"pop\":\"%s\",\"transport\":\"%s\"}",
                    PROV_QR_VERSION, name, username, pop, transport);
#endif
    } else {
        snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\"" \
                    ",\"transport\":\"%s\"}",
                    PROV_QR_VERSION, name, transport);
    }
#ifdef CONFIG_EXAMPLE_PROV_SHOW_QR
    ESP_LOGI(TAG, "Scan this QR code from the provisioning application for Provisioning.");
    esp_qrcode_config_t cfg = ESP_QRCODE_CONFIG_DEFAULT();
    esp_qrcode_generate(&cfg, payload);
#endif /* CONFIG_APP_WIFI_PROV_SHOW_QR */
    ESP_LOGI(TAG, "If QR code is not visible, copy paste the below URL in a browser.\n%s?data=%s", QRCODE_BASE_URL, payload);
}

esp_mqtt_client_handle_t mqtt_client = NULL;  // MQTT client handle

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%ld", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "TEST", 0, 1, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
        ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "BROKER_URL",
        .credentials.username = "USER_NAME",
        .credentials.authentication.password = "PASSWORD",
        //    /topic/qos1
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

void PROVI(){
     /* Initialize NVS partition */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        /* NVS partition was truncated
         * and needs to be erased */
        ESP_ERROR_CHECK(nvs_flash_erase());

        /* Retry nvs_flash_init */
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    /* Initialize TCP/IP */
    ESP_ERROR_CHECK(esp_netif_init());

    /* Initialize the event loop */
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_event_group = xEventGroupCreate();

    /* Register our event handler for Wi-Fi, IP and Provisioning related events */
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
    ESP_ERROR_CHECK(esp_event_handler_register(PROTOCOMM_TRANSPORT_BLE_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
#endif
    ESP_ERROR_CHECK(esp_event_handler_register(PROTOCOMM_SECURITY_SESSION_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    /* Initialize Wi-Fi including netif with default config */
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* Configuration for the provisioning manager */
    wifi_prov_mgr_config_t config = {
        /* What is the Provisioning Scheme that we want ?
         * wifi_prov_scheme_ble */
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
        .scheme = wifi_prov_scheme_ble,
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */

        /* Any default scheme specific event handler that you would
         * like to choose. Since our example application requires
         * neither BT nor BLE, we can choose to release the associated
         * memory once provisioning is complete, or not needed
         * (in case when device is already provisioned). Choosing
         * appropriate scheme specific event handler allows the manager
         * to take care of this automatically. This can be set to
         * WIFI_PROV_EVENT_HANDLER_NONE when using wifi_prov_scheme_ble*/
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
        .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */
    };

    /* Initialize provisioning manager with the
     * configuration parameters set above */
    ESP_ERROR_CHECK(wifi_prov_mgr_init(config));

    bool provisioned = false;
#ifdef CONFIG_EXAMPLE_RESET_PROVISIONED
    wifi_prov_mgr_reset_provisioning();
#else
    /* Let's find out if the device is provisioned */
    ESP_ERROR_CHECK(wifi_prov_mgr_is_provisioned(&provisioned));

#endif
    /* If device is not yet provisioned start provisioning service */
    if (0) {
        ESP_LOGI(TAG, "Starting provisioning");

        /* What is the Device Service Name that we want
         * This translates to :
         *     - device name when scheme is wifi_prov_scheme_ble
         */
        char service_name[12];
        get_device_service_name(service_name, sizeof(service_name));

#ifdef CONFIG_EXAMPLE_PROV_SECURITY_VERSION_1
        /* What is the security level that we want (0, 1, 2):
         *      - WIFI_PROV_SECURITY_0 is simply plain text communication.
         *      - WIFI_PROV_SECURITY_1 is secure communication which consists of secure handshake
         *          using X25519 key exchange and proof of possession (pop) and AES-CTR
         *          for encryption/decryption of messages.
         *      - WIFI_PROV_SECURITY_2 SRP6a based authentication and key exchange
         *        + AES-GCM encryption/decryption of messages
         */
        wifi_prov_security_t security = WIFI_PROV_SECURITY_1;

        /* Do we want a proof-of-possession (ignored if Security 0 is selected):
         *      - this should be a string with length > 0
         *      - NULL if not used
         */
        const char *pop = "abcd1234";

        /* This is the structure for passing security parameters
         * for the protocomm security 1.
         */
        wifi_prov_security1_params_t *sec_params = pop;

        const char *username  = NULL;

#elif CONFIG_EXAMPLE_PROV_SECURITY_VERSION_2
        wifi_prov_security_t security = WIFI_PROV_SECURITY_2;
        /* The username must be the same one, which has been used in the generation of salt and verifier */

#if CONFIG_EXAMPLE_PROV_SEC2_DEV_MODE
        /* This pop field represents the password that will be used to generate salt and verifier.
         * The field is present here in order to generate the QR code containing password.
         * In production this password field shall not be stored on the device */
        const char *username  = EXAMPLE_PROV_SEC2_USERNAME;
        const char *pop = EXAMPLE_PROV_SEC2_PWD;
#elif CONFIG_EXAMPLE_PROV_SEC2_PROD_MODE
        /* The username and password shall not be embedded in the firmware,
         * they should be provided to the user by other means.
         * e.g. QR code sticker */
        const char *username  = NULL;
        const char *pop = NULL;
#endif
        /* This is the structure for passing security parameters
         * for the protocomm security 2.
         * If dynamically allocated, sec2_params pointer and its content
         * must be valid till WIFI_PROV_END event is triggered.
         */
        wifi_prov_security2_params_t sec2_params = {};

        ESP_ERROR_CHECK(example_get_sec2_salt(&sec2_params.salt, &sec2_params.salt_len));
        ESP_ERROR_CHECK(example_get_sec2_verifier(&sec2_params.verifier, &sec2_params.verifier_len));

        wifi_prov_security2_params_t *sec_params = &sec2_params;
#endif
        /* What is the service key (could be NULL)
         * This translates to :
         *     - simply ignored when scheme is wifi_prov_scheme_ble
         */
        const char *service_key = NULL;

#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
        /* This step is only useful when scheme is wifi_prov_scheme_ble. This will
         * set a custom 128 bit UUID which will be included in the BLE advertisement
         * and will correspond to the primary GATT service that provides provisioning
         * endpoints as GATT characteristics. Each GATT characteristic will be
         * formed using the primary service UUID as base, with different auto assigned
         * 12th and 13th bytes (assume counting starts from 0th byte). The client side
         * applications must identify the endpoints by reading the User Characteristic
         * Description descriptor (0x2901) for each characteristic, which contains the
         * endpoint name of the characteristic */
        uint8_t custom_service_uuid[] = {
            /* LSB <---------------------------------------
             * ---------------------------------------> MSB */
            0xb4, 0xdf, 0x5a, 0x1c, 0x3f, 0x6b, 0xf4, 0xbf,
            0xea, 0x4a, 0x82, 0x03, 0x04, 0x90, 0x1a, 0x02,
        };

        /* If your build fails with linker errors at this point, then you may have
         * forgotten to enable the BT stack or BTDM BLE settings in the SDK (e.g. see
         * the sdkconfig.defaults in the example project) */
        wifi_prov_scheme_ble_set_service_uuid(custom_service_uuid);
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */

        /* An optional endpoint that applications can create if they expect to
         * get some additional custom data during provisioning workflow.
         * The endpoint name can be anything of your choice.
         * This call must be made before starting the provisioning.
         */
        wifi_prov_mgr_endpoint_create("custom-data");

        /* Do not stop and de-init provisioning even after success,
         * so that we can restart it later. */
#ifdef CONFIG_EXAMPLE_REPROVISIONING
        wifi_prov_mgr_disable_auto_stop(1000);
#endif
        /* Start provisioning service */
        ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(security, (const void *) sec_params, service_name, service_key));

        /* The handler for the optional endpoint created above.
         * This call must be made after starting the provisioning, and only if the endpoint
         * has already been created above.
         */
        wifi_prov_mgr_endpoint_register("custom-data", custom_prov_data_handler, NULL);

        /* Uncomment the following to wait for the provisioning to finish and then release
         * the resources of the manager. Since in this case de-initialization is triggered
         * by the default event loop handler, we don't need to call the following */
        // wifi_prov_mgr_wait();
        // wifi_prov_mgr_deinit();
        /* Print QR code for provisioning */
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
        wifi_prov_print_qr(service_name, username, pop, PROV_TRANSPORT_BLE);
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */
    } else {
        ESP_LOGI(TAG, "Already provisioned, starting Wi-Fi STA");

        /* We don't need the manager as device is already provisioned,
         * so let's release it's resources */
        wifi_prov_mgr_deinit();

        /* Start Wi-Fi station */
        wifi_init_sta();
    }

    /* Wait for Wi-Fi connection */
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_EVENT, true, true, portMAX_DELAY);

    /* Start main application now */
#if CONFIG_EXAMPLE_REPROVISIONING
    while (1) {
        for (int i = 0; i < 10; i++) {
            ESP_LOGI(TAG, "Hello World!");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        /* Resetting provisioning state machine to enable re-provisioning */
        wifi_prov_mgr_reset_sm_state_for_reprovision();

        /* Wait for Wi-Fi connection */
        xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_EVENT, true, true, portMAX_DELAY);
    }
#else
     if(BIT0 ==1) {
         ESP_LOGI(TAG, "PROVI END!");
         vTaskDelay(50 / portTICK_PERIOD_MS);
     }
#endif

}

void MQTT(void *pvParameters)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %lu bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);
    


    ESP_ERROR_CHECK(nvs_flash_init());

    mqtt_app_start(); // Initialize and start MQTT

    while (1) {

        if (mqtt_client != NULL) {
            esp_mqtt_client_publish(mqtt_client, "/topic/qos1", "message", 0, 1, 0);
        }

        // Wait for 1 second
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void setup_led_strip(led_strip_t **strip, gpio_num_t pin, rmt_channel_t channel) {
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(pin, channel);
    config.clk_div = 2;
    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(channel, 0, 0));
    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(NUM_LEDS, (led_strip_dev_t)channel);
    *strip = led_strip_new_rmt_ws2812(&strip_config);
    if (!(*strip)) {
        ESP_LOGE("LED", "Failed to initialize LED strip on GPIO %d", pin);
    }
}

void mild_pattern(led_strip_t *strip) {
    for (int i = 0; i < NUM_LEDS; i++) {
        ESP_ERROR_CHECK(strip->set_pixel(strip, i, (i % 2 == 0) ? 0 : 0, (i % 2 == 0) ? 255 : 0, (i % 2 == 0) ? 0 : 0));
    }
    ESP_ERROR_CHECK(strip->refresh(strip, 100));
}

void medium_pattern(led_strip_t *strip) {
    for (int offset = 0; offset < NUM_LEDS; offset++) {
        for (int j = 0; j < NUM_LEDS; j++) {
            ESP_ERROR_CHECK(strip->set_pixel(strip, j, ((j + offset) % NUM_LEDS < 5) ? 255 : 0, ((j + offset) % NUM_LEDS < 5) ? 255 : 0, 0));
        }
        ESP_ERROR_CHECK(strip->refresh(strip, 100));
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void hot_pattern(led_strip_t *strip) {
    for (int intensity = 0; intensity <= 255; intensity += 5) {
        for (int j = 0; j < NUM_LEDS; j++) {
            ESP_ERROR_CHECK(strip->set_pixel(strip, j, 255 * intensity / 255, 165 * intensity / 255, 0));
        }
        ESP_ERROR_CHECK(strip->refresh(strip, 100));
        vTaskDelay(pdMS_TO_TICKS(30));
    }
    for (int intensity = 255; intensity >= 0; intensity -= 5) {
        for (int j = 0; j < NUM_LEDS; j++) {
            ESP_ERROR_CHECK(strip->set_pixel(strip, j, 255 * intensity / 255, 165 * intensity / 255, 0));
        }
        ESP_ERROR_CHECK(strip->refresh(strip, 100));
        vTaskDelay(pdMS_TO_TICKS(30));
    }
}

void extra_hot_pattern(led_strip_t *strip) {
    for (int blink = 0; blink < 5; blink++) {
        for (int j = 0; j < NUM_LEDS; j++) {
            ESP_ERROR_CHECK(strip->set_pixel(strip, j, 255, 0, 0)); // Red
        }
        ESP_ERROR_CHECK(strip->refresh(strip, 100));
        vTaskDelay(pdMS_TO_TICKS(500));
        ESP_ERROR_CHECK(strip->clear(strip, 100));
        ESP_ERROR_CHECK(strip->refresh(strip, 100));
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void indicate_spice_level(led_strip_t *strip, spice_level_t level) {
    switch (level) {
        case SPICE_LEVEL_MILD:
            mild_pattern(strip);
            break;
        case SPICE_LEVEL_MEDIUM:
            medium_pattern(strip);
            break;
        case SPICE_LEVEL_HOT:
            hot_pattern(strip);
            break;
        case SPICE_LEVEL_EXTRA_HOT:
            extra_hot_pattern(strip);
            break;
        default:
            ESP_LOGE("LED", "Unknown spice level");
            break;
    }
}

void led_strip()
{
    setup_led_strip(&strips[0], LED_STRIP_1_PIN, RMT_CHANNEL_0);
    setup_led_strip(&strips[1], LED_STRIP_2_PIN, RMT_CHANNEL_1);
    setup_led_strip(&strips[2], LED_STRIP_3_PIN, RMT_CHANNEL_2);
    setup_led_strip(&strips[3], LED_STRIP_4_PIN, RMT_CHANNEL_3);

        while (true) {
        // Example usage
        indicate_spice_level(strips[0], SPICE_LEVEL_MILD);
        indicate_spice_level(strips[1], SPICE_LEVEL_MEDIUM);
        indicate_spice_level(strips[2], SPICE_LEVEL_HOT);
        indicate_spice_level(strips[3], SPICE_LEVEL_EXTRA_HOT);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

}

void init_espnow(void) {
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(espnow_send_cb));
    ESP_ERROR_CHECK(esp_now_set_pmk((uint8_t *)"KATOMARAN")); // Primary Master Key
}

void add_peer(const uint8_t *peer_mac) {
    esp_now_peer_info_t peer_info = {};
    memcpy(peer_info.peer_addr, peer_mac, 6);
    peer_info.channel = 0; // Use the same channel as WiFi
    peer_info.encrypt = false;

    if (esp_now_add_peer(&peer_info) != ESP_OK) {
        ESP_LOGE(TAG1, "Failed to add peer with MAC: %02X:%02X:%02X:%02X:%02X:%02X",
                 peer_mac[0], peer_mac[1], peer_mac[2], peer_mac[3], peer_mac[4], peer_mac[5]);
    }
}

void send_espnow_data(int index,const char *data) {
    if (index < 0 || index >= sizeof(receiver_macs) / sizeof(receiver_macs[0])) {
        ESP_LOGE(TAG1, "Invalid index for sending data.");
        return;
    }
    esp_err_t result = esp_now_send(receiver_macs[index], (uint8_t *)data, strlen(data));

    if (result == ESP_OK) {
        ESP_LOGI(TAG1, "Send Success to peer %d: %s", index, data);
    } else {
        ESP_LOGE(TAG1, "Send Fail to peer %d: %s", index, data);
    }
}
static void wifi_init_sta(void)
{
    /* Start Wi-Fi in station mode */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NAN));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static int32_t hx711_readone() {
    int32_t count = 0;
    while (gpio_get_level(HX711_DOUT_PIN1) == 1) {
        // Wait for DOUT to go low
    }

    for (int i = 0; i < 24; i++) {
        gpio_set_level(HX711_SCK_PIN1, 1);
        count = count << 1;
        gpio_set_level(HX711_SCK_PIN1, 0);
        if (gpio_get_level(HX711_DOUT_PIN1) == 1) {
            count++;
        }
    }

    // Set the gain to 128 and read the data
    gpio_set_level(HX711_SCK_PIN1, 1);
    vTaskDelay(20);
    gpio_set_level(HX711_SCK_PIN1, 0);
    vTaskDelay(20);
    // Convert 24-bit two's complement to 32-bit two's complement
    count = count ^ 0x800000;
    return count;
}
 int32_t cellonereturn(){
     reading = hx711_readone();
    //   printf( "------------------------------------\n");
    //   printf( "Raw reading1: %ld\n", reading);
        float K1=8391799;
        float K2=222;
        float K3=8481601;
        float K4;
        K4=K2/(K3-K1);
        weight = K4*(reading-K1);
        printf( "weight1 %lf\n",weight );
        // printf( "------------------------------------\n");
        return weight;
        vTaskDelay(500);
}

static int32_t hx711_readtwo() {
    int32_t count = 0;
    while (gpio_get_level(HX711_DOUT_PIN2) == 1) {
        // Wait for DOUT to go low
    }

    for (int i = 0; i < 24; i++) {
        gpio_set_level(HX711_SCK_PIN2, 1);
        count = count << 1;
        gpio_set_level(HX711_SCK_PIN2, 0);
        if (gpio_get_level(HX711_DOUT_PIN2) == 1) {
            count++;
        }
    }

    // Set the gain to 128 and read the data
    gpio_set_level(HX711_SCK_PIN2, 1);
    count = count ^ 0x800000;  // Convert 24-bit two's complement to 32-bit two's complement
    gpio_set_level(HX711_SCK_PIN2, 0);

    return count;
}
int32_t celltworeturn()
{
   while(1){
      reading = hx711_readtwo();
    //   printf( "------------------------------------\n");
    //   printf( "Raw reading2: %ld\n", reading);
        float K1=8440115;
        float K2=222;
        float K3=8442266;
        float K4;
        K4=K2/(K3-K1);
        weight = K4*(reading-K1);
        // printf( "weight2 %lf\n",weight );
         return weight;
        // printf( "------------------------------------\n");
        vTaskDelay(500);
    }
}
static int32_t hx711_readthree() {
    int32_t count = 0;
    while (gpio_get_level(HX711_DOUT_PIN3) == 1) {
        // Wait for DOUT to go low
    }
    for (int i = 0; i < 24; i++) {
        gpio_set_level(HX711_SCK_PIN3, 1);
        count = count << 1;
        gpio_set_level(HX711_SCK_PIN3, 0);
        if (gpio_get_level(HX711_DOUT_PIN3) == 1) {
            count++;
        }
    }
    // Set the gain to 128 and read the data
    gpio_set_level(HX711_SCK_PIN3, 1);
    count = count ^ 0x800000;  // Convert 24-bit two's complement to 32-bit two's complement
    gpio_set_level(HX711_SCK_PIN3, 0);

    return count;
}
int32_t cellthreereturn()
{
   while(1){
      reading = hx711_readthree();
    //   printf( "------------------------------------\n");
    //   printf( "Raw reading3: %ld\n", reading);
        float K1=8391799;
        float K2=222;
        float K3=8481601;
        float K4;
        K4=K2/(K3-K1);
        weight = K4*(reading-K1);
        // printf( "weight3 %lf\n",weight );
        return weight;
        // printf( "------------------------------------\n");
        vTaskDelay(500);
    }
}
static int32_t hx711_readfour() {
    int32_t count = 0;
    while (gpio_get_level(HX711_DOUT_PIN4) == 1) {
        // Wait for DOUT to go low
    }

    for (int i = 0; i < 24; i++) {
        gpio_set_level(HX711_SCK_PIN4, 1);
        count = count << 1;
        gpio_set_level(HX711_SCK_PIN4, 0);
        if (gpio_get_level(HX711_DOUT_PIN4) == 1) {
            count++;
        }
    }

    // Set the gain to 128 and read the data
    gpio_set_level(HX711_SCK_PIN4, 1);
    count = count ^ 0x800000;  // Convert 24-bit two's complement to 32-bit two's complement
    gpio_set_level(HX711_SCK_PIN4, 0);

    return count;
}
int32_t cellfourreturn()
{
   while(1){
      reading = hx711_readfour();
    //   printf( "------------------------------------\n");
    //   printf( "Raw reading4: %ld\n", reading);
        float K1=8398303;
        float K2=222;
        float K3=8398368;
        float K4;
        K4=K2/(K3-K1);
        weight = K4*(reading-K1);
        // printf( "weight4 %lf\n",weight );
        return weight;
        // printf( "------------------------------------\n");
        vTaskDelay(500);
    }
}
void Loadcell()
{
   while(1){
       cellonereturn();
       celltworeturn();
       cellthreereturn();
       cellfourreturn();
       vTaskDelay(100);
        }
    
}
void display(void *pvParameters){
  // Adding peers
    for (int i = 0; i < sizeof(receiver_macs) / sizeof(receiver_macs[0]); i++) {
        add_peer(receiver_macs[i]);
        vTaskDelay(pdMS_TO_TICKS(10)); 
        // Delay in milliseconds
    }
    while(1){
     weight1 = cellonereturn();
    //  printf("%lf\n",weight1);
    weighttoserver[0]=weight1;
    sprintf(buffer1,"%lf",weight1);
    // send_espnow_data(0, "SMART TRAY1");
    vTaskDelay(10);
    send_espnow_data(0,"AROMI");
    vTaskDelay(pdMS_TO_TICKS(10));
    weight2=celltworeturn();
    sprintf(buffer2,"%lf",weight2);
    send_espnow_data(1,buffer2);
    weighttoserver[1]=weight2;
    vTaskDelay(pdMS_TO_TICKS(10));
    weight3=cellthreereturn();
    sprintf(buffer3,"%lf",weight3);
    send_espnow_data(2,buffer3);
    weighttoserver[2]=weight3;
    vTaskDelay(pdMS_TO_TICKS(10));
    weight4=cellfourreturn();
    sprintf(buffer4,"%lf",weight4);
    send_espnow_data(3,buffer4);
    weighttoserver[3]=weight4;
    vTaskDelay(1000);
    }
}

void nfcread()
{
    nfc1_task(&nfc1);
    vTaskDelay(100 / portTICK_RATE_MS);
    nfc2_task(&nfc2);
    vTaskDelay(100 / portTICK_RATE_MS);
    nfc3_task(&nfc3);
    vTaskDelay(100 / portTICK_RATE_MS);
    nfc4_task(&nfc4);
    vTaskDelay(100 / portTICK_RATE_MS);
}

static void nfc_int()
{
    pn532_spi_init(&nfc1, PN532_SCK_1, PN532_MISO_1, PN532_MOSI_1, PN532_SS_1);
    pn532_begin(&nfc1);
    pn532_SAMConfig(&nfc1);

    // Initialize second NFC reader
    pn532_spi_init(&nfc2, PN532_SCK_2, PN532_MISO_2, PN532_MOSI_2, PN532_SS_2);
    pn532_begin(&nfc2);
    pn532_SAMConfig(&nfc2);

        // Initialize 3 NFC reader
    pn532_spi_init(&nfc3, PN532_SCK_3, PN532_MISO_3, PN532_MOSI_3, PN532_SS_3);
    pn532_begin(&nfc3);
    pn532_SAMConfig(&nfc3);

    // Initialize 4 NFC reader
    pn532_spi_init(&nfc4, PN532_SCK_4, PN532_MISO_4, PN532_MOSI_4, PN532_SS_4);
    pn532_begin(&nfc4);
    pn532_SAMConfig(&nfc4);

      ESP_LOGI(TAG, "Waiting for ISO14443A Cards ...");
}

void nfc1_task(void *pvParameter)
{
        uint8_t success;
        uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
        uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
        uint32_t timeout = 10;

        // Read from first NFC reader
        success = pn532_readPassiveTargetID(&nfc1, PN532_MIFARE_ISO14443A, uid, &uidLength, timeout);
        if (success)
        {
            ESP_LOGI(TAG, "Reader 1 - Found ISO14443A card");
            ESP_LOGI(TAG, "UID Length: %d bytes", uidLength);
            ESP_LOGI(TAG, "UID Value:");
            esp_log_buffer_hexdump_internal(TAG, uid, uidLength, ESP_LOG_INFO);
        }
        else{
             ESP_LOGW(TAG, "Reader 1 - NOT FOUND");
        }

        vTaskDelay(1000 / portTICK_RATE_MS); // Delay between reads

}

void nfc2_task(void *pvParameter)
{

        uint8_t success;
        uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
        uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
        uint32_t timeout = 10;

        // Read from second NFC reader
        success = pn532_readPassiveTargetID(&nfc2, PN532_MIFARE_ISO14443A, uid, &uidLength, timeout);
        if (success)
        {
            ESP_LOGI(TAG, "Reader 2 - Found ISO14443A card");
            ESP_LOGI(TAG, "UID Length: %d bytes", uidLength);
            ESP_LOGI(TAG, "UID Value:");
            esp_log_buffer_hexdump_internal(TAG, uid, uidLength, ESP_LOG_INFO);
        }
        else{
             ESP_LOGW(TAG, "Reader 2 - NOT FOUND");
        }

        vTaskDelay(1000 / portTICK_RATE_MS); // Delay between reads

}

void nfc3_task(void *pvParameter)
{
        uint8_t success;
        uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
        uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
        uint32_t timeout = 10;


        success = pn532_readPassiveTargetID(&nfc3, PN532_MIFARE_ISO14443A, uid, &uidLength, timeout);
        if (success)
        {
            ESP_LOGI(TAG, "Reader 3 - Found ISO14443A card");
            ESP_LOGI(TAG, "UID Length: %d bytes", uidLength);
            ESP_LOGI(TAG, "UID Value:");
            esp_log_buffer_hexdump_internal(TAG, uid, uidLength, ESP_LOG_INFO);
        }
        else{
             ESP_LOGW(TAG, "Reader 3 - NOT FOUND");
        }

        vTaskDelay(1000 / portTICK_RATE_MS); // Delay between reads
}


void nfc4_task(void *pvParameter)
{

        uint8_t success;
        uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
        uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
        uint32_t timeout = 10;

        // Read from second NFC reader
        success = pn532_readPassiveTargetID(&nfc4, PN532_MIFARE_ISO14443A, uid, &uidLength,timeout);
        if (success)
        {
            ESP_LOGI(TAG, "Reader 4 - Found ISO14443A card");
            ESP_LOGI(TAG, "UID Length: %d bytes", uidLength);
            ESP_LOGI(TAG, "UID Value:");
            esp_log_buffer_hexdump_internal(TAG, uid, uidLength, ESP_LOG_INFO);
        }
        else{
             ESP_LOGW(TAG, "Reader 4 - NOT FOUND");
        }


        vTaskDelay(1000 / portTICK_RATE_MS); // Delay between reads
}



void app_main(void)
{
    PROVI();
    init_espnow();
    hx711_init();

    xTaskCreate( MQTT, " MQTT",  2048 ,NULL,1,NULL);
    xTaskCreate( Loadcell, " LOADSTRIP",  2048 ,NULL,2,NULL);
    xTaskCreate( display, "DISPLAY",  2048 ,NULL,3,NULL);
    xTaskCreate( nfcread, "NFC-READ",  2048 ,NULL,4,NULL);
    xTaskCreate( led_strip, " LED-STRIP",  2048 ,NULL,4,NULL);

}