 itoc(char *c, int i)
// {
//     int index = -1;
//     char temp[10];
//     while (i != 0)
//     {
//         // printf("%d\n", i);
//         // printf("%d\n", i % 10);
//         temp[++index] = i % 10 + '0';
//         i /= 10;
//     }
//     c[index + 1] = '\0';
//     while (index >= 0)
//         c[i++] = temp[index--];
// }

// int ctoi(char *c, int *i)
// {
//     *i = 0;
//     int lent = strlen(c);
//     for (int index = 0; index < lent; index++)
//         *i = (*i * 10) + (int)(c[index] - '0');
// }
