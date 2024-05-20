#ifndef I2C_H
#define I2C_H

#include <Wire.h>

typedef void (*funcptr)(int);


// Type of messages that can be sent
//
enum MessageType {
  NO_MESSAGE,
  INT_MESSAGE,
  STRING_MESSAGE
};

// This class is a wrapper for the Wire library
//
class I2C {
private:
  int address = -1;
  void messageReceived(int bytes);
  inline static char message[32];  // 32 bytes is the limit of the Wire library implementation
  inline static int messageSize;
  inline static int numberReceived;
public:
  void (*onCommand)(String, String);

  I2C(int myAddress);
  ~I2C();
  void Transmit(int sendToAddress, int command);
  void Transmit(int sendToAddress, char *message);
  MessageType Received(void);
  char *GetStringMessage(void);
  int GetIntMessage(void);
  void checkAvailable();
};

// Indicate if a message has been received
// NO_MESSAGE = No Message received!
//
MessageType I2C::Received(void) {
  if (messageSize == 0) {
    return NO_MESSAGE;
  }
  if (messageSize == 1) {
    return INT_MESSAGE;
  }
  return STRING_MESSAGE;
}


// Function to retreive a string message
//
char *I2C::GetStringMessage(void) {
  messageSize = 0;
  return message;
}

// Function to retrieve an INT
//
int I2C::GetIntMessage(void) {
  messageSize = 0;
  return numberReceived;
}

I2C::I2C(int myAddress) {
  address = myAddress;
  Wire.begin(myAddress);
  Wire.setClock(3400000);
  Wire.setWireTimeout();

  Wire.onReceive((funcptr)&messageReceived);
  messageSize = 0;
}

I2C::~I2C() {
}

// Transmit an INT
//
void I2C::Transmit(int sendToAddress, int command) {
  Wire.beginTransmission(sendToAddress);
  Wire.write(command);
  Wire.endTransmission();
}

// Transmit a string
//
void I2C::Transmit(int sendToAddress, char *message) {
  Wire.beginTransmission(sendToAddress);  // transmit to device #9

  for (int i = 0; i < strlen(message); i++) {
    Wire.write(message[i]);
  }

  Wire.endTransmission();  // stop transmitting
}

// Handler called when a command is received
// can lock up the device, we do the minimium
void I2C::messageReceived(int bytes) {
  if (bytes == 1) {
    numberReceived = Wire.read();
    this->messageSize = bytes;
  } else {
    int i = 0;
    while (Wire.available()) {
      message[i] = Wire.read();
      i++;
    }
    message[i] = 0;
    this->messageSize = bytes;
  }
}

void I2C::checkAvailable(){
  switch (this->Received()){
    delay(10);
    case STRING_MESSAGE:
        String msg = this->GetStringMessage();
        onCommand(utils.parseStr(msg, 0, "="), utils.parseStr(msg, 1, "="));
        break;
    case INT_MESSAGE:
    default:
        break;
    }
}


#endif // I2C_H