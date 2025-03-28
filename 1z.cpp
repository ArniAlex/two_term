/* 1. Реализовать класс encoder. В классе определить и реализовать:
 * ● конструктор, принимающий ключ шифрования в виде массив байтов типа unsigned char const * и размер
 * этого массива
 * ● mutator для значения ключа
 * ● метод encode, который принимает путь ко входному файлу (типа char const *), выходному файлу (типа
 * char const *) и флаг, отвечающий за то, выполнять шифрование или дешифрование (типа bool) и
 * выполняет процесс шифрования/дешифрования файла
 * Шифрование/дешифрование файлов выполняется алгоритмом RC4. Структура содержимого файлов произвольна. //по байту за раз
 * Продемонстрировать работу класса, произведя шифрование/дешифрование различных файлов: текстовых,
 * графических, аудио, видео, исполняемых.
 */

 //алгоритмо RC4 - символ открытого (незашифрованного) текста будет зашифрован в зависимости от ключа и положения символа в открытом тексте.
#include <iostream>
#include <fstream>
#include <stdexcept>  // для исключений
#include <cstring>   // для memcpy

class encoder final {
    unsigned char* key;
    size_t key_s;
public:
    encoder(unsigned char const*, size_t); //инициализация
    encoder(const encoder& other); 
    encoder& operator=(const encoder&);
    void mutator(unsigned char*, size_t); //проверка валидности данных(ключа) при его изменении
    void encode(char const*, char const*); //шифрование/дешифрование
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
    key = new unsigned char[key_s]; // обработать исключение в main
    memcpy(key, _key, key_s);
}

encoder::encoder(const encoder& other): key_s(other.key_s) {
    key = new unsigned char[key_s]; 
    memcpy(key, other.key, key_s);  
}

encoder& encoder::operator=(const encoder& other) {
    if (this == &other) {
        return *this;
    }
    delete[] key;

    key_s = other.key_s;
    key = new unsigned char[key_s];
    memcpy(key, other.key, key_s);
}
void encoder::mutator(unsigned char* _key, size_t _key_s) {
    if (_key == nullptr || _key_s == 0) {
        throw std::invalid_argument("Key cannot be null or empty."); // проверить исключение
    }
    key_s = _key_s;
    delete[] key;
    key = new unsigned char[key_s]; // обработать исключение в main
    memcpy(key, _key, key_s);
}

void encoder::encode(char const* input_f, char const* output_f) {
    std::ifstream inputFile(input_f, std::ios::binary);
    std::ofstream outputFile(output_f, std::ios::binary);

    if (!inputFile.is_open()) {
        throw std::runtime_error("Failed to open input file.");
    }

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
    while (inputFile.get((char&)byte)) {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;
        std::swap(S[i], S[j]);
        unsigned char key_stream = S[(S[i] + S[j]) % 256];
        unsigned char result = byte ^ key_stream;

        outputFile.put(result);
    }

    inputFile.close();
    outputFile.close();
}
//нужно перенести инициализацию списка S в конструктор и мутатор, а в этой функции оставить открытие файлов и xor

encoder::~encoder() noexcept {
    delete[] key;
}
