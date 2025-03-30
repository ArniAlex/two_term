#include <iostream>
#include <stdexcept>
#include <cmath>    //std::abs
#include <algorithm> //std::swap
#include <new>      // Для std::bad_alloc

struct my_pair { 
    double first;
    size_t second; 

    my_pair(double f = 0.0, size_t s = 0) : first(f), second(s) {};
};


class matrix {
    double** m;
    size_t rows;
    size_t columns;

public:
    matrix(size_t r, size_t c) : rows(r), columns(c) {
        try {
            m = new double* [rows];
            for (size_t i = 0; i < rows; ++i) {
                m[i] = new double[columns];
                for (size_t j = 0; j < columns; ++j) {
                    m[i][j] = 0.0;
                }
            }
        }
        catch (const std::bad_alloc& e) {
            for (size_t i = 0; i < rows; ++i) {
                delete[] m[i];
            }
            delete[] m;
            m = nullptr;
            rows = 0;
            columns = 0;

            throw MatrixException("Failed to allocate memory for matrix.");
        }
    }

    matrix(const matrix& other) : rows(other.rows), columns(other.columns) {
        m = new double* [rows];
        for (size_t i = 0; i < rows; ++i) {
            m[i] = new double[columns];
            for (size_t j = 0; j < columns; ++j) {
                m[i][j] = other.m[i][j];
            }
        }
    }

    matrix& operator=(const matrix& other) {
        if (this == &other) {
            return *this;
        }

        for (size_t i = 0; i < rows; ++i) {
            delete[] m[i];
        }
        delete[] m;

        rows = other.rows;
        columns = other.columns;

        try {
            m = new double* [rows];
            for (size_t i = 0; i < rows; ++i) {
                m[i] = new double[columns];
                for (size_t j = 0; j < columns; ++j) {
                    m[i][j] = other.m[i][j];
                }
            }
        }
        catch (const std::bad_alloc& e) {
            //Восстановить предыдущее состояние, освободить память
            for (size_t i = 0; i < rows; ++i) {
                delete[] m[i];
            }
            delete[] m;
            m = nullptr;
            rows = 0;
            columns = 0;
            throw MatrixException("Failed to allocate memory for matrix in assignment operator.");

        }
        return *this;
    };

    matrix operator+(const matrix& second) const {
        if (rows != second.rows || columns != second.columns) {
            throw InvalidDimensions("Matrices must have the same dimensions for addition.");
        }
        matrix result(rows, columns);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < columns; ++j) {
                result.m[i][j] = m[i][j] + second.m[i][j];
            }
        }
        return result;
    };

    matrix operator*(double num) const {
        matrix result(rows, columns);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < columns; ++j) {
                result.m[i][j] = m[i][j] * num;
            }
        }
        return result;
    };

    friend matrix operator*(double num, const matrix& matr) {
        matrix result(matr.rows, matr.columns);
        for (size_t i = 0; i < matr.rows; ++i) {
            for (size_t j = 0; j < matr.columns; ++j) {
                result.m[i][j] = matr.m[i][j] * num;
            }
        }
        return result;
    };

    matrix operator*(const matrix& second) const {
        if (columns != second.rows) {
            throw std::invalid_argument("Incompatible matrix dimensions for multiplication.");
        }

        matrix result(rows, second.columns);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < second.columns; ++j) {
                for (size_t k = 0; k < columns; ++k) {
                    result.m[i][j] += m[i][k] * second.m[k][j];
                }
            }
        }

        return result;
    };

    matrix operator-(const matrix& second) const {
        if (!(rows == second.rows && columns == second.columns)) {
            throw std::invalid_argument("Matrices must have the same dimensions for subtraction.");
        }
        matrix result(rows, columns);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < columns; ++j) {
                result.m[i][j] = m[i][j] - second.m[i][j];
            }
        }
        return result;
    };

    matrix transposed() const {
        matrix result(columns, rows);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < columns; ++j) {
                result.m[j][i] = m[i][j];
            }
        }
        return result;
    };

    double determinant() const {
        if (rows != columns) {
            throw NonSquareMatrix("Matrix must be square to calculate the determinant.");
        }
        if (rows == 1) {
            return m[0][0];
        }
        if (rows == 2) {
            return m[0][0] * m[1][1] - m[0][1] * m[1][0];
        }

        double det = 0;
        matrix submatrix(rows - 1, columns - 1);
        for (size_t i = 0; i < columns; ++i) {
            getSubmatrix(0, i, submatrix);
            det += m[0][i] * std::pow(-1, i) * submatrix.determinant();
        }

        return det;
    };


    matrix inverse_matrix() const {
        if (rows != columns) {
            throw NonSquareMatrix("Matrix must be square to calculate the inverse matrix.");
        }
        double det = determinant();
        if (isEqual(det, 0.0)) {
            throw SingularMatrix("The matrix has no inverse because the determinant is zero.");
        }

        matrix adjugate(rows, columns);
        matrix submatrix(rows - 1, columns - 1);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < columns; ++j) {
                getSubmatrix(i, j, submatrix);
                adjugate.m[i][j] = std::pow(-1, i + j) * submatrix.determinant();
            }
        }
        matrix inverse = adjugate.transposed() * (1.0 / det);
        return inverse;
    };


    //для [][]
    class overload {
        double* str;
        size_t col;
    public:
        overload(double* r, size_t c) : str(r), col(c) {};

        double& operator[](size_t cols) {
            if (cols < 0 || cols >= col) {
                throw std::out_of_range("Index out of range");
            }
            return str[cols];
        }

        overload(const overload& other) = default;
        overload& operator=(const overload& other) = default;
        ~overload() = default;
    };

    overload operator[](size_t row) {
        if (row < 0 || row >= rows) {
            throw std::out_of_range("Index out of range");
        }
        return overload(m[row], columns); //указатель на строку
    };

    ~matrix() {
        for (size_t i = 0; i < rows; ++i) {
            delete[] m[i];
        }
        delete[] m;
    };

    friend std::ostream& operator<<(std::ostream& os, const matrix& mat) {
        for (size_t i = 0; i < mat.rows; ++i) {
            for (size_t j = 0; j < mat.columns; ++j) {
                os << mat.m[i][j] << " ";
            }
            os << std::endl;
        }
        return os;
    };

    //классы для исключений
    class MatrixException : public std::exception { //базовый
    public:
        MatrixException(const std::string& message) : message_(message) {}
        const char* what() const noexcept override { return message_.c_str(); }
    private:
        std::string message_;
    };

    class InvalidDimensions : public MatrixException {
    public:
        InvalidDimensions(const std::string& message) : MatrixException(message) {}
    };

    class NonSquareMatrix : public MatrixException {
    public:
        NonSquareMatrix(const std::string& message) : MatrixException(message) {}
    };

    class SingularMatrix : public MatrixException {
    public:
        SingularMatrix(const std::string& message) : MatrixException(message) {}
    };

private:
    //метод Гаусса (приведение к верхнетреугольному виду)
    my_pair gaussianElimination(matrix& a) const {
        size_t n = a.rows;
        double det = 1.0;
        size_t sign = 1; //меняется, когда меняем строки местами

        for (size_t i = 0; i < n; ++i) {
            size_t max_row = i;
            for (size_t k = i + 1; k < n; ++k) {
                if (std::abs(a.m[k][i]) > std::abs(a.m[max_row][i])) {
                    max_row = k;
                }
            }

            if (max_row != i) {
                std::swap(a.m[i], a.m[max_row]);
                sign *= -1;
            }

            if (isEqual(a.m[i][i], 0.0)) {
                return my_pair(0.0, sign); //либо выбросить исключение, т.к матрица вырождена
            }

            for (size_t k = i + 1; k < n; ++k) {
                double factor = a.m[k][i] / a.m[i][i];
                for (size_t j = i; j < n; ++j) {
                    a.m[k][j] -= factor * a.m[i][j];
                }
            }
        }
        for (size_t i = 0; i < n; ++i) {
            det *= a.m[i][i];
        }

        return my_pair(det, sign);
    };

    void getSubmatrix(size_t row_rem, size_t col_rem, matrix& submatrix) const {
        size_t subRow = 0;
        for (size_t i = 0; i < rows; ++i) {
            if (i == row_rem) continue;
            size_t subCol = 0;
            for (size_t j = 0; j < columns; ++j) {
                if (j == col_rem) continue;
                submatrix.m[subRow][subCol] = m[i][j];
                ++subCol;
            }
            ++subRow;
        }
    };

    bool isEqual(double a, double b, double epsilon = 1e-9) const {
        return std::abs(a - b) < epsilon;
    };
};

int main() {
    try {
        matrix A(3, 3);
        matrix B(3, 3);
        matrix C(2, 2);
        matrix D(2, 3);

        A[0][0] = 1.0; A[0][1] = 2.0; A[0][2] = 8.0;
        A[1][0] = 3.0; A[1][1] = 4.0; A[1][2] = 67.0;
        A[2][0] = 8.0; A[2][1] = 12.0; A[2][2] = 3.0;

        B[0][0] = 5.0; B[0][1] = 6.0; B[0][2] = 4.0;
        B[1][0] = 7.0; B[1][1] = 8.0; B[1][2] = 12.0;
        B[2][0] = 74.0; B[2][1] = 7.0; B[2][2] = 5.0;

        C[0][0] = 1.0; C[0][1] = 0.0;
        C[1][0] = -1.0; C[1][1] = 5.0;

        D[0][0] = 1.0; D[0][1] = 2.0; D[0][2] = 17.0;
        D[1][0] = 22.0; D[1][1] = 5.0; D[1][2] = 6.0;

        std::cout << "Matrix A:\n" << A << std::endl;
        std::cout << "Matrix B:\n" << B << std::endl;
        std::cout << "Matrix C:\n" << C << std::endl;
        std::cout << "Matrix D:\n" << D << std::endl;

        matrix Sum = A + B;
        std::cout << "A + B:\n" << Sum << std::endl;

        matrix Diff = A - B;
        std::cout << "A - B:\n" << Diff << std::endl;

        double scalar = 2.5;
        matrix ScalarMult1 = A * scalar;
        std::cout << "A * " << scalar << ":\n" << ScalarMult1 << std::endl;

        matrix ScalarMult2 = scalar * A;
        std::cout << scalar << " * A:\n" << ScalarMult2 << std::endl;

        matrix MatrixMult = A * B;
        std::cout << "A * B:\n" << MatrixMult << std::endl;

        matrix TransposedA = A.transposed();
        std::cout << "A Transposed:\n" << TransposedA << std::endl;

        double detC = C.determinant();
        std::cout << "Determinant of C: " << detC << std::endl;

        matrix InverseC = C.inverse_matrix();
        std::cout << "Inverse of C:\n" << InverseC << std::endl;

        try {
            matrix InvalidSum = A + D;
        }
        catch (const matrix::InvalidDimensions& e) {
            std::cerr << "Exception caught: " << e.what() << std::endl;
        }

        try {
            matrix NonsquareInverse = D.inverse_matrix();
        }
        catch (const matrix::NonSquareMatrix& e) {
            std::cerr << "Exception caught: " << e.what() << std::endl;
        }

        matrix Singular(2, 2);
        Singular[0][0] = 1.0;
        Singular[0][1] = 1.0;
        Singular[1][0] = 1.0;
        Singular[1][1] = 1.0;

        try {
            matrix InverseSingular = Singular.inverse_matrix();
        }
        catch (const matrix::SingularMatrix& e) {
            std::cerr << "Exception caught: " << e.what() << std::endl;
        }

        try {
            matrix Huge(10000, 10000);
        }
        catch (const matrix::MatrixException& e) {
            std::cerr << "Exception caught: " << e.what() << std::endl;
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Unexpected exception: " << e.what() << std::endl;
    }
    return 0;
}
