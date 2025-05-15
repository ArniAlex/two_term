#include <iostream>
#include <stdexcept>
#include <cmath>
#include <utility>  // std::move

class Matrix {
public:
    class MatrixException : public std::exception {
    private:
        std::string message;
    public:
        explicit MatrixException(const std::string& msg) : message(msg) {}
        const char* what() const noexcept {
            return message.c_str();
        }
    };

private:
    size_t rows, cols;
    double** data;

    void allocateMemory() {
        data = new double* [rows];
        for (size_t i = 0; i < rows; ++i) {
            data[i] = new double[cols] {0};
        }
    }

    void freeMemory() {
        if (data) {
            for (size_t i = 0; i < rows; ++i) delete[] data[i];
            delete[] data;
        }
        data = nullptr;
    }

    bool isEqual(double a, double b, double eps = 1e-6) const {
        return std::fabs(a - b) < eps;
    }

    void copyData(const Matrix& other) {
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                data[i][j] = other.data[i][j];
    }

public:
    // Конструктор
    Matrix(size_t r, size_t c) : rows(r), cols(c), data(nullptr) {
        allocateMemory();
    }

    // Деструктор
    ~Matrix() {
        freeMemory();
    }

    // Конструктор копирования
    Matrix(const Matrix& other) : rows(other.rows), cols(other.cols), data(nullptr) {
        allocateMemory();
        copyData(other);
    }

    // Оператор присваивания
    Matrix& operator=(const Matrix& other) {
        if (this != &other) {
            freeMemory();
            rows = other.rows;
            cols = other.cols;
            allocateMemory();
            copyData(other);
        }
        return *this;
    }



    // Доступ к элементу с проверкой границ
    double& at(size_t i, size_t j) {
        if (i >= rows || j >= cols)
            throw MatrixException("Index out of bounds");
        return data[i][j];
    }

    // Перегрузка ()
    double& operator()(size_t i, size_t j) { return at(i, j); }

    // Перегрузка []
    class RowProxy {
    private:
        double* row;
        size_t cols;
    public:
        RowProxy(double* row_ptr, size_t c) : row(row_ptr), cols(c) {}
        double& operator[](size_t j) {
            if (j >= cols) throw std::out_of_range("Column index out of bounds");
            return row[j];
        }
    };

    RowProxy operator[](size_t i) {
        if (i >= rows) throw std::out_of_range("Row index out of bounds");
        return RowProxy(data[i], cols);
    }

    // Оператор вывода
    friend std::ostream& operator<<(std::ostream& os, const Matrix& m) {
        for (size_t i = 0; i < m.rows; ++i) {
            for (size_t j = 0; j < m.cols; ++j)
                os << m.data[i][j] << " ";
            os << '\n';
        }
        return os;
    }



    // Операции над матрицами
    Matrix operator+(Matrix& other) const {
        if (rows != other.rows || cols != other.cols)
            throw MatrixException("Dimension mismatch for addition");
        Matrix result(rows, cols);
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                result(i, j) = data[i][j] + other(i, j);
        return result;
    }

    Matrix operator-(Matrix& other) const {
        if (rows != other.rows || cols != other.cols)
            throw MatrixException("Dimension mismatch for subtraction");
        Matrix result(rows, cols);
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                result(i, j) = data[i][j] - other(i, j);
        return result;
    }

    Matrix operator*(Matrix& other) const {
        if (cols != other.rows) 
            throw MatrixException("Dimension mismatch for multiplication");
        Matrix result(rows, other.cols); // кол-во столбцов первой == кол-во строк второй
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < other.cols; ++j)
                for (size_t k = 0; k < cols; ++k)
                    result(i, j) += data[i][k] * other(k, j);
        return result;
    }

    Matrix operator*(double scalar) const {
        Matrix result(rows, cols);
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                result(i, j) = data[i][j] * scalar;
        return result;
    }

    // Умножение числа на матрицу
    friend Matrix operator*(double scalar, const Matrix& m) {
        return m * scalar;
    }

    // Транспонирование
    Matrix transposed() const {
        Matrix result(cols, rows);
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                result(j, i) = data[i][j];
        return result;
    }

    // Определитель методом Гаусса
    double determinant(double epsilon = 1e-6) const {
        if (rows != cols) // только для квадратных 
            throw MatrixException("Determinant only for square matrices");

        Matrix temp(*this);
        double det = 1.0;

        for (size_t i = 0; i < rows; ++i) {
            size_t cur = i; // ненулевой ведущий эл
            while (cur < rows && isEqual(temp(cur, i), 0.0, epsilon)) ++cur;

            if (cur == rows) return 0.0;

            if (cur != i) {
                std::swap(temp.data[i], temp.data[cur]); 
                det = -det;
            }

            det *= temp(i, i); // обнуляем элементы ниже текущего
            for (size_t j = i + 1; j < rows; ++j) {
                double factor = temp(j, i) / temp(i, i);
                for (size_t k = i; k < cols; ++k)
                    temp(j, k) -= factor * temp(i, k);
            }
        }

        return det;
    }

    // Обратная матрица
    Matrix inverse_matrix(double epsilon = 1e-6) const {
        if (rows != cols)
            throw MatrixException("Only square matrix can be inverted");

        size_t n = rows;
        Matrix result(n, n);
        Matrix temp(*this);

        // Единичная матрица
        for (size_t i = 0; i < n; ++i)
            result(i, i) = 1.0;

        for (size_t i = 0; i < n; ++i) {
            double cur = temp(i, i); 
            if (isEqual(cur, 0.0, epsilon)) 
                throw MatrixException("Matrix is singular");

            for (size_t j = 0; j < n; ++j) { // нормализация строки...
                temp(i, j) /= cur;
                result(i, j) /= cur;
            }

            for (size_t k = 0; k < n; ++k) {
                if (k != i) {
                    double factor = temp(k, i);
                    for (size_t j = 0; j < n; ++j) {
                        temp(k, j) -= factor * temp(i, j);
                        result(k, j) -= factor * result(i, j);
                    }
                }
            }
        }

        return result;
    }
};



int main() {
    try {
        Matrix A(3, 3);
        Matrix B(3, 3);
        Matrix C(2, 2);
        Matrix D(2, 3);

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

        Matrix sum = A + B;
        std::cout << "A + B:\n" << sum << std::endl;

        Matrix diff = A - B;
        std::cout << "A - B:\n" << diff << std::endl;

        double scalar = 2.5;
        Matrix scalarMult1 = A * scalar;
        std::cout << "A * " << scalar << ":\n" << scalarMult1 << std::endl;

        Matrix scalarMult2 = scalar * A;
        std::cout << scalar << " * A:\n" << scalarMult2 << std::endl;

        Matrix matrixMult = A * B;
        std::cout << "A * B:\n" << matrixMult << std::endl;

        Matrix transposedA = A.transposed();
        std::cout << "A Transposed:\n" << transposedA << std::endl;

        double detC = C.determinant();
        std::cout << "Determinant of C: " << detC << std::endl;

        Matrix inverseC = C.inverse_matrix();
        std::cout << "Inverse of C:\n" << inverseC << std::endl;

        // матрицы разных размеров
        try {
            Matrix invalidSum = A + D;
        }
        catch (const Matrix::MatrixException& e)
        {
            std::cerr << "Exception caught (InvalidDimensions): " << e.what() << std::endl;
        }

        // не квадратной матрицы
        try {
            Matrix nonsquareInverse = D.inverse_matrix();
        }
        catch (const Matrix::MatrixException& e)
        {
            std::cerr << "Exception caught (NonSquareMatrix): " << e.what() << std::endl;
        }

        // определитель == 0
        Matrix singular(2, 2);
        singular[0][0] = 1.0; singular[0][1] = 1.0;
        singular[1][0] = 1.0; singular[1][1] = 1.0;

        try {
            Matrix invSingular = singular.inverse_matrix();
        }
        catch (const Matrix::MatrixException& e)
        {
            std::cerr << "Exception caught (SingularMatrix): " << e.what() << std::endl;
        }

        // попытка выделения большой матрицы
        try {
            Matrix huge(10000, 10000);
        }
        catch (const std::bad_alloc& e) {
            std::cerr << "Memory allocation failed: " << e.what() << std::endl;
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Unexpected exception: " << e.what() << std::endl;
    }

    return 0;
}

