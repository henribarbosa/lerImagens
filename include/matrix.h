#pragma once

#include <vector>

class matrix
{
public:
    int lins, cols; // number of lines and columns
    std::vector<float> mat; // vector of data organized per lines
    matrix(const int lins, const int cols); // constructor from lines and columns
    matrix(const matrix& other); // copy constructor
    ~matrix(); //destructor
    float get(int lin, int col); // access element
    float get(int lin, int col) const;
    void set(int lin, int col, float val); // set element from position
    matrix mult(matrix& other); // matrix multiplication
    matrix mult(const matrix& other);
    matrix mult(float x); // scalar multiplication
    matrix operator*(matrix& other); // operator override
    matrix operator*(const matrix& other);
    matrix operator*(float x);
    matrix operator+(matrix& other);
    bool equal(matrix& other); // compare matrix
    bool operator==(matrix& other);
    matrix T(); // transpose
    void print(); // print to console
    matrix add(matrix& other); // matrix sum
    matrix join(matrix& other); // do not use
    matrix split(const int indice); // do not use
    matrix slice(const int lin_i, const int lin_f, const int col_i, const int col_f); // get submatrix from index
    void append(matrix join); // append an extra matrix below the current one
    matrix pop(int axis, int index); // exclude line or row from index
};

matrix thomas(matrix& A, matrix& v); //solve linear system for tridiagonal matrix
matrix thomas_p(matrix& A, matrix& v); // solve linear system for periodic tridiagonal matrix
matrix eye(const int n); // identity matrix
void min(matrix mat, int axis, int index, int& min_place, double& min_value); // find minimum value in a row or column
void max(matrix mat, int axis, int index, int& max_place, double& max_value); // find maximum value in a row or column
