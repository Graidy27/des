#include <stdio.h>
#include <string.h>
int turnCommen = 0;
int decrypt = 1;
const int pc_1[56] = {57, 49, 41, 33, 25, 17, 9,
                      1, 58, 50, 42, 34, 26, 18,
                      10, 2, 59, 51, 43, 35, 27,
                      19, 11, 3, 60, 52, 44, 36,
                      63, 55, 47, 39, 31, 23, 15,
                      7, 62, 54, 46, 38, 30, 22,
                      14, 6, 61, 53, 45, 37, 29,
                      21, 13, 5, 28, 20, 12, 4};
int num_leftShift[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1}; // number of bits to shift for each iteration
const int pc_2[48] = {14, 17, 11, 24, 1, 5,
                      3, 28, 15, 6, 21, 10,
                      23, 19, 12, 4, 26, 8,
                      16, 7, 27, 20, 13, 2,
                      41, 52, 31, 37, 47, 55,
                      30, 40, 51, 45, 33, 48,
                      44, 49, 39, 56, 34, 53,
                      46, 42, 50, 36, 29, 32};

// constants regarding the plain text
const int IP_t[64] = {58, 50, 42, 34, 26, 18, 10, 2, // intital permutation table
                      60, 52, 44, 36, 28, 20, 12, 4,
                      62, 54, 46, 38, 30, 22, 14, 6,
                      64, 56, 48, 40, 32, 24, 16, 8,
                      57, 49, 41, 33, 25, 17, 9, 1,
                      59, 51, 43, 35, 27, 19, 11, 3,
                      61, 53, 45, 37, 29, 21, 13, 5,
                      63, 55, 47, 39, 31, 23, 15, 7};
const int E_t[48] = {32, 1, 2, 3, 4, 5, // expantion table
                     4, 5, 6, 7, 8, 9,
                     8, 9, 10, 11, 12, 13,
                     12, 13, 14, 15, 16, 17,
                     16, 17, 18, 19, 20, 21,
                     20, 21, 22, 23, 24, 25,
                     24, 25, 26, 27, 28, 29,
                     28, 29, 30, 31, 32, 1};
int S[8][4][16] = { // S-box
    {
        {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
        {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
        {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
        {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},
    {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
     {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
     {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
     {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},
    {{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
     {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
     {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
     {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},
    {{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
     {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
     {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
     {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},
    {{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
     {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
     {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
     {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},
    {{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
     {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
     {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
     {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},
    {{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
     {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
     {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
     {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},
    {{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
     {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
     {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
     {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}};
const int P[32] = {16, 7, 20, 21,
                   29, 12, 28, 17,
                   1, 15, 23, 26,
                   5, 18, 31, 10,
                   2, 8, 24, 14,
                   32, 27, 3, 9,
                   19, 13, 30, 6,
                   22, 11, 4, 25};
const int P_1[64] = {40, 8, 48, 16, 56, 24, 64, 32,
                     39, 7, 47, 15, 55, 23, 63, 31,
                     38, 6, 46, 14, 54, 22, 62, 30,
                     37, 5, 45, 13, 53, 21, 61, 29,
                     36, 4, 44, 12, 52, 20, 60, 28,
                     35, 3, 43, 11, 51, 19, 59, 27,
                     34, 2, 42, 10, 50, 18, 58, 26,
                     33, 1, 41, 9, 49, 17, 57, 25};
void ascii2bin(char *input)
{
    char temp[8193];
    int inputLen = strlen(input);
    int index;
    for (int i = 0; i < inputLen; i++)
    {
        index = i * 8;
        for (int j = 7; j >= 0; j--)
        {
            temp[index + j] = (input[i] % 2) + '0';
            input[i] /= 2;
        }
    }
    index += 7;
    input[index + 1] = '\0';
    while (index >= 0)
        input[index] = temp[index--];
}
void bin2ascii(char *input)
{
    int temp2 = input[0] - '0';
    int len = strlen(input);
    for (int i = 1; i < len; i++)
    {
        if (i % 8 == 0)
        {
            input[(i / 8) - 1] = temp2;
            temp2 = 0;
        }
        temp2 *= 2;
        temp2 += input[i] - '0';
    }
    len /= 8;
    input[len - 1] = (char)temp2;
    input[len] = '\0';
}
void hex2bin(char *input)
{
    char temp[8193] = "";
    int inputLen = strlen(input);
    printf("%d\n", inputLen);
    for (int i = 0; i < inputLen; i++)
    {
        switch (input[i])
        {
        case '0':
            strcat(temp, "0000");
            break;
        case '1':
            strcat(temp, "0001");
            break;
        case '2':
            strcat(temp, "0010");
            break;
        case '3':
            strcat(temp, "0011");
            break;
        case '4':
            strcat(temp, "0100");
            break;
        case '5':
            strcat(temp, "0101");
            break;
        case '6':
            strcat(temp, "0110");
            break;
        case '7':
            strcat(temp, "0111");
            break;
        case '8':
            strcat(temp, "1000");
            break;
        case '9':
            strcat(temp, "1001");
            break;
        case 'A':
        case 'a':
            strcat(temp, "1010");
            break;
        case 'B':
        case 'b':
            strcat(temp, "1011");
            break;
        case 'C':
        case 'c':
            strcat(temp, "1100");
            break;
        case 'D':
        case 'd':
            strcat(temp, "1101");
            break;
        case 'E':
        case 'e':
            strcat(temp, "1110");
            break;
        case 'F':
        case 'f':
            strcat(temp, "1111");
            break;
        }
    }

    inputLen *= 4;
    for (int i = 0; i < inputLen; i++)
        input[i] = temp[i];
    input[inputLen] = '\0';
}
void bin2hex(char *input)
{
    int len = strlen(input);
    char temp[5];
    temp[4] = '\0';
    temp[0] = input[0];
    for (int i = 1; i <= len; i++)
    {
        if (i % 4 == 0)
        {
            if (temp[0] == '0')
            {
                if (temp[1] == '0')
                {

                    if (temp[2] == '0')
                    {
                        if (temp[3] == '0')
                            input[(i / 4) - 1] = '0';
                        else
                            input[(i / 4) - 1] = '1';
                    }
                    else
                    {
                        if (temp[3] == '0')
                            input[(i / 4) - 1] = '2';
                        else
                            input[(i / 4) - 1] = '3';
                    }
                }
                else
                {

                    if (temp[2] == '0')
                    {
                        if (temp[3] == '0')
                            input[(i / 4) - 1] = '4';
                        else
                            input[(i / 4) - 1] = '5';
                    }
                    else
                    {
                        if (temp[3] == '0')
                            input[(i / 4) - 1] = '6';
                        else
                            input[(i / 4) - 1] = '7';
                    }
                }
            }
            else
            {
                if (temp[1] == '0')
                {

                    if (temp[2] == '0')
                    {
                        if (temp[3] == '0')
                            input[(i / 4) - 1] = '8';
                        else
                            input[(i / 4) - 1] = '9';
                    }
                    else
                    {
                        if (temp[3] == '0')
                            input[(i / 4) - 1] = 'A';
                        else
                            input[(i / 4) - 1] = 'B';
                    }
                }
                else
                {

                    if (temp[2] == '0')
                    {
                        if (temp[3] == '0')
                            input[(i / 4) - 1] = 'C';
                        else
                            input[(i / 4) - 1] = 'D';
                    }
                    else
                    {
                        if (temp[3] == '0')
                            input[(i / 4) - 1] = 'E';
                        else
                            input[(i / 4) - 1] = 'F';
                    }
                }
            }
            if (i == len)
            {
                len /= 4;
                input[len] = '\0';
                return;
            }
        }
        temp[i % 4] = input[i];
    }
    len /= 4;
    input[len] = '\0';
}
void dec2bin(char *input, int n)
{
    int tempDiv = 8;
    for (int i = 0; i < 4; i++)
    {
        // printf("%d\n", i);
        if (n < tempDiv)
            input[i] = '0';
        else
        {
            input[i] = '1';
            n -= tempDiv;
        }
        tempDiv /= 2;
    }
}
void shift_bit(char *output, char *input, int n)
{
    int tempIndex = 0;
    int len = strlen(input);
    for (int i = n; i < len; i++)
        output[tempIndex++] = input[i];
    for (int i = 0; i < n; i++)
        output[tempIndex++] = input[i];
    output[len] = '\0';
}
char xor_add(char *output, char *s1, char *s2)
{
    int len = strlen(s1);
    for (int j = 0; j < len; j++)
    {
        if (s1[j] != s2[j])
            output[j] = '1';
        else
            output[j] = '0';
    }
}
void get_element_from_box(char *input, char *s, int k)
{
    int dec1 = 0, dec2 = 0, pwr = 0;
    dec1 = (int)(s[0] - '0') * 2 + (int)(s[5] - '0');
    for (int i = 1; i <= 4; i++)
    {
        dec2 *= 2;
        dec2 += (int)(s[i] - '0');
    }
    dec2bin(input, S[k][dec1][dec2]);
}
int main()
{
    int option, showBin, correctInput = 0;
    printf("What do we want to do?\n0: encrypt\n1: decrypt\n");
    do
    {
        printf("Choose: ");
        scanf("%d", &decrypt);
        if (decrypt == 0 || decrypt == 1)
        {
            correctInput = 1;
            break;
        }
        printf("Option \"%d\" is not found. Try again\n", option);
    } while (!correctInput);
    correctInput = 0;
    char options[][50] = {"standard ASCII characters", "Hexadecimal"};
    printf("Choose plaintext format\n0: standard ASCII characters\n1: Hexadecimal\n");
    do
    {
        printf("Format: ");
        scanf("%d", &option);
        if (option == 0 || option == 1)
        {
            correctInput = 1;
            showBin = option;
            break;
        }
        printf("Option \"%d\" is not found. Try again\n", option);
    } while (!correctInput);
    char plainText[8193];
    printf("Insert plaintext in %s format : ", options[option]);
    getchar();
    gets(plainText);
    if (turnCommen)
    {
        printf("\n---------------- Plain Text ----------------\n");
        printf("[%s]: %s\n", options[option], plainText);
    }
    if (option == 0)
    { // standard ASCII characters max 1024 character
        if (strlen(plainText) > 1024)
        {
            printf("Length exceed. Maximal 1024 %s\n", options[option]);
            return 0;
        }
        ascii2bin(plainText);
    }
    else
    { // Hexadecimal
        if (strlen(plainText) > 2048)
        {
            printf("Length exceed. Maximal 2048 %s character", options[option]);
            return 0;
        }
        hex2bin(plainText);
    }

    int textLen = strlen(plainText);
    while (textLen % 64 != 0)
        plainText[textLen++] = '0';
    plainText[textLen] = '\0';
    if (turnCommen)
    {
        printf("#### Binary ####\n", plainText);
        for (int commen = 0; commen < textLen; commen++)
        {
            printf("%c", plainText[commen]);
            if (commen % 8 == 7)
                printf(" ");
        }
        printf("\n---------------- End of Plain Text ----------------\n\n");
    }

    correctInput = 0;
    printf("Choose key format\n0: standard ASCII characters (tepat 8 karakter)\n1: Hexadecimal (tepat 16 karakter)\n");
    do
    {
        printf("Format: ");
        scanf("%d", &option);
        if (option == 0 || option == 1)
        {
            correctInput = 1;
            break;
        }
        printf("Option \"%d\" is not found. Try again\n", option);
    } while (!correctInput);
    char key_64[65];
    printf("Insert key in %s format : ", options[option]);
    getchar();
    gets(key_64);
    if (turnCommen)
    {
        printf("\n---------------- Key Text----------------\n");
        printf("[%s]: %s\n", options[option], key_64);
    }
    if (option == 0)
    { // standard ASCII characters max 8 character
        if (strlen(key_64) != 8)
        {
            printf("Wrong %s input\n", options[option]);
            return 0;
        }
        ascii2bin(key_64);
    }
    else
    { // Hexadecimal
        if (strlen(key_64) != 16)
        {
            printf("Wrong %s input\n", options[option]);
            return 0;
        }
        hex2bin(key_64);
    }
    char key_56[60], key_firstHalf[30], key_secondHalf[30];
    int i;
    for (i = 0; i < 56; i++)
        key_56[i] = key_64[pc_1[i] - 1];
    for (i = 0; i < 28; i++)
    {
        key_firstHalf[i] = key_56[i];
        key_secondHalf[i] = key_56[i + 28];
    }
    key_56[56] = key_firstHalf[28] = key_secondHalf[28] = '\0';

    char L_key[16][30], R_key[16][30], keys_48[16][50], keys_56[16][60];
    shift_bit(L_key[0], key_firstHalf, num_leftShift[0]); // shifting the bits according to num_leftSifht
    shift_bit(R_key[0], key_secondHalf, num_leftShift[0]);
    for (int i = 1; i < 16; i++)
    {
        shift_bit(L_key[i], L_key[i - 1], num_leftShift[i]);
        shift_bit(R_key[i], R_key[i - 1], num_leftShift[i]);
    }
    for (int i = 0; i < 16; i++)
    {
        strcpy(keys_56[i], L_key[i]);
        strcat(keys_56[i], R_key[i]);
        for (int j = 0; j < 48; j++)
            keys_48[i][j] = keys_56[i][pc_2[j] - 1];
        keys_48[i][48] = '\0';
    }
    if (turnCommen)
    {
        printf("\n#### key 64 bits ####\n");
        for (int commen = 0; commen < 64; commen++)
        {
            printf("%c  ", key_64[commen]);
            if (commen % 8 == 7)
                puts("");
        }
        printf("\n\n#### permutasi key pertama ####\n");
        for (int commen = 0; commen < 56; commen++)
        {
            if (pc_1[commen] < 8)
                printf("\'", commen + 1);
            else
                printf(" ");
            printf("%d  ", pc_1[commen]);
            if (commen % 7 == 6)
                puts("");
        }
        printf("\n\n#### key 56 bits (hasil permutasi pertama) ####\n");
        printf("key 64 index 0-8: ");
        for (int commen = 0; commen < 8; commen++)
            printf("%c", key_64[commen]);
        puts("");
        for (int commen = 0; commen < 56; commen++)
        {
            if (pc_1[commen] < 9)
                printf("%d:", pc_1[commen]);
            else
                printf("  ");
            printf("%c  ", key_56[commen]);
            if (commen % 7 == 6)
                puts("");
            if (commen == 27)
                puts("");
        }
        printf("\n\n #### key dibagi 2 ####\n** Left **\n");
        for (int commen = 0; commen < 28; commen++)
        {
            printf("%c  ", key_firstHalf[commen]);
            if (commen % 7 == 6)
                puts("");
        }

        printf("\n** Right **\n");
        for (int commen = 0; commen < 28; commen++)
        {
            printf("%c  ", key_secondHalf[commen]);
            if (commen % 7 == 6)
                puts("");
        }
        printf("\n #### shifting (tuk contoh, digunakan baris pertama) ####");
        printf("\nnum_leftShift[0]: %d\n", num_leftShift[0]);
        printf("          <<--\nLeft    : ");
        for (int commen = 0; commen < 28; commen++)
        {
            printf("%c", key_firstHalf[commen]);
            if (commen % 4 == 3)
                printf(" ");
        }
        printf("\nL_key[0]: ");
        for (int commen = 0; commen < 28; commen++)
        {
            printf("%c", L_key[0][commen]);
            if (commen % 4 == 3)
                printf(" ");
        }
        printf("\n          <<--\nright   : ", key_secondHalf);
        for (int commen = 0; commen < 28; commen++)
        {
            printf("%c", key_secondHalf[commen]);
            if (commen % 4 == 3)
                printf(" ");
        }
        printf("\nR_key[0]: ");
        for (int commen = 0; commen < 28; commen++)
        {
            printf("%c", R_key[0][commen]);
            if (commen % 4 == 3)
                printf(" ");
        }
        printf("\n\n\n #### digabung untuk membentuk key round 56 bit ####\nkeys round 1 (keys_56[0]): ");
        for (int commen = 0; commen < 56; commen++)
        {
            printf("%c", keys_56[0][commen]);
            if (commen % 4 == 3)
                printf(" ");
        }
        printf("\n\n\n #### permutasi key kedua ( digunakan untuk xor ) ####\n");
        for (int commen = 0; commen < 48; commen++)
        {
            printf("%c", keys_48[0][commen]);
            if (commen % 4 == 3)
                printf(" ");
        }
        puts("");
    }

    // Plain Text
    int n_iter = strlen(plainText);
    int iterPart = 0;
    char IP[65], L[33], R[33], R_48[3][49], R_xor_K[3][48], s[3][8][7], L_32[3][33], R_32[3][33], s_1[3][33], P_R[3][70];
    int curr = 0, next = 1;
    IP[64] = L[32] = R[32] = R_48[0][48] = R_48[1][48] = R_xor_K[0][48] = R_xor_K[1][48] = '\0';
    R_32[0][32] = R_32[1][32] = L_32[0][32] = L_32[1][32] = '\0';
    s_1[0][32] = s_1[1][32] = P_R[0][32] = P_R[1][32] = '\0';
    for (int i = 0; i < 8; i++)
        s[0][i][6] = s[1][i][6] = '\0';
    n_iter /= 64;
    for (int iter = 0; iter < n_iter; iter++)
    {
        iterPart = iter * 64;
        for (int i = 0; i < 64; i++)
            IP[i] = plainText[IP_t[i] - 1 + iterPart];
        for (int i = 0; i < 32; i++)
        {
            L[i] = IP[i];
            R[i] = IP[i + 32];
        }
        if (turnCommen)
        {
            printf("\n\n--------------------- Plain Text (Round Demo) ---------------------\n");
            printf("IP: ");
            for (int commen = 0; commen < 64; commen++)
            {
                printf("%c", IP[commen]);
                if (commen % 4 == 3)
                    printf(" ");
            }
            puts("");
            printf("L : ");
            for (int commen = 0; commen < 32; commen++)
            {
                printf("%c", L[commen]);
                if (commen % 4 == 3)
                    printf(" ");
            }
            puts("");
            printf("R :                                         ");
            for (int commen = 0; commen < 32; commen++)
            {
                printf("%c", R[commen]);
                if (commen % 4 == 3)
                    printf(" ");
            }
            puts("");
        }
        for (int i = 0; i < 48; i++)
            R_48[curr][i] = R[E_t[i] - 1];
        if (!decrypt)
            xor_add(R_xor_K[curr], R_48[curr], keys_48[0]);
        else
            xor_add(R_xor_K[curr], R_48[curr], keys_48[15]);

        if (turnCommen)
        {

            printf("R_48    : ");
            for (int commen = 0; commen < 48; commen++)
            {
                printf("%c", R_48[curr][commen]);
                if (commen % 4 == 3)
                    printf(" ");
            }
            puts("");
            printf("keys_48 : ");
            for (int commen = 0; commen < 48; commen++)
            {
                printf("%c", keys_48[curr][commen]);
                if (commen % 4 == 3)
                    printf(" ");
            }
            puts("");
            printf("R_xor_K : ");
            for (int commen = 0; commen < 48; commen++)
            {
                printf("%c", R_xor_K[curr][commen]);
                if (commen % 4 == 3)
                    printf(" ");
            }
            puts("");
        }

        int jTemp = 0;
        for (int i = 0; i < 48; i++)
            s[curr][i / 6][i % 6] = R_xor_K[curr][i];

        for (int i = 0; i < 8; i++)
            get_element_from_box(&s_1[curr][i * 4], s[curr][i], i);
        if (turnCommen)
        {
            printf("s       : ");
            for (int commen = 0; commen < 48; commen++)
            {
                printf("%c", s[curr][commen / 6][commen % 6]);
                if (commen % 6 == 5)
                    printf(" ");
            }
            puts("");
            printf("s_1     : ");
            for (int commen = 0; commen < 32; commen++)
            {
                printf("%c", s_1[curr][commen]);
                if (commen % 4 == 3)
                    printf(" ");
            }
            puts("");
        }

        for (int i = 0; i < 32; i++)
            P_R[curr][i] = s_1[curr][P[i] - 1];
        xor_add(R_32[curr], P_R[curr], L);
        if (turnCommen)
        {
            printf("\nP_R          : ");
            for (int commen = 0; commen < 32; commen++)
            {
                printf("%c", P_R[curr][commen]);
                if (commen % 4 == 3)
                    printf(" ");
            }
            puts("");
            printf("R            : ");
            for (int commen = 0; commen < 32; commen++)
            {
                printf("%c", R[commen]);
                if (commen % 4 == 3)
                    printf(" ");
            }
            puts("");
            printf("R_32 (xor) : ");
            for (int commen = 0; commen < 32; commen++)
            {
                printf("%c", R_32[curr][commen]);
                if (commen % 4 == 3)
                    printf(" ");
            }
            puts("");
        }

        for (int i = 0; i < 32; i++)
            L_32[curr][i] = R[i];
        for (int i = 1; i < 16; i++)
        { // sisa 15 round of iteration
            curr = next;
            next = (next + 1) % 2;
            for (int j = 0; j < 48; j++)
                R_48[curr][j] = R_32[next][E_t[j] - 1];
            if (!decrypt)
                xor_add(R_xor_K[curr], R_48[curr], keys_48[i]);
            else
                xor_add(R_xor_K[curr], R_48[curr], keys_48[15 - i]);

            for (int i = 0; i < 48; i++)
                s[curr][i / 6][i % 6] = R_xor_K[curr][i];

            for (int i = 0; i < 8; i++)
                get_element_from_box(&s_1[curr][i * 4], s[curr][i], i);
            for (int i = 0; i < 32; i++)
            {
                P_R[curr][i] = s_1[curr][P[i] - 1];
                L_32[curr][i] = R_32[next][i];
            }

            xor_add(R_32[curr], P_R[curr], L_32[next]);
        }

        char RL[65];
        strcpy(RL, R_32[curr]);
        strcat(RL, R_32[next]);

        // char tempp[65];
        for (int i = 0; i < 64; i++)
            plainText[i + iterPart] = RL[P_1[i] - 1];
    }
    if (decrypt)
        bin2ascii(plainText);
    else
        bin2hex(plainText);
    printf("%s\n", plainText);

    return 0;
}
