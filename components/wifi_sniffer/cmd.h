// main/cmd.h
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Start the UART REPL and register Wi-Fi sniffer console commands. */
void command_init(void);

#ifdef __cplusplus
}
#endif