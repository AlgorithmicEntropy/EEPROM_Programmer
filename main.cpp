#include <iostream>

int stringlength(const char * buffer)
{
    int i = 0;
    while (buffer[i] != NULL)
    {
        i++;
    }
    return i;
}

void stringcopy(const char* source, char* dest)
{
    int i = 0;
    while (source[i] != NULL)
    {
        dest[i] = source[i];
        i++;
    }
    dest[i+1]=0x0;
}

int main(int argc, char ** argv, char** env) {
    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << std::endl;
    }

    int i = 0;
    while (env[i] != nullptr)
    {
        std::cout << env[i] << std::endl;
        i++;
    }

    return 0;
}