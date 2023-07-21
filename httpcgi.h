#ifndef _HTTP_CGI_H_
#define _HTTP_CGI_H_

#ifdef __cplusplus
    extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <json-c/json.h>
#include <pthread.h>

#include "event2/event.h"
#include "event2/http.h"
#include "event2/buffer.h"
#include "event2/visibility.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>

#define PORT 80
#define  MAX_ENTRIES     100   
#define MAX_BUFFER_SIZE 2048
#define BASE_DIR          "/app/core/web"//网页资源路径

#define CGI_CHARGER_INFO  "/api/info/charger"
#define CGI_CHGPILE_INFO  "/api/info/chgpile"
#define CGI_DEVICEINOF    "/api/info/device"
#define CGI_NETWORK_INFO  "/api/info/network"
#define CGI_ROOT_INFO     "/api/info/root"
#define CGI_OTHER_INFO     "/api/info/other"
#define CGI_WEB_LOGIN      "/api/info/login"
#define CGI_WEB_LOGOUT      "/api/info/logout"
#define CGI_OCPP_INFO      "/api/info/ocpp"
#define CGI_MODULE_INFO    "/api/info/module"
#define CGI_RTL_ALARM     "/api/rtl/alarm"
#define CGI_ALARM_LOG     "/api/log/alarmlog"
#define CGI_CHARGE_LOG     "/api/log/chargelog"
#define CGI_UPDATE_FILE     "/api/file/update"

// 定义最大重连次数和每次重连的间隔时间
#define MAX_RECONNECT_ATTEMPTS 5
#define RECONNECT_INTERVAL_SECONDS 10 // 间隔时间为 10 秒
#define FEED_DOG_INTERVAL 1 // 喂狗间隔时间，单位：秒
#define MODIFY_TOKEN_INTERVAL 6 // 喂狗间隔时间，单位：时

struct http_server_data {
    struct event_base *base;
    struct evhttp *http_server;
    struct evhttp_connection *conn;
    int is_connected;
    struct event *reconnect_timer;
    struct event *feed_dog_timer;
    struct event *modify_token_timer;
};

void send_json_response(struct evhttp_request* req, const char* json_str);

void send_json_code_response(struct evhttp_request *req, int code);


int validateAuthorization(const char *authorization);

int isNumericString(const char* str);

int extractTime(const char *timeStr, int *hour, int *minute);
int setSystemDateTimeFromUTC(int utcSeconds);
time_t getCurrentTime();
int getTimeZoneOffset();
int setTimeZone(int tz);

void GETChargerinfo(struct evhttp_request* req);
void GETNetwork(struct evhttp_request* req);
void GETDeviceinfo(struct evhttp_request* req);
void GETChgpileinfo(struct evhttp_request* req);
void GETModule(struct evhttp_request* req);
void GETOCPP(struct evhttp_request *req);
void GETLogout(struct evhttp_request* req);
void GETchargelog(struct evhttp_request* req);
void GETalarm(struct evhttp_request *req);
void GETother(struct evhttp_request *req);
void GETalarmlog(struct evhttp_request *req);

int POSTNetwork(struct evhttp_request* req);
int POSTLogin(struct evhttp_request* req);
int POSTChargerinfo(struct evhttp_request *req);
int POSTDeviceinfo(struct evhttp_request *req);
int POSTChgpileinfo(struct evhttp_request* req);
int POSTModule(struct evhttp_request* req);
int POSTOCPP(struct evhttp_request *req);
int POSTupdate(struct evhttp_request* req);
int POSTother(struct evhttp_request *req);
int POSTRoot(struct evhttp_request *req);

void* pv_http_server_thread(void* arg);


#ifdef __cplusplus
        }
#endif

#endif  


