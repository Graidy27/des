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
    char key[17];
    // char key[17] = "0123456789ABCDEF";
    // char key[17] = "ABCDEF0123456789";

public:
    void randomizeKey()
    {
        int max_char = 16;
        char hexa[max_char] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        char tempKey[17];
        long long int val = 1;
        srand(time(0));
        while (true)
        {
            val = 1;
            for (int i = 0; i < 5; i++)
                val = val * rand() % max_char;
            if (val != 1 && val != 0 && val != 2)
                break;
        }
        for (int i = 0; i < max_char; i++)
        {
            val = (val * (i + 1)) + 1;
            tempKey[i] = hexa[val % max_char];
        }
        tempKey[max_char] = '\0';
        strcpy(key, tempKey);
    }
    void getKey(char *varKey)
    {
        strcpy(varKey, key);
    }
    void setKey(char *varKey)
    {
        strcpy(key, varKey);
    }
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