#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "esp_console.h"
#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"

// Project headers
#include "cmd.h"
#include "wifi.h"
#include "sniffer.h"


static const char *TAG = "ESP_CMD";

static esp_console_repl_t *s_repl = NULL;
static bool s_initialized = false;

static void print_usage(void);
static int  wifi_command(int argc, char **argv);

void command_init(void)
{
    if (s_initialized) {
        return;
    }
    s_initialized = true;

    ESP_ERROR_CHECK(esp_console_register_help_command());

    const esp_console_cmd_t wifi_cmd = {
        .command = "wifi",
        .help    = "Wi-Fi control: mode/ap/sta/status",
        .hint    = NULL,
        .func    = &wifi_command,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&wifi_cmd));

    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "wifi> ";
    repl_config.max_cmdline_length = 256;

    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    uart_config.channel   = CONFIG_ESP_CONSOLE_UART_NUM;
    uart_config.baud_rate = CONFIG_ESP_CONSOLE_UART_BAUDRATE;

    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &s_repl));
    ESP_ERROR_CHECK(esp_console_start_repl(s_repl));

    ESP_LOGI(TAG, "Console ready. Type 'help' to list commands.");
}

static void print_usage(void)
{
    printf("Usage:\n");
    printf("  wifi mode <ap|sta|ap+sta|null>\n");
    printf("  wifi sta ssid \"<ssid>\"\n");
    printf("  wifi sta pw \"<password>\"\n");
    printf("  wifi ap ssid \"<ssid>\"\n");
    printf("  wifi ap pw \"<password>\"  (>=8 chars) or empty to open\n");
    printf("  wifi ch <1-13>\n");
    printf("  wifi ring reset\n");   
    printf("  wifi status\n");
 
}

static int wifi_command(int argc, char **argv)
{
    if (argc < 2) {
        print_usage();
        return 0;
    }
    
    // wifi mode <mode>
    if (strcasecmp(argv[1], "mode") == 0) {
        if (argc < 3) {
            print_usage();
            return 0;
        }
        esp_err_t err = wifi_mgr_set_mode_str(argv[2]);
        if (err != ESP_OK) {
            printf("Failed to set mode '%s': %s\n",
                   argv[2], esp_err_to_name(err));
        }
        return 0;
    }

    // wifi sta ssid <ssid> / pw <password>
    if (strcasecmp(argv[1], "sta") == 0) {
        if (argc < 4) {
            print_usage();
            return 0;
        }
        
        // wifi sta ssid <ssid> / pw <password>
        esp_err_t err = ESP_OK;
        if (strcasecmp(argv[2], "ssid") == 0) {
            err = wifi_mgr_set_sta_ssid(argv[3]);
        } else if (strcasecmp(argv[2], "pw") == 0 ||
                   strcasecmp(argv[2], "password") == 0) {
            err = wifi_mgr_set_sta_password(argv[3]);
        } else {
            printf("Unknown STA field '%s'\n", argv[2]);
            print_usage();
            return 0;
        }
        if (err != ESP_OK) {
            printf("Failed to update STA config: %s\n", esp_err_to_name(err));
        }
        return 0;
    }

    // wifi ap ssid/pw
    if (strcasecmp(argv[1], "ap") == 0) {
        if (argc < 4) {
            print_usage();
            return 0;
        }
        // wifi ap ssid <ssid> / pw <password>
        esp_err_t err = ESP_OK;
        if (strcasecmp(argv[2], "ssid") == 0) {
            err = wifi_mgr_set_ap_ssid(argv[3]);
        } else if (strcasecmp(argv[2], "pw") == 0 ||
                   strcasecmp(argv[2], "password") == 0) {
            err = wifi_mgr_set_ap_password(argv[3]);
        } else {
            printf("Unknown AP field '%s'\n", argv[2]);
            print_usage();
            return 0;
        }
        if (err != ESP_OK) {
            printf("Failed to update AP config: %s\n", esp_err_to_name(err));
        }
        return 0;
    }

    // wifi status
    if (strcasecmp(argv[1], "status") == 0) {
        wifi_mgr_print_status();
        sniffer_print_stats();
        return 0;
    }

    // wifi ch 1~13
    if (strcasecmp(argv[1], "ch") == 0 || strcasecmp(argv[1], "channel") == 0) {
        if (argc < 3) {
            print_usage();
            return 0;
        }
        int ch = atoi(argv[2]);
        esp_err_t err = wifi_mgr_set_channel((uint8_t)ch);
        if (err != ESP_OK) {
            printf("Failed to set channel: %s\n", esp_err_to_name(err));
        }
        return 0;
    }    

    // wifi ring reset 
    if (strcasecmp(argv[1], "ring") == 0) {
        
        if (strcasecmp(argv[2], "reset") == 0) {
            sniffer_ring_reset();
            sniffer_print_stats();
            return 0;
        }
    

    }   

    printf("Unknown wifi command '%s'\n", argv[1]);
    print_usage();
    return 0;
}
