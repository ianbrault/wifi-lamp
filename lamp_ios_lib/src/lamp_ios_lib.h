#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct WebSocket_AutoStream {
  void *inner;
} WebSocket_AutoStream;

struct WebSocket_AutoStream connect_to_lamp(const char *address);

void disconnect_from_lamp(struct WebSocket_AutoStream socket);

bool declare_client_connection(struct WebSocket_AutoStream socket, uint8_t owner);

bool power_on(struct WebSocket_AutoStream socket);

bool power_off(struct WebSocket_AutoStream socket);
