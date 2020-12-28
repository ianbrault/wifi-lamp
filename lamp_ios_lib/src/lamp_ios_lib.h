/*
** src/lamp_ios_lib.h
*/

#include <stdbool.h>
#include <stdint.h>

typedef struct web_socket websocket_t;

const uint8_t USER_ARNI = 0x30;
const uint8_t USER_IAN  = 0x31;

extern websocket_t* connect(const char*);
extern bool declare_client_connection(websocket_t*, uint8_t);
extern bool power_on(websocket_t*);
extern bool power_off(websocket_t*);
extern void free_websocket(websocket_t*);
