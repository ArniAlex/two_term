#include <iostream>
#include <stdexcept>
#include <cmath>    //std::abs
#include <algorithm> //std::swap
#include <new>      // Для std::bad_alloc

class matrix {
    double** m;
    size_t rows;
    size_t columns;

public:
    matrix(size_t r, size_t c) : rows(r), columns(c) {
        m = new double* [rows]; //исключения
        for (size_t i = 0; i < rows; ++i) {
            m[i] = new double[columns];
            for (size_t j = 0; j < columns; ++j) {
                m[i][j] = 0.0;
            }
        }
    };

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

        m = new double* [rows];
        for (size_t i = 0; i < rows; ++i) {
            m[i] = new double[columns];
            for (size_t j = 0; j < columns; ++j) {
                m[i][j] = other.m[i][j];
            }
        }

        return *this;
    };

    matrix operator+(const matrix& second) const {
        if (!(rows == second.rows && columns == second.columns)) {
            throw std::invalid_argument("Matrices must have the same dimensions for addition.");
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
            throw std::invalid_argument("Determinant can only be calculated for square matrices.");
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
            throw std::invalid_argument("Matrix must be square to calculate the inverse matrix.");
        }
        double det = determinant();
        if (isEqual(det, 0.0)) {
            throw std::domain_error("The matrix has no inverse because the determinant is zero.");
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

    size_t getRows() const { return rows; };
    size_t getColumns() const { return columns; };

    ~matrix() {
        for (size_t i = 0; i < rows; ++i) {
            delete[] m[i];
        }
        delete[] m;
    };

private:
    //метод Гаусса (приведение к верхнетреугольному виду)
    std::pair<double, size_t> gaussianElimination(matrix& a) const {
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

            if (a.m[i][i] == 0.0) {
                return std::make_pair(0.0, sign);
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

        return std::make_pair(det, sign);
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
        A[0][0] = 1.0; A[0][1] = 2.0; A[0][2] = 8.0;
        A[1][0] = 3.0; A[1][1] = 4.0; A[0][2] = 67.0;
        A[2][0] = 8.0; A[2][1] = 12.0; A[2][2] = 3.0;
       

        matrix B(3, 3);
        B[0][0] = 5.0; B[0][1] = 6.0; B[0][2] = 4.0;
        B[1][0] = 7.0; B[1][1] = 8.0; B[1][2] = 12.0;
        B[2][0] = 74.0; B[2][1] = 7.0; B[2][2] = 5.0;
        

        std::cout << "Matrix A:\n";
        for (size_t i = 0; i < A.getRows(); ++i) {
            for (size_t j = 0; j < A.getColumns(); ++j) {
                std::cout << A[i][j] << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "Matrix B:\n";
        for (size_t i = 0; i < B.getRows(); ++i) {
            for (size_t j = 0; j < B.getColumns(); ++j) {
                std::cout << B[i][j] << " ";
            }
            std::cout << std::endl;
        }

        //операции
        matrix C = A + B;
        std::cout << "A + B:\n";
        for (size_t i = 0; i < C.getRows(); ++i) {
            for (size_t j = 0; j < C.getColumns(); ++j) {
                std::cout << C[i][j] << " ";
            }
            std::cout << std::endl;
        }

        matrix D = A * 2.0;
        std::cout << "A * 2.0:\n";
        for (size_t i = 0; i < D.getRows(); ++i) {
            for (size_t j = 0; j < D.getColumns(); ++j) {
                std::cout << D[i][j] << " ";
            }
            std::cout << std::endl;
        }

        matrix E = A * B;
        std::cout << "A * B:\n";
        for (size_t i = 0; i < E.getRows(); ++i) {
            for (size_t j = 0; j < E.getColumns(); ++j) {
                std::cout << E[i][j] << " ";
            }
            std::cout << std::endl;
        }

        matrix F = A - B;
        std::cout << "A - B:\n";
        for (size_t i = 0; i < F.getRows(); ++i) {
            for (size_t j = 0; j < F.getColumns(); ++j) {
                std::cout << F[i][j] << " ";
            }
            std::cout << std::endl;
        }

        matrix G = A.transposed();
        std::cout << "A Transposed:\n";
        for (size_t i = 0; i < G.getRows(); ++i) {
            for (size_t j = 0; j < G.getColumns(); ++j) {
                std::cout << G[i][j] << " ";
            }
            std::cout << std::endl;
        }

        //определитель и обратная матрица
        matrix H(3, 3);
        H[0][0] = 1.0; H[0][1] = 2.0; H[0][2] = 1.0;
        H[1][0] = 0.0; H[1][1] = 1.0; H[1][2] = 0.0;
        H[2][0] = 2.0; H[2][1] = 3.0; H[2][2] = 4.0;

        double det = H.determinant();
        std::cout << "Determinant H: " << det << std::endl;

        matrix I = H.inverse_matrix();
        std::cout << "Inverse matrix H:\n";
        for (size_t i = 0; i < I.getRows(); ++i) {
            for (size_t j = 0; j < I.getColumns(); ++j) {
                std::cout << I[i][j] << " ";
            }
            std::cout << std::endl;
        }

        //исключения
        try {
            matrix invalid_matrix(2, 3);
            matrix result = A + invalid_matrix; //матрицы разных размеров
        }
        catch (const std::invalid_argument& error) {
            std::cerr << "Error: " << error.what() << std::endl;
        }
        catch (const std::bad_alloc& error) {
            std::cerr << "Error bad_alloc: " << error.what() << std::endl;
        }

        try {
            matrix nonsquare_matrix(2, 3);
            double det = nonsquare_matrix.determinant(); //определитель неквадратной матрицы
        }
        catch (const std::invalid_argument& error) {
            std::cerr << "Error: " << error.what() << std::endl;
        }
        catch (const std::bad_alloc& error) {
            std::cerr << "Error bad_alloc: " << error.what() << std::endl;
        }


        //очень большая матрица, чтобы вызвать bad_alloc
        try {
            matrix huge_matrix(10000, 10000);
        }
        catch (const std::bad_alloc& error) {
            std::cerr << "Error bad_alloc: " << error.what() << std::endl;
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
} 
