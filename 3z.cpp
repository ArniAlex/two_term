#include <iostream>
#include <cmath>

class complex_number {
    double real_part;
    double imag_part;

public:
    complex_number(double real = 0, double imaginary = 0) : real_part(real), imag_part(imaginary) {};
    
    complex_number operator+(const complex_number& obj) const { return complex_number(real_part + obj.real_part, imag_part + obj.imag_part); };
    complex_number& operator+=(const complex_number& obj) {
        real_part += obj.real_part;
        imag_part += obj.imag_part;
        return *this;
    };

    complex_number operator-(const complex_number& obj) const { return complex_number(real_part - obj.real_part, imag_part - obj.imag_part); };
    complex_number& operator-=(const complex_number& obj) {
        real_part -= obj.real_part;
        imag_part -= obj.imag_part;
        return *this;
    };

    complex_number operator*(const complex_number& obj) const { return complex_number(real_part * obj.real_part - imag_part * obj.imag_part, real_part * obj.imag_part + imag_part * obj.real_part); }; 
    complex_number& operator*=(const complex_number& obj) {
        double temp_real = real_part * obj.real_part - imag_part * obj.imag_part;
        imag_part = real_part * obj.imag_part + imag_part * obj.real_part;
        real_part = temp_real; 
        return *this;
    };

    complex_number operator/(const complex_number& obj) const { 
        if ((real_part * real_part + imag_part * imag_part) == 0) {
            throw std::runtime_error("Division cannot be performed.");
        }
        return complex_number((real_part * obj.real_part + imag_part * obj.imag_part) / (real_part * real_part + imag_part * imag_part), (imag_part * obj.real_part - real_part * obj.imag_part) / (real_part * real_part + imag_part * imag_part));
    };
    complex_number& operator/=(const complex_number& obj) {
        if ((real_part * real_part + imag_part * imag_part) == 0) {
            throw std::runtime_error("Division cannot be performed.");
        }
        double temp_real = (real_part * obj.real_part + imag_part * obj.imag_part) / (real_part * real_part + imag_part * imag_part);
        imag_part = (imag_part * obj.real_part - real_part * obj.imag_part) / (real_part * real_part + imag_part * imag_part);
        real_part = temp_real;
        return *this;
    };

    double mod() const { return std::sqrt(real_part * real_part + imag_part * imag_part); };
    double arg() const { return std::atan2(imag_part, real_part); };  // учитывает знаки аргументов

    friend std::ostream& operator<<(std::ostream& out, const complex_number& number) { 
        out << number.real_part << '+' << number.imag_part << 'i'; 
        return out;
    };
    friend std::istream& operator>>(std::istream& in, complex_number& number) {
        std::cout << "Real part: ";
        in >> number.real_part;
        std::cout << "Imaginary part: ";
        in >> number.imag_part;
        return in; 
    };
};

int main()
{
    complex_number first(3, 5), second(67);

    std::cout << "Numbers: " << first << ", " << second << std::endl;
    //операторные методы
    std::cout << "Addition: " << first + second << std::endl; 
    first += second;
    std::cout << "The first number after 'first+=second': " << first << std::endl;
    
    std::cout << "Subtraction: " << first - second << std::endl;
    second -= first;
    std::cout << "The second number after 'second -= first': " << second << std::endl;

    std::cout << "Multiplication: " << first * second << std::endl;
    first *= second;
    std::cout << "The first number after 'first *= second': " << first << std::endl; 
    
    try {
        std::cout << "Division the first number by the second: " << first / second << std::endl;
        second /= first;
        std::cout << "The second number after 'second /= first': " << second << std::endl;
    } 
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    std::cout << "The module of the first number: " << first.mod() << std::endl;
    std::cout << "The argument of the second number: " << second.arg() << std::endl; 

    return 0;
}

