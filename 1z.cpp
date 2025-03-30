 //алгоритмо RC4 - символ открытого (незашифрованного) текста будет зашифрован в зависимости от ключа и положения символа в открытом тексте.
#include <iostream>
#include <cstdio>  
#include <stdexcept>  // для исключений
#include <cstring>   // для memcpy

#pragma warning (disable: 4996)

class encoder final {
    unsigned char* key;
    size_t key_s;
public:
    encoder(unsigned char const*, size_t); //инициализация
    encoder(const encoder& other); 
    encoder& operator=(const encoder&);
    void mutator(unsigned char*, size_t); //проверка валидности данных(ключа) при его изменении
    void encode(char const*, char const*) const; //шифрование/дешифрование
    ~encoder() noexcept; //освобождает память
};

int main() {
    try {
        unsigned char my_key[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
        encoder enc(my_key, sizeof(my_key));
        enc.encode("об3.jpg", "out.txt"); // Шифрование
        std::cout << "The file is encrypted." << std::endl;
        enc.encode("out.txt", "outp.jpg"); // Дешифрование
        std::cout << "The file is decrypted." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

encoder::encoder(const unsigned char* _key, size_t _key_s) {
    if (_key == nullptr || _key_s == 0) {
        throw std::invalid_argument("Key cannot be null or empty.");
    }
    key_s = _key_s;
    try {
        key = new unsigned char[key_s];
        memcpy(key, _key, key_s);
    }
    catch (const std::bad_alloc&) {
        throw std::runtime_error("Failed to allocate memory for key.");
    }
}

encoder::encoder(const encoder& other): key_s(other.key_s) {
    try {
        key = new unsigned char[key_s];
        memcpy(key, other.key, key_s);
    }
    catch (const std::bad_alloc&) {
        throw std::runtime_error("Failed to allocate memory in copy constructor.");
    }
}

encoder& encoder::operator=(const encoder& other) {
    if (this == &other) {
        return *this;
    }
    try {
        unsigned char* new_key = new unsigned char[other.key_s];  
        memcpy(new_key, other.key, other.key_s);

        delete[] key;  
        key = new_key;
        key_s = other.key_s;
    }
    catch (const std::bad_alloc&) {
        throw std::runtime_error("Failed to allocate memory in assignment operator.");
    }
    return *this;
}
void encoder::mutator(unsigned char* _key, size_t _key_s) {
    if (_key == nullptr || _key_s == 0) {
        throw std::invalid_argument("Key cannot be null or empty."); 
    }
    try {
        unsigned char* new_key = new unsigned char[_key_s];  

        memcpy(new_key, _key, _key_s);

        delete[] key;   
        key = new_key;
        key_s = _key_s;
    }
    catch (const std::bad_alloc&) {
        throw std::runtime_error("Failed to allocate memory in mutator.");
    }
}

void encoder::encode(char const* input_f, char const* output_f) const{
    FILE* inputFile = fopen(input_f, "rb");

    if (!inputFile) {
        throw std::runtime_error("Failed to open input file.");
    }
    FILE* outputFile = fopen(output_f, "wb");

    if (!outputFile) {
        fclose(inputFile); 
        throw std::runtime_error("Failed to open output file.");
    }

    unsigned char S[256];

    for (int i = 0; i < 256; ++i) {
        S[i] = i;
    }

    int j = 0;
    for (int i = 0; i < 256; ++i) {
        j = (j + S[i] + key[i % key_s]) % 256;
        std::swap(S[i], S[j]);
    }

    int i = 0;
    j = 0;
    unsigned char byte;
    while (fread(&byte, 1, 1, inputFile) == 1) {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;
        std::swap(S[i], S[j]);
        unsigned char key_stream = S[(S[i] + S[j]) % 256];
        unsigned char result = byte ^ key_stream;

        fwrite(&result, 1, 1, outputFile);
    }

    fclose(inputFile);
    fclose(outputFile);
}

encoder::~encoder() noexcept {
    delete[] key;
}
