#ifndef SYSTEM_H
#define SYSTEM_H

#include "camera.h"
#include "modem.h"
#include "task_manager.h"
#include <TinyGsmParser.h>
#include "../../include/config.h"
#include "../../include/utils.h"

// System initialization function
void system_init();

// Power on board
void board_on();

// Power on wifi
void wifi_on();

// Power on bluetooth
void bluetooth_on();

#endif // SYSTEM_H