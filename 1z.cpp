 //алгоритмо RC4 - символ открытого (незашифрованного) текста будет зашифрован в зависимости от ключа и положения символа в открытом тексте.
#include <iostream>
#include <fstream>  // Для работы с файлами
#include <cstdio>  
#include <stdexcept>  // для исключений
#include <cstring>   // для memcpy

#pragma warning (disable: 4996)

class encoder final {
    unsigned char* key;
    size_t key_s;

    unsigned char* allocateKey(size_t size);
public:
    encoder(unsigned char const*, size_t); //инициализация
    encoder(const encoder& other); 
    encoder& operator=(const encoder&);
    void mutator(unsigned char*, size_t); //проверка валидности данных(ключа) при его изменении
    void encode(char const*, char const*) const; //шифрование/дешифрование
    ~encoder() noexcept;
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

unsigned char* encoder::allocateKey(size_t size) { // Для выделения памяти и обработки bad_alloc
    try {
        return new unsigned char[size];
    }
    catch (const std::bad_alloc&) {
        throw std::runtime_error("Failed to allocate memory for key.");
    }
}

encoder::encoder(const unsigned char* _key, size_t _key_s): key(nullptr), key_s(_key_s) {
    if (_key == nullptr || _key_s == 0) {
        throw std::invalid_argument("Key cannot be null or empty.");
    }
    key = allocateKey(key_s); 
    memcpy(key, _key, key_s);
}

encoder::encoder(const encoder& other): key(nullptr), key_s(other.key_s) { 
    key = allocateKey(key_s); 
    memcpy(key, other.key, key_s);
}


encoder& encoder::operator=(const encoder& other) {
    if (this == &other) {
        return *this;
    }

    unsigned char* new_key = allocateKey(other.key_s);
    memcpy(new_key, other.key, other.key_s);

    delete[] key;
    key = new_key;
    key_s = other.key_s;
    return *this;
}

void encoder::mutator(unsigned char* _key, size_t _key_s) {
    if (_key == nullptr || _key_s == 0) {
        throw std::invalid_argument("Key cannot be null or empty."); 
    }
    unsigned char* new_key = allocateKey(_key_s);
    memcpy(new_key, _key, _key_s);

    delete[] key;
    key = new_key;  
    key_s = _key_s;  
}

void encoder::encode(char const* input_f, char const* output_f) const{
    // std::ostream std::ofstream
  
    std::ifstream inputFile(input_f, std::ios::binary);

    if (!inputFile.is_open()) {
        throw std::runtime_error("Failed to open input file.");
    }

    std::ofstream outputFile(output_f, std::ios::binary);

    if (!outputFile.is_open()) {
        inputFile.close();
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
    while (inputFile.read(reinterpret_cast<char*>(&byte), 1)) {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;
        std::swap(S[i], S[j]);
        unsigned char key_stream = S[(S[i] + S[j]) % 256];
        unsigned char result = byte ^ key_stream;

        outputFile.write(reinterpret_cast<char*>(&result), 1); 
    }

    inputFile.close();
    outputFile.close();
}

encoder::~encoder() noexcept {
    delete[] key;
}
