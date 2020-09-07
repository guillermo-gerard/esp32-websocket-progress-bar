// #ifndef ProgressStatusBar
// #define ProgressStatusBar

// #include <WebSocketsServer.h>

// class ProgressStatusBar
// {
// private:
//     uint8_t clientNumber;
//     uint8_t currentValue;
//     unsigned long lastAnimationStep;
//     WebSocketsServer* ws;

// public:
//     ProgressStatusBar(WebSocketsServer* ws, uint8_t clientNumber)
//     {
//         this.clientNumber = clientNumber;
//         lastAnimationStep = millis();
//         this->ws = &ws;
//     }

//     void Refresh()
//     {
//         if (lastAnimStep - millis() >= stepInterval)
//         {
//             lastAnimStep = millis();
//             ws.sendTXT(client_num, msg_buf);
//         }
//     }
// }

// #endif