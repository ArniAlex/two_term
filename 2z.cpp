#include <iostream>
#include <string>
#pragma warning (disable: 4996) 


class logical_values_array {
    unsigned int _value;

public:
    unsigned int get_value() const { return _value; };

    logical_values_array(unsigned int val = 0) : _value(val) {};

    //методы всякие

    // Логические операции
    logical_values_array invert() const { return logical_values_array(~_value); }; // Инверсия 
    logical_values_array conjunction(const logical_values_array& second_obj) const { return logical_values_array(_value & second_obj._value); };  // Конъюнкция
    logical_values_array disjunction(const logical_values_array& second_obj) const { return logical_values_array(_value | second_obj._value); };  // Дизъюнкция 
    logical_values_array implication(const logical_values_array& second_obj) const { return logical_values_array(~_value | second_obj._value); };  // Импликация
    logical_values_array coimplication(const logical_values_array& second_obj) const { return logical_values_array(_value | ~second_obj._value); };  // Коимпликация
    logical_values_array xor_op(const logical_values_array& second_obj) const { return logical_values_array(_value ^ second_obj._value); };  // Сложение по модулю 2
    logical_values_array equivalence(const logical_values_array& second_obj) const { return logical_values_array(~(_value ^ second_obj._value)); }; // Эквивалентность
    logical_values_array peirce_arrow(const logical_values_array& second_obj) const { return logical_values_array(~(_value | second_obj._value)); };  // Стрелка Пирса NOR
    logical_values_array sheffer_stroke(const logical_values_array& second_obj) const { return logical_values_array(~(_value & second_obj._value)); };  // Штрих Шеффера NAND

    static bool equals(const logical_values_array& obj1, const logical_values_array& obj2) { return obj1._value == obj2._value; };

    bool get_bit(size_t position) const { return _value & (1 << position); };

    bool operator[](size_t position) const { return get_bit(position); };

    void to_binary_string(char* buffer) const
    {
        unsigned int temp_value = _value; 
        size_t buffer_len = strlen(buffer);  
        if (buffer_len == 0) {
            throw std::runtime_error("The size of the buffer is zero.");
        }

        for (size_t i = 0; i < buffer_len - 1; ++i) {  
            buffer[i] = '0';
        }
        buffer[buffer_len - 1] = '\0';

        for (size_t i = buffer_len - 2; ; --i) {
            buffer[i] = (temp_value & 1) ? '1' : '0';
            temp_value >>= 1;
            if (temp_value == 0) {
                break; 
            }
            if (i == 0 && temp_value != 0) {
                throw std::runtime_error("The size of array is insufficient.");
            }
            if (i == 0) break;
        }
    };

};

int main() {
    logical_values_array first(4), second(56);
    std::cout << "First value: " << first.get_value() << '\n' << "Second value: " << second.get_value() << std::endl;
    
    std::cout << "Invert of first: " << first.invert().get_value() << std::endl;
    std::cout << "Conjunction of first and second: " << first.conjunction(second).get_value() << std::endl;
    std::cout << "Disjunction of first and second: " << first.disjunction(second).get_value() << std::endl;
    std::cout << "Implication of first to second: " << first.implication(second).get_value() << std::endl;
    std::cout << "Coimplication of first to second: " << first.coimplication(second).get_value() << std::endl;
    std::cout << "XOR of first and second: " << first.xor_op(second).get_value() << std::endl;
    std::cout << "Equivalence of first and second: " << first.equivalence(second).get_value() << std::endl;
    std::cout << "Peirce arrow (NOR) of first and second: " << first.peirce_arrow(second).get_value() << std::endl;
    std::cout << "Sheffer stroke (NAND) of first and second: " << first.sheffer_stroke(second).get_value() << std::endl;

    std::cout << "First and second are equal: " << logical_values_array::equals(first, second) << std::endl; 
    std::cout << "Bit 1 of first: " << first[1] << std::endl;  // 0
    std::cout << "Bit 2 of first: " << first.get_bit(2) << std::endl;  // 1

    char binary_buffer[9]; 
    strcpy(binary_buffer, "00000000"); 
    try {
        first.to_binary_string(binary_buffer);
        std::cout << "Binary representation of first: " << binary_buffer << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error converting to binary: " << e.what() << std::endl;
    }
    return 0;
}
