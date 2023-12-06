#ifndef MY_CHAT_H
#define MY_CHAT_H

/*
    Function List On CHAT Class:

    string messageEncryption(string message);
    string messageDecryption(string message);
*/

class CHAT
{
    DES_Encryption DES;
    // char key[17] = "0123456789ABCDEF";
    char key[17] = "ABCDEF0123456789";

public:
    void messageEncryption(char *message)
    {
        DES.encrypt(message, key, 0);
    }
    void messageDecryption(char *message)
    {
        DES.encrypt(message, key, 1);
    }
};

#endif