// test character promotion
// - diagnostic driven by -jw option

int chrprom()
{
    char ch = 'a';
    char arr[2] = { 'b', 'c' };
    int i = ch;
    i = (int)ch;
    i = ch + 1;
    i = arr[1];
    i = arr[ch];
    i = ch[arr];
    i = ch;
    return i;
}
