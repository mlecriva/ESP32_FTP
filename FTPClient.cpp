#include "FTPClient.h"
#include <iostream>
#include <string>

using namespace std;

FTPClient::FTPClient(char *serverAdress, char *username, char *password)
{
    _username = username;
    _password = password;
    _serverAdress = serverAdress;
}

void FTPClient::writeCommand(FTPCommands_t command, char *parameter)
{
    if (parameter == NULL)
    {
        cout << "Send" << FTPCommandList[command] << endl;
        _client.println(F(FTPCommandList[command]));
    }
    else
    {
        string cmd = FTPCommandList[command];

        cmd.append(" ");
        cmd.append(parameter);

        cout << "Send" << cmd << endl;

        _client.println(F(cmd.c_str()));
    }
}

void FTPClient::getServerAnswer(char *result, int offsetStart)
{
    byte respCode;
    byte thisByte;

    _outCounter = 0;
    while (!_client.available())
        delay(1);

    while (_client.available())
    {
        thisByte = _client.read();
        if (_outCounter < 127)
        {
            _outBuffer[_outCounter++] = thisByte;
            _outCounter++;
            _outBuffer[_outCounter] = 0;
        }
    }
    if (result != NULL)
    {
        cout << "Result start" << endl;
        for (int i = offsetStart; i < 21; i++)
        {
            result[i] = _outBuffer[i - offsetStart];
        }
        cout << "Result: " << result << "Result end" << endl;
    }
}

void FTPClient::initFile(TransfertType_t type)
{
    int array_pasv[6];
    unsigned int port;

    if (type == TYPE_A)
    {
        writeCommand(TYPE, "A");
    }
    else if (type == TYPE_I)
    {
        writeCommand(TYPE, "I");
    }
    getServerAnswer();

    writeCommand(PASV);
    getServerAnswer();

    char *tStr = strtok(_outBuffer, "(,");

    for (int i = 0; i < 6; i++)
    {
        tStr = strtok(NULL, "(,");
        array_pasv[i] = atoi(tStr);
        if (tStr == NULL)
        {
            cout << F("Bad PASV Answer") << endl;
        }
    }

    port = ((array_pasv[4] << 8) | (array_pasv[5] & 255));
    cout << F("Data port: ") << port << endl;

    if (_dataClient.connect(_serverAdress, port))
    {
        cout << F("Data connection etablished") << endl;
    }
}

void FTPClient::connect()
{
    cout << "Connecting to " << _serverAdress << endl;
    if (_client.connect(_serverAdress, 21))
    {
        cout << F("Command connected") << endl;
    }
    getServerAnswer();

    writeCommand(USER, _username);
    getServerAnswer();

    writeCommand(PASS, _password);
    getServerAnswer();

    writeCommand(SYST);
    getServerAnswer();
}

void FTPClient::stop()
{
    _client.println(F("QUIT"));
    _client.stop();
    cout << F("Connection closed") << endl;
}

void FTPClient::createFile(char *filename, TransfertType_t type)
{
    initFile(type);
    writeCommand(STOR, filename);
    getServerAnswer();
}

void FTPClient::appendFile(char *filename, TransfertType_t type)
{
    initFile(type);
    writeCommand(APPE, filename);
    getServerAnswer();
}

void FTPClient::RenameFile(char *from, char *to)
{
    writeCommand(RNFR, from);
    getServerAnswer();

    writeCommand(RNTO, to);
    getServerAnswer();
}

void FTPClient::write(char *str)
{
    _dataClient.print(str);
}

void FTPClient::closeFile()
{
    cout << F("Close File") << endl;
    _dataClient.stop();
    getServerAnswer();
}