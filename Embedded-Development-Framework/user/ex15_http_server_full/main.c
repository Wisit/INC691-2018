/***************************************************
 *               Dr.Santi Nuratch                  *
 *    Embedded Computing and Control Laboratory    *
 *               28 Fubruary, 2018                 *
 * *************************************************
 *      ESP8266 WiFi module, HTTP Server/AT (FULL) *
 *   This example shows how to create HTTP Server  *
 *   The program is implemented using even-driven  *
 *  and callback technique. Each function operates *
 *              as Asynchronous Operation          *
 ***************************************************
 */

#include "os.h" //!! ECC-OS and all utillities functions


//!!
//!! WiFI Netwok connection parameters
//!!
const char *SSID = "SSID";  //!! SSID
const char *PASS = "PASS";  //!! Password


//!!
//!! Desered AT command response tokens
//!!
#define ESP_RETURN_ITEMS        8
#define ESP_RETURN_OK           0
#define ESP_RETURN_ERROR        1
#define ESP_RETURN_FAILED       2
#define ESP_RETURN_READY        3
#define ESP_RETURN_DISCONNECTED 4
#define ESP_RETURN_CONNECTED    5
#define ESP_RETURN_GOT_IP       6
#define ESP_RETURN_BUSY         7
#define ESP_RETURN_UNDEFINED    -1
const char *EspReturnMsg[ESP_RETURN_ITEMS] = {
    "OK\r\n",               //!! [0]
    "ERROR\r\n",            //!! [1]
    "FAILED\r\n",           //!! [2]
    "ready\r\n",            //!! [3] Module boot up
    "WIFI DISCONNECT\r\n",  //!! [4]
    "WIFI CONNECTED\r\n",   //!! [5]
    "WIFI GOT IP\r\n",      //!! [6]
    "busy p...\r\n"         //!! [7] The command will be ignored
};


//!!
//!! Module
//!!
typedef struct {
    uint8_t  state;
    uint16_t idleCount;
    timer_t *timer;
}module_state_t;
module_state_t module;
#define MODULE_STATE_ITEMS              3
#define MODULE_STATE_DISCONNECTED       0   //!! [0] No WiFi module is detedted
#define MODULE_STATE_WAITING            1   //!! [1] WiFi module is restarted, waiting for response
#define MODULE_STATE_CONNECTED          2   //!! [2] WiFi module is detected
const char *ModuleStateMsg[] = {
    "[0] MODULE_STATE_DISCONNECTED\r\n",    //!! [0]
    "[1] MODULE_STATE_WAITING\r\n",         //!! [1]
    "[2] MODULE_STATE_CONNECTED\r\n",       //!! [2]
};

//!! Print the Module state
#define module_print_state() {                              \
    Uart1_AsyncWriteString("Module.state:  ");              \
    Uart1_AsyncWriteString(ModuleStateMsg[module.state]);   \
}

//!! Initialize the Module parameters
void Module_Init(void) {
    module.idleCount = 0;
    module.state = MODULE_STATE_DISCONNECTED;
}

//!! State machines of the WiFi module
int Module_ProcessState(int atResponseCode) {
    //!! Reset the module.idleCount
    module.idleCount = 0;

    //!! No WiFi module is detected
    if(module.state == MODULE_STATE_DISCONNECTED) {
        //!! This state is checked and controled by ProcessSystemState()
        //!! The ProcessSystemState() resets the module and change state to MODULE_STATE_WAITING, 
        //!! So we need to wait for "ready\r\n", the ESP_RETURN_READY
    }

    //!! Waiting a response from the WiFi module
    else if(module.state == MODULE_STATE_WAITING) {
        if(atResponseCode == ESP_RETURN_READY) {
            //!! The WiFi module is detected, decrease time interval t0 100 mS
            module.state = MODULE_STATE_CONNECTED;
            OS_TimerSetTicks(module.timer, 100);    
        }
    }

    //!! The WiFi module is detected
    else if(module.state == MODULE_STATE_CONNECTED) {
        //!! Donothing
    }

    return module.state;
}


//!!
//!! Network
//!!
#define NET_STATE_ITEMS         3
#define NET_STATE_DISCONNECTED  0           //!! [0] Disconnected from network
#define NET_STATE_CONNECTED     1           //!! [1] Connected to network
#define NET_STATE_GOT_IP        2           //!! [2] Received IP address
const char *NetStateMsg [] = {
    "[0] NETWORK_STATE_DISCONNECTED\r\n",   //!! [0]
    "[1] NEWORKT_STATE_CONNECTED\r\n",      //!! [1]
    "[2] NETWORK_STATE_GOT_IP\r\n"          //!! [2]
};

typedef struct {
    uint8_t state;
    const char * ssid;
    const char * pass;
}network_t;
network_t network;

//!! Initialze Network parameters
void Network_Init(const char *ssid, const char *pass) {
    network.ssid  = ssid;
    network.pass  = pass;
    network.state = NET_STATE_DISCONNECTED;
}

//!! Called by Network_ProcessState() to print network information
#define network_print_state() {                         \
    Uart1_AsyncWriteString("Network.state: ");          \
    Uart1_AsyncWriteString(NetStateMsg[network.state]); \
}



//!! Called by WiFiLineReceived()
int Network_ProcessState(int atResponseCode) {

    //!! Disconnected from network
    if(network.state == NET_STATE_DISCONNECTED) {
        if(atResponseCode == ESP_RETURN_CONNECTED) {
            network.state = NET_STATE_CONNECTED;
            
        }
    }

    //!! Connected to network
    else if(network.state == NET_STATE_CONNECTED) {
        if(atResponseCode == ESP_RETURN_GOT_IP) {
            network.state = NET_STATE_GOT_IP;
        }
    }

    //!! Received ip address
    else if(network.state == NET_STATE_GOT_IP) {
        if(atResponseCode == ESP_RETURN_DISCONNECTED) {
            network.state = NET_STATE_DISCONNECTED;
        }
    }
    return network.state;
}


//!!
//!! Server
//!!
#define SERVER_STATE_STOPPED  0  //!! [0] Server is stopped
#define SERVER_STATE_PREPARE  1  //!! [1] Prepare to start
#define SERVER_STATE_STARTING 2  //!! [2] Try to start
#define SERVER_STATE_RUNNING  3  //!! [3] Server is running...
typedef struct {
    uint8_t state;
    bool    requested;
    timer_t *timer;
}server_t;
server_t server;
const char * ServStateMsg[] = {
    "[0] SERVER_STATE_STOPPED\r\n",     //!! [0]
    "[1] SERVER_STATE_PREPARE\r\n",     //!! [1]
    "[2] SERVER_STATE_STARTING\r\n",    //!! [2]
    "[3] SERVER_STATE_RUNNING\r\n"      //!! [3]
};

//!! Print server stste
#define server_print_state() {                          \
    Uart1_AsyncWriteString("Server.state:  ");          \
    Uart1_AsyncWriteString(ServStateMsg[server.state]); \
}

//!! Initial server
void Server_Init(void) {
    server.state = SERVER_STATE_STOPPED;
    server.requested = false;
}

//!! Prototype functions
void MakeResponse(void *evt);
void StartServer(void *evt);

int Server_ProcessState(int atResponseCode) {
    if(server.state == SERVER_STATE_STOPPED) {
        server.state = SERVER_STATE_PREPARE;
        server.timer = OS_TimerCreate("server", 500, 1, StartServer);
    }
    else if(server.state == SERVER_STATE_PREPARE && atResponseCode == ESP_RETURN_OK) {
        server.state = SERVER_STATE_STARTING;
    }
    else if(server.state == SERVER_STATE_STARTING && atResponseCode == ESP_RETURN_OK) {
        server.state = SERVER_STATE_RUNNING;
    }
    return server.state;
}

//!! Called by WiFiLineReceived()
int ProcessResponseMsg(const char *msg) {
    int i;
    for(i=0; i<ESP_RETURN_ITEMS; i++) {
        if(!strcmp(msg, EspReturnMsg[i])) {
            Uart1_AsyncWriteString("    >>AT Response: ");
            Uart1_AsyncWriteString(EspReturnMsg[i]);
            return i;
        }
    }
    return ESP_RETURN_UNDEFINED;
}


//!!
//!! Request
//!!
#define REQ_STATE_WAITING       0   //!![0] Waiting for a package
#define REQ_STATE_REQUESTED     1   //!![1] Received a requset package
#define REQ_STATE_SENDING       2   //!![2] Sending a response meassage
#define REQ_STATE_COMPLETED     3   //!![3] Response message is sent
typedef struct {
    uint8_t state;
    timer_t *timer;
}request_t;
request_t request;


//!! Callback function of LineReceived of the WiFi module
void WiFiLineReceived(void *evt) {

    //!! Get the received line
    data_t line_data = ((uart_event_t *)evt)->data;
    const char *esp_data = (const char *)line_data.buffer;

    //!! Print a received line
    Uart1_AsyncWriteString("  >>WiFi Received: ");
    Uart1_AsyncWriteString(esp_data);


    //!! Check the incoming package
    if(server.state == SERVER_STATE_RUNNING) {
        if(str_index_of_first_token(esp_data, "+IPD,") == 0) {
            Uart1_AsyncWriteString("\r\n");
            request.state = REQ_STATE_REQUESTED;
            MakeResponse((void *)esp_data);
        }
    }

    //!!
    //!! Process Response Message
    //!!
    int atResponseCode = ProcessResponseMsg(esp_data);
    (void)atResponseCode;

    //!!
    //!! Process System State
    //!!
    int moduleState = Module_ProcessState(atResponseCode);
    (void)moduleState;
    //!! The WiFi module is not detected, return
    if(module.state != MODULE_STATE_CONNECTED) {
        return;
    }

    //!!
    //!! Process Network State
    //!!
    int netState = Network_ProcessState(atResponseCode);
    (void)netState;

    //!! No ip address is received yet, wait...
    if(network.state != NET_STATE_GOT_IP) {
        return;
    }

    //!!
    //!! Process Server State
    //!!
    int servState = Server_ProcessState(atResponseCode);
    (void)servState;

    //!! Server is not running
    if(server.state != SERVER_STATE_RUNNING) {
        return;
    }
}

//!! Response to all of "led" commands
bool ExecuteLED(char *getData, char *message) {

    //!! The following commands are accepted:
    //!! ledxon     : Turn LEDx ON
    //!! ledxoff    : Turn LEDx OFF   
    //!! ledx       : Get status of LEDx
    
    //!! Check command length
    int len = strlen(getData);
    if(len <= 3) {
        return false;
    }

    //!! leds : Get status of all LEDs
    if(getData[3] == 's') {
        sprintf(message, "<html><body><h1><div style='color:orange;'>\
        LED[0]: %s<br/>\
        LED[1]: %s<br/>\
        LED[2]: %s<br/>\
        LED[3]: %s<br/>\
        </div></h1><body></html>", LED0_Get()?"ON":"OFF", LED1_Get()?"ON":"OFF", LED2_Get()?"ON":"OFF", LED3_Get()?"ON":"OFF");
        return true;    
    }

    //!! Get id
    uint8_t id = getData[3] - 0x30;
    if(id<0 || id>3) {
        return false;
    }

    //!! Pointer to ON/OFF
    const char *status;

    // ledx : Get status of LEDx
    if(len == 4) {
        status = LED_Get(id)?"ON":"OFF";    
    }
    //!! ledxon : Turn on LEDx
    else if(!strcmp(getData + 4, "on")) {
        status = "ON";
        LED_Set(id);
    }
    //!! ledxoff : Turn off LEDx
    else if(!strcmp(getData + 4, "off")) {
        status = "OFF";
        LED_Clr(id);
    }
    //!! ledxinv : Toffle LEDx
    else if(!strcmp(getData + 4, "inv")) {
        LED_Inv(id);
        status = LED_Get(id)?"ON":"OFF";
    }
    else {
        return false; 
    }

    sprintf(message, "<html><body><h1><div style='color:orange;'>LED[%d]: %s</div></h1><body></html>", id, status);
    return true;
}

//!! Response to all of "psw" commands
bool ExecutePSW(char *getData, char *message) {

    //!!The following commands are accepted:
    //!! pswx       Get status of PSWx
    //!! psws       Get status of all PSWs
    
    //!! The length of the command must be 4
    int len = strlen(getData);
    if(len != 4) {
        return false;
    }

    //!! leds : Get status of all PSWs
    if(getData[3] == 's') {
        sprintf(message, "<html><body><h1><div style='color:green;'>\
        PSW[0]: %s<br/>\
        PSW[1]: %s<br/>\
        PSW[2]: %s<br/>\
        PSW[3]: %s<br/>\
        </div></h1><body></html>", PSW0_Get()?"ON":"OFF", PSW1_Get()?"ON":"OFF", PSW2_Get()?"ON":"OFF", PSW3_Get()?"ON":"OFF");
        return true;    
    }

    //!! Get id of switch
    uint8_t id = getData[3] - 0x30;
    if(id<0 || id>3) {
        return false;
    }

    //!! pswx : Get status of PSWx
    const char *status = PSW_Get(id)?"ON":"OFF";
    sprintf(message, "<html><body><h1><div style='color:green;'>PSW[%d]: %s</div></h1><body></html>", id, status);
    return true;
}

//!! Response to all of "adc" commands
bool ExecuteADC(char *getData, char *message) {

    //!! The following command are accepted:
    //!! adcx : Get value of an ADCx
    //!! adcs : Get values of all ADCs
    
    //!! The length of the command must be 4
    int len = strlen(getData);
    if(len != 4) {
        return false;
    }

    //!! adcs : Get values of all ADCs
    if(getData[3] == 's') {
        sprintf(message, "<html><body><h1><div style='color:blue;'>\
        ADC[0]: %.4d<br/>\
        ADC[1]: %.4d<br/>\
        ADC[2]: %.4d<br/>\
        ADC[3]: %.4d<br/>\
        </div></h1><body></html>", ADC0_Get(), ADC1_Get(), ADC2_Get(), ADC3_Get());
        return true;    
    }

    //!! Get id of adc channel
    uint8_t id = getData[3] - 0x30;
    if(id<0 || id>3) {
        return false;
    }

    //!! adcx : Get value of an ADCx
    int value = ADC_Get(id);
    sprintf(message, "<html><body><h1><div style='color:blue;'>ADC[%d]: %.4d</div></h1><body></html>", id, value);
    return true;
}

//!! Response to "all" command
bool ExecuteALL(char *getData, char *message) {

    //!! Only one command is accepted, "all"
    if(strcmp(getData, "all")) {
        //!! Not matched, return
        return false;
    }

    //!! Create a response message containing all information
    sprintf(message, "<html><body><h1>\
        <div style='color:green'>\
        PSW[0]: %s<br/>\
        PSW[1]: %s<br/>\
        PSW[2]: %s<br/>\
        PSW[3]: %s<br/></div><hr/>\
        <div style='color:blue'>\
        ADC[0]: %.4d<br/>\
        ADC[1]: %.4d<br/>\
        ADC[2]: %.4d<br/>\
        ADC[3]: %.4d<br/></div><hr/>\
        <div style='color:orange'>\
        LED[0]: %s<br/>\
        LED[1]: %s<br/>\
        LED[2]: %s<br/>\
        LED[3]: %s<br/></div><hr/>\
        </h1><body></html>",\
        PSW0_Get()?"ON":"OFF", PSW1_Get()?"ON":"OFF", PSW2_Get()?"ON":"OFF", PSW3_Get()?"ON":"OFF",\
        ADC0_Get(), ADC1_Get(), ADC2_Get(), ADC3_Get(),\
        LED0_Get()?"ON":"OFF", LED1_Get()?"ON":"OFF", LED2_Get()?"ON":"OFF", LED3_Get()?"ON":"OFF");

    return true;
}

//!! Process Request message and make an appropriate response message
void MakeResponse(void *evt) {
    static uint8_t channel;
    static char getData[32] = {0};
    static char message[512] = {0};
    static char *ptr = message;

    char cmdBuff[32] = {0};   

    if(server.state != SERVER_STATE_RUNNING ) {
        //!! Stop
        OS_TimerDelete(request.timer);  
        request.timer = NULL;    

        return;
    }

    if(request.timer == NULL) {
        //!! Start the timer for next time execution
        request.timer = OS_TimerCreate("resp", 10, 1, MakeResponse);    
    }

    //!! FIRST STATE: Process requested message
    if(request.state == REQ_STATE_REQUESTED) {

        //!! Take received line
        const char *line = (const char *)evt;

        //!! Tergets indexes
        uint8_t idx1 = str_index_of_first_token(line, ",");
        uint8_t idx2 = str_index_of_first_token(line, "GET /");
        uint8_t idx3 = str_index_of_first_token(line, "HTTP/");
        channel = line[idx1+1] - 0x30;

        //!! do checking
        if (idx2 < 0 || idx3 < 0 || str_index_of_first_token(line, "favicon.ico") >= 0) {
            //!! Wrong format or favicon.ico is requested, reject the request!
            request.state = REQ_STATE_WAITING;
            OS_TimerDelete(request.timer);  
            request.timer = NULL;  
            //Beep(1000);
            return;
        }

        //!! point to GET content
        idx2 += 5;
        idx3 -= 1;

        //!! Take the GET content
        memcpy(getData, line + idx2, (idx3 - idx2));
        getData[(idx3 - idx2)] = 0; //!! NULL terminate

        //!! Create response message, will be used in next state
        bool completed = false;
        os_time_t time = OS_TimeGet();
        if(!strcmp(getData, "time")) {
            sprintf(message, "<html><body><h1><div>System Time: %.2d:%.2d:%.2d:%.3d</div></h1><body></html>", time.hh, time.mm, time.ss, time.ms);
        } 

        //!! LED commands
        else if(str_index_of_first_token(getData, "led") == 0) {
            completed = ExecuteLED(getData, message);
        }
        //!! ADC commands
        else if(str_index_of_first_token(getData, "adc") == 0) {
            completed = ExecuteADC(getData, message);
        }
        //!! PSW commands
        else if(str_index_of_first_token(getData, "psw") == 0) {
            completed = ExecutePSW(getData, message);
        }
        //!! An all command
        else if(str_index_of_first_token(getData, "all") == 0) {
            completed = ExecuteALL(getData, message);
        }
        //!! GET command not matched
        else {
            completed = false;
        }

        //!! Generate error message
        if(!completed) {
            sprintf(message, "<html><body><h1><div style='color:red;'>The request \"/%s\" is not supported!</div></h1><body></html>", getData);
        }

        //!! Send CIPSEND request
        //!! After the CIPSEND is applied, request.state must be changed to REQ_STATE_SENDING
        //!! The request.state is checked in the next state, see in next else if protion below
        sprintf(cmdBuff, "AT+CIPSEND=%d,%d\r\n", channel, strlen(ptr));
        Uart2_AsyncWriteString(cmdBuff);

        //!! Change to the next state
        request.state = REQ_STATE_SENDING;

        //!! Go to next state in next 50 mS
        OS_TimerSetTicks(request.timer, 50);
        Beep(20);
    }

    //!! SECOND STATE: Apply the payload, send the message to the client
    else if(request.state == REQ_STATE_SENDING) {
        Uart2_AsyncWriteString(ptr);
        request.state = REQ_STATE_COMPLETED;
        OS_TimerSetTicks(request.timer, 200);
    }
    //!! THIRD STATE: Close the connection, the client/browser will finish after this command is applied
    else if(request.state == REQ_STATE_COMPLETED) {
        sprintf(cmdBuff, "AT+CIPCLOSE=%d\r\n", channel);
        Uart2_AsyncWriteString(cmdBuff);

        //!! Back to the first state, delete the timer and waiting for new package
        request.state = REQ_STATE_WAITING;  
        OS_TimerDelete(request.timer);   
        request.timer = NULL;   
    }
}


//!! State machines used for start server
//!! This function works as callback function of the server.timer
void StartServer(void *evt) {

    switch(server.state){
        //!! Server is stopped, kill the timer
        case SERVER_STATE_STOPPED:{
            OS_TimerDelete(server.timer);   
        }break;

        //!! Server is started, apply CIPMUX command
        case SERVER_STATE_PREPARE:{
            //Uart1_AsyncWriteString(">> Preparing to start server...\r\n");
            Uart2_AsyncWriteString("AT+CIPMUX=1\r\n");
        }break;

        //!! Start the server
        case SERVER_STATE_STARTING:{
            //Uart1_AsyncWriteString(">> Starting server...\r\n");
            Uart2_AsyncWriteString("AT+CIPSERVER=1,80\r\n");
        }break;

        //!! Server is now running
        case SERVER_STATE_RUNNING:{
            Uart2_AsyncWriteString("AT+CIFSR\r\n");
            OS_TimerDelete(server.timer);
            Beep(30);
        }break;
    }
}


//!! prints system time
#define print_system_time() {                                   \
    char buff[32];                                              \
    os_time_t time = OS_TimeGet();                              \
    sprintf(buff, "System Time:   [%d] %.2d:%.2d:%.2d\r\n",     \
                module.idleCount, time.hh, time.mm, time.ss);   \
    Uart1_AsyncWriteString(buff);                               \
}

//!! Main loop 
void MainLoop(void *evt) {
    static uint32_t prev_sec = 0;
    os_time_t time = OS_TimeGet();

    if(module.state == MODULE_STATE_DISCONNECTED) {
        //!! Reset the module and wait for "ready\r\n";
        Uart1_AsyncWriteString("\r\nReseting the WiFi module...\r\n");
        module.state = MODULE_STATE_WAITING;
        Uart2_AsyncWriteString("AT+RST\r\n");       //!! Reset command

        // Next loop will be executed in next 3 seconds
        OS_TimerSetTicks(module.timer, 3000);
    } 

    //!! Print module status
    if( time.ss > prev_sec ) {
        //!! DO NOT print all information if the request.state != REQ_STATE_WAITING
        //!! This can improve for large response request, e.g.; /all
        if(request.state == REQ_STATE_WAITING) {
            print_system_time();
            module_print_state();
            network_print_state();
            server_print_state();
            Uart1_AsyncWriteString("\r\n");  
        }

        //!! Make handshaking if no package is received
        module.idleCount++;
        if(module.idleCount == 10) {
            Uart2_AsyncWriteString("AT+CIFSR\r\n");     //!! Handshake
            //!! After this command is applied, the module will retuen "OK\r\n" and
            //!! the module.idleCount will be reset in the WiFiLineReceived()
        }
        else if(module.idleCount >= 12) {
            //!! No response message sent from the module, reset
            module.idleCount = 0;
            module.state = MODULE_STATE_DISCONNECTED;
            Uart1_AsyncWriteString("\r\nNo incoming message, reseting the WiFi module...\r\n");

            //!! Change module.state to MODULE_STATE_DISCONNECTED
            //!! The module will be reset next turn in this function, see above
            module.state  = MODULE_STATE_DISCONNECTED;

            //!! Change netwotk state to NET_STATE_DISCONNECTED
            network.state = NET_STATE_DISCONNECTED;

            //!! Change server state to stop
            server.state = SERVER_STATE_STOPPED;
        }
    }
    prev_sec = time.ss;
}


//!! For first boot only
void DoFirstBoot(void) {
    char cmd[48];
    if(PSW0_Get()) {
        Uart1_AsyncWriteString("\r\n***Applying First Boot***\r\n");
        Uart2_AsyncWriteString("ATE0\r\n");         __delay_ms(100);
        Uart2_AsyncWriteString("AT+CWMODE=1\r\n");  __delay_ms(100);
        sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASS);
        Uart2_AsyncWriteString(cmd);
        Beep(200);
    }
}

int main(void)
{
    //!! Initialize OS
    OS_Init();

    //!! Initialize Module, Network and Server
    Module_Init();
    Network_Init(SSID, PASS);
    Server_Init();

    //!! Short beep
    Beep_FreqSet(3500);
    Beep(20);

    //!! Change beep frequency
    Beep_FreqSet(2000);

    //!! Create main loop service and give it acallback
    module.timer = OS_TimerCreate("main", 100, 1, MainLoop);

    //!! For the first time running, hold down PSW0 and press RESET button
    //!! This function will apply ATE, CWMODE, and CWJAP commands
    //!! All other initialized properties of the WiFi module shoud be applied in this function
    DoFirstBoot();

    //!! Register LineEeceived callback of the WiFi module 
    OS_Uart2SetLineReceivedCallback(WiFiLineReceived);
   
    //!! Start OS
    OS_Start();
}

