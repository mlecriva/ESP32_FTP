#ifndef __FTPCLIENT_H__
#define __FTPCLIENT_H__

#include <WiFiClient.h>
#include "FTPCommands.h"

typedef enum
{
    TYPE_A = 0X00,
    TYPE_I = 0X01
} TransfertType_t;

class FTPClient
{
public:
    FTPClient(char *serverAdress, char *username, char *password);

    void connect();
    void stop();

    void createFile(char *filename, TransfertType_t type = TYPE_I);
    void appendFile(char *filename, TransfertType_t type = TYPE_I);
    void RenameFile(char *from, char *to);

    void write(char *str);
    void closeFile();

private:
    void writeCommand(FTPCommands_t command, char *parameter = NULL);
    void getServerAnswer(char *result = NULL, int offsetStart = 0);
    void initFile(TransfertType_t type);

    /* data */
    char *_username;
    char *_password;
    char *_serverAdress;

    WiFiClient _client;
    WiFiClient _dataClient;

    char _outBuffer[128];
    char _outCounter;
};

#endif /* __FTPCLIENT_H__ */