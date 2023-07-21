#include "./httpcgi.h"

void send_json_response(struct evhttp_request *req, const char *json_str)
{
    if (json_str == NULL)
    {
        return;
    }

    // 设置响应头部的 Content-Type
    evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", "application/json");

    // 创建响应缓冲区
    struct evbuffer *output_buffer = evbuffer_new();
    if (output_buffer == NULL)
    {
        // 创建缓冲区失败，处理错误并返回
        return;
    }

    // 将 JSON 字符串写入缓冲区
    evbuffer_add(output_buffer, json_str, strlen(json_str));

    // 发送响应
    evhttp_send_reply(req, HTTP_OK, "OK", output_buffer);

    // 释放资源
    evbuffer_free(output_buffer);
}

int validateAuthorization(const char *authorization)
{
    if (authorization == NULL)
    {
        return -1; // 表示验证失败，因为authorization为空
    }

    return 401; // 表示验证失败，authorization不包含在login.strtoken中
}

int isNumericString(const char *str)
{
    size_t i = 0;
    while (str[i] != '\0')
    {
        if (!isdigit(str[i]))
        {
            return 0; // 非数字字符，返回0
        }
        i++;
    }
    return 1; // 字符串中所有字符都是数字字符，返回1
}

int extractTime(const char *timeStr, int *hour, int *minute)
{
    if (sscanf(timeStr, "%d:%d", hour, minute) != 2)
    {
        // 解析失败，格式不正确
        return 2;
    }

    if (*hour < 0 || *hour > 23 || *minute < 0 || *minute > 59)
    {
        // 时间范围不合法
        return 2;
    }

    return 0;
}

int setSystemDateTimeFromUTC(int utcSeconds)
{
    time_t timestamp = utcSeconds;
    struct tm *timeinfo;
    char dateTimeStr[128];

    timeinfo = localtime(&timestamp);
    strftime(dateTimeStr, sizeof(dateTimeStr), "%Y-%m-%d %H:%M:%S", timeinfo);

    char cmd[128];
    snprintf(cmd, sizeof(cmd), "date -u -s '%s'", dateTimeStr);

    if (system(cmd) != 0)
    {
        return 2; // 设置系统日期和时间失败
    }

    return 0; // 设置系统日期和时间成功
}

int setTimeZone(int tz)
{
    char tzValue[16];
    sprintf(tzValue, "UTC%+d", tz);

    setenv("TZ", tzValue, 1);
    tzset();

    return 0; // 设置时区成功
}
time_t getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}

int getTimeZoneOffset()
{
    time_t currentTime = getCurrentTime();
    struct tm *localTime = localtime(&currentTime);
    struct tm *utcTime = gmtime(&currentTime);
    int timeZoneOffset = (localTime->tm_hour - utcTime->tm_hour) * 3600 + (localTime->tm_min - utcTime->tm_min) * 60;
    return timeZoneOffset;
}

void send_json_code_response(struct evhttp_request *req, int code)
{
    // 创建 JSON 对象
    json_object *root = json_object_new_object();

    // 添加错误码字段
    json_object *code_obj = json_object_new_int(code);
    json_object_object_add(root, "code", code_obj);

    // 将 JSON 对象转换为字符串
    const char *json_str = json_object_to_json_string_ext(root, JSON_C_TO_STRING_PLAIN);

    // 创建响应缓冲区
    struct evbuffer *evbuf = evbuffer_new();
    if (evbuf == NULL)
    {
        // 创建响应缓冲区失败，发送内部服务器错误响应
        evhttp_send_error(req, HTTP_INTERNAL, "Internal Server Error");
        json_object_put(root); // 释放 JSON 对象内存
        return;
    }

    // 将 JSON 字符串写入缓冲区
    evbuffer_add(evbuf, json_str, strlen(json_str));

    // 设置响应头部的 Content-Type
    evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", "application/json");

    // 创建响应缓冲区
    struct evbuffer *output_buffer = evbuffer_new();
    if (output_buffer == NULL)
    {
        // 创建缓冲区失败，处理错误并返回
        return;
    }

    // 将 JSON 字符串写入缓冲区
    evbuffer_add(output_buffer, json_str, strlen(json_str));

    // 发送响应
    evhttp_send_reply(req, HTTP_OK, "OK", output_buffer);

    // 释放资源
    evbuffer_free(output_buffer);
}

static void handle_post_request(struct evhttp_request *req)
{

    const char *content_token = evhttp_find_header(evhttp_request_get_input_headers(req), "Authorization");
    const char *uri = evhttp_request_get_uri(req);

    int code = 1;

    if (!strcmp(CGI_WEB_LOGIN, uri))
    {
    }

    if (validateAuthorization(content_token) != 0)
    {
        return;
    }
    else if (strstr(uri, CGI_ROOT_INFO) != NULL)
    {
    }
    else if (strstr(uri, CGI_NETWORK_INFO) != NULL)
    {
    }
    else if (strstr(uri, CGI_CHARGER_INFO) != NULL)
    {
    }
    else if (strstr(uri, CGI_DEVICEINOF) != NULL)
    {
    }

    send_json_code_response(req, code);
}

static void handle_get_request(struct evhttp_request *req)
{

    const char *content_token = evhttp_find_header(evhttp_request_get_input_headers(req), "Authorization");
    const char *uri = evhttp_request_get_uri(req);
    if (validateAuthorization(content_token) == 0 || validateAuthorization(uri) == 0)
    {
        if (strstr(uri, CGI_NETWORK_INFO) != NULL)
        {
            GETNetwork(req);
            return;
        }
        else if (strstr(uri, CGI_CHARGER_INFO) != NULL)
        {
            GETChargerinfo(req);
            return;
        }
    }

    else
    {
        send_json_code_response(req, 401);
        return;
    }
}

static const char *get_content_type(const char *file_path)
{
    const char *extension = strrchr(file_path, '.');
    if (extension != NULL)
    {
        if (strcmp(extension, ".html") == 0)
            return "text/html";
        else if (strcmp(extension, ".css") == 0)
            return "text/css";
        else if (strcmp(extension, ".js") == 0)
            return "text/javascript";
        else if (strcmp(extension, ".jpg") == 0 || strcmp(extension, ".jpeg") == 0)
            return "image/jpeg";
        else if (strcmp(extension, ".png") == 0)
            return "image/png";
        else if (strcmp(extension, ".gif") == 0)
            return "image/gif";
        else if (strcmp(extension, ".bmp") == 0)
            return "image/bmp";
        else if (strcmp(extension, ".svg") == 0)
            return "image/svg+xml";
    }
    return NULL;
}

static void serve_static_file(struct evhttp_request *req, const char *file_path)
{
    FILE *file = fopen(file_path, "rb");
    if (file == NULL)
    {
        evhttp_send_error(req, HTTP_NOTFOUND, "File Not Found");
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    struct evbuffer *evbuf = evbuffer_new();
    if (evbuf == NULL)
    {
        fclose(file);
        evhttp_send_error(req, HTTP_INTERNAL, "Internal Server Error");
        return;
    }

    char *file_content = malloc(file_size);
    if (file_content == NULL)
    {
        fclose(file);
        evbuffer_free(evbuf);
        evhttp_send_error(req, HTTP_INTERNAL, "Internal Server Error");
        return;
    }

    size_t read_size = fread(file_content, 1, file_size, file);
    if (read_size != (size_t)file_size)
    {
        fclose(file);
        free(file_content);
        evbuffer_free(evbuf);
        evhttp_send_error(req, HTTP_INTERNAL, "Internal Server Error");
        return;
    }

    fclose(file);

    struct evkeyvalq *headers = evhttp_request_get_output_headers(req);
    const char *content_type = get_content_type(file_path);
    if (content_type != NULL)
    {
        evhttp_add_header(headers, "Content-Type", content_type);
    }
    else
    {
        evhttp_add_header(headers, "Content-Type", "application/octet-stream");
    }

    evbuffer_add(evbuf, file_content, file_size);
    free(file_content);

    evhttp_send_reply(req, HTTP_OK, "OK", evbuf);
    evbuffer_free(evbuf);
}

static void handle_static_file_request(struct evhttp_request *req)
{
    const char *uri = evhttp_request_get_uri(req);

    // 拼接文件路径
    const char *base_dir = BASE_DIR;
    char file_path[1024];
    snprintf(file_path, sizeof(file_path), "%s%s", base_dir, uri);

    serve_static_file(req, file_path);
}

static void handle_root_request(struct evhttp_request *req)
{
    const char *file_path = "/app/core/web/index.html";
    serve_static_file(req, file_path);
}

static void request_handler(struct evhttp_request *req, void *arg __attribute__((unused)))
{
    enum evhttp_cmd_type method = evhttp_request_get_command(req);
    const char *uri = evhttp_request_get_uri(req);

    if (strcmp(uri, "/") == 0)
    {
        handle_root_request(req);
        return;
    }

    if (strstr(uri, "/static/") == uri)
    {
        handle_static_file_request(req);
        return;
    }

    if (method == EVHTTP_REQ_POST)
    {
        handle_post_request(req);
        return;
    }

    if (method == EVHTTP_REQ_GET)
    {
        handle_get_request(req);
        return;
    }

    send_json_code_response(req, 3);

    return;
}

static void reconnect_server(evutil_socket_t fd __attribute__((unused)), short event __attribute__((unused)), void *arg)
{
    struct http_server_data *server_data = (struct http_server_data *)arg;

    // 关闭之前的连接
    if (server_data->conn != NULL)
    {
        evhttp_connection_free(server_data->conn);
        server_data->conn = NULL;
    }

    // 重新连接服务器
    const char *ip = NULL; // 使用系统默认的 IP 地址
    int port = PORT;       // 使用宏定义的端口号
    int reconnect_attempts = 0;

    while (reconnect_attempts < MAX_RECONNECT_ATTEMPTS)
    {
        server_data->conn = evhttp_connection_base_new(server_data->base, NULL, ip, port);
        if (server_data->conn != NULL)
        {
            server_data->is_connected = 1;
            printf("Server reconnected on %s:%d\n", ip, port);
            break; // 连接成功，跳出循环
        }
        else
        {
            fprintf(stderr, "Failed to reconnect server on %s:%d (Attempt %d)\n", ip, port, reconnect_attempts + 1);
            server_data->is_connected = 0;
            reconnect_attempts++;

            if (reconnect_attempts < MAX_RECONNECT_ATTEMPTS)
            {
                // 重连失败，等待一定时间后再尝试
                sleep(RECONNECT_INTERVAL_SECONDS);
            }
        }
    }

    if (!server_data->is_connected)
    {
        // 连接失败，等待一分钟再尝试
        printf("Failed to reconnect server after %d attempts. Waiting 1 minute before retrying.\n", MAX_RECONNECT_ATTEMPTS);
        sleep(60); // 等待 1 分钟，然后再次尝试重连
    }
}

static void feed_dog(evutil_socket_t fd __attribute__((unused)), short event __attribute__((unused)), void *arg)
{
    struct http_server_data *server_data = (struct http_server_data *)arg;

    // 喂狗操作

    // 启动下一次喂狗的定时器
    struct timeval feed_dog_interval = {FEED_DOG_INTERVAL, 0};
    evtimer_add(server_data->feed_dog_timer, &feed_dog_interval);
}

static void timer_callback(evutil_socket_t fd __attribute__((unused)), short event __attribute__((unused)), void *arg)
{
     struct http_server_data *server_data = (struct http_server_data *)arg;
    // 执行定时器触发的操作
    struct timeval timer_interval = {MODIFY_TOKEN_INTERVAL * 60 * 60, 0}; // 6个小时的时间间隔
    evtimer_add(server_data->modify_token_timer, &timer_interval);
}
void *pv_http_server_thread(void *arg __attribute__((unused)))
{
    DEBUG_MSG("创建http_server线程");
    struct http_server_data server_data;
    server_data.base = event_base_new();
    server_data.http_server = evhttp_new(server_data.base);
    server_data.conn = NULL;
    server_data.is_connected = 0;

    const char *ip = NULL; // 使用系统默认的 IP 地址
    int port = PORT;       // 使用宏定义的端口号

    if (evhttp_bind_socket(server_data.http_server, ip, port) != 0)
    {
        fprintf(stderr, "Failed to bind server on %s:%d\n", ip, port);
        return NULL;
    }

    // 设置通用回调函数
    evhttp_set_gencb(server_data.http_server, request_handler, NULL);
    printf("Server started on %s:%d\n", ip, port);

    // 注册连接断开事件
    server_data.reconnect_timer = event_new(server_data.base, -1, EV_TIMEOUT, reconnect_server, &server_data);
    event_add(server_data.reconnect_timer, NULL);

    // 注册喂狗定时器
    server_data.feed_dog_timer = event_new(server_data.base, -1, EV_TIMEOUT, feed_dog, &server_data);
    struct timeval feed_dog_interval = {FEED_DOG_INTERVAL, 0};
    evtimer_add(server_data.feed_dog_timer, &feed_dog_interval);
    // 添加每6个小时执行一次token的定时器
    server_data.modify_token_timer = event_new(server_data.base, -1, EV_TIMEOUT, timer_callback, &server_data);
    struct timeval timer_interval = {MODIFY_TOKEN_INTERVAL * 60 * 60, 0}; // 6个小时的时间间隔
    event_add(server_data.modify_token_timer, &timer_interval);

    // 进入事件循环
    event_base_dispatch(server_data.base);

    // 释放资源
    if (server_data.conn != NULL)
    {
        evhttp_connection_free(server_data.conn);
    }
    if (server_data.http_server != NULL)
    {
        evhttp_free(server_data.http_server);
    }
    event_free(server_data.reconnect_timer);
    event_free(server_data.feed_dog_timer);
    event_free(server_data.modify_token_timer);
    event_base_free(server_data.base);

    return NULL;
}