#ifndef SERVER_H_
#define SERVER_H_

#include <ESP8266WebServer.h>

#define TEXT_PLAIN "text/plain"

#ifndef SERVER_SSID
#define SERVER_SSID           "DEFAULT_SSID"
#endif /* SERVER_SSID */

#ifndef SERVER_PASSWD
#define SERVER_PASSWD         "DEFAULT_PASSWD"
#endif /* SERVER_PASSWD */

#ifndef SERVER_PORT
#define SERVER_PORT           80
#endif /* SERVER_PORT */

void serverTask(void);
void serverInit(void);

void wifiInit(void);

extern ESP8266WebServer server;

typedef struct {
    const char * route;
    const enum HTTPMethod method;
    void (*function)(void);
} route_config_t;

#endif /* SERVER_H_ */
