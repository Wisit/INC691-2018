
#include "os.h"
#include "http.h"

int main(void) {
    OS_Init();
    HTTP_ServerInit();
    OS_Start();
}


