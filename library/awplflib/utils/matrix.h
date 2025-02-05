#pragma once

#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <cstring> // For memcpy

class TMatrix {
private:
    size_t rows_ = 0;
    size_t cols_ = 0;
    std::vector<float> data_;

public:
    // Constructor
    TMatrix(size_t rows, size_t cols)
        : rows_(rows), cols_(cols), data_(rows* cols, 0.0f) {}

    TMatrix(size_t rows, size_t cols, const std::vector<float> &data)
        : rows_(rows), cols_(cols), data_(data) {}

    TMatrix(TMatrix&& other)
        : rows_(other.rows_), cols_(other.cols_), data_(std::move(other.data_)) {
        other.rows_ = 0;
        other.cols_ = 0;
    }


    TMatrix& operator=(TMatrix&& other) {
        data_ = std::move(other.data_);
        rows_ = other.rows_;
        cols_ = other.cols_;
        other.rows_ = 0;
        other.cols_ = 0;
        return *this;
    }

    TMatrix() = default;

    // Access element
    float& operator()(size_t row, size_t col) {
        if (row >= rows_ || col >= cols_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[row * cols_ + col];
    }

    float operator()(size_t row, size_t col) const {
        if (row >= rows_ || col >= cols_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[row * cols_ + col];
    }

    // Get number of rows
    size_t rows() const { return rows_; }

    // Get number of columns
    size_t cols() const { return cols_; }

    // Resize matrix
    void Resize(size_t new_rows, size_t new_cols) {

        if (rows_ != 0 && cols_ != 0) {
            std::vector<float> new_data(new_rows * new_cols, 0.0f);

            size_t min_rows = std::min<size_t>(rows_, new_rows);
            size_t min_cols = std::min<size_t>(cols_, new_cols);

            for (size_t row = 0; row < min_rows; ++row) {
                std::memcpy(&new_data[row * new_cols], &data_[row * cols_], min_cols * sizeof(float));
            }

         
            data_ = std::move(new_data);
        }
        else {
            data_.resize(new_rows * new_cols, 0.0f);
        }

        rows_ = new_rows;
        cols_ = new_cols;
    }

    void    Clear() {
        rows_ = cols_ = 0;
    }


    void Reserve(size_t new_rows, size_t new_cols) {
        data_.resize(0);

        data_.reserve(new_rows * new_cols);
        rows_ = 0;
        cols_ = new_cols;
    }

    // Copy a row from another matrix
    void CopyRow(size_t target_row, const TMatrix& source, size_t source_row, size_t num_cols) {
        if (target_row >= rows_ || source_row >= source.rows()) {
            throw std::out_of_range("Row index out of range");
        }

        size_t copy_cols = std::min<size_t>({ num_cols, cols_, source.cols() });
        std::memcpy(&data_[target_row * cols_], &source.data_[source_row * source.cols_], copy_cols * sizeof(float));

        // If the current matrix is wider, fill the remaining columns with zeros
        if (copy_cols < cols_) {
            std::fill(&data_[target_row * cols_ + copy_cols], &data_[target_row * cols_ + cols_], 0.0f);
        }
    }


    template<typename F>
    void	CopyRowsIf(F&& binary, const TMatrix& other) {
        size_t target_row = 0;
        for (size_t row = 0; row < other.rows(); ++row) {
            if (binary(row)) target_row++;
        }

        Resize(target_row, other.cols());
        
        target_row = 0;
        for (size_t row = 0; row < other.rows(); ++row) {
            if (binary(row)) {
                std::memcpy(GetRow(target_row), other.GetRow(row), other.cols() * sizeof(float));
                target_row++;
            }
        }
    }

    // Get a row by index
    float* GetRow(size_t row_index) {
        if (row_index >= rows_) {
            throw std::out_of_range("Row index out of range");
        }
        return &data_[row_index * cols_];
    }
        
    const float* GetRow(size_t row_index) const {
        if (row_index >= rows_) {
            throw std::out_of_range("Row index out of range");
        }
        return &data_[row_index * cols_];
    }

    // Add a new row to the matrix
    void AddRow(const float* new_row, size_t size) {

        size = std::min<size_t>(size, cols_);
        
        data_.insert(data_.end(), new_row, new_row + size);

        // Fill remaining columns with zeros if the new row is smaller than cols_
        if (size < cols_) {
            data_.insert(data_.end(), cols_ - size, 0.0f);
        }

        ++rows_;
    }


    // Friend class for TMatrixView
    friend class TMatrixView;
};

class TMatrixView {
private:
    const TMatrix& matrix_;
    size_t row_start_;
    size_t col_start_;
    size_t view_rows_;
    size_t view_cols_;

public:
    // Constructor
    TMatrixView(const TMatrix& matrix, size_t row_start, size_t col_start, size_t view_rows, size_t view_cols)
        : matrix_(matrix), row_start_(row_start), col_start_(col_start), view_rows_(view_rows), view_cols_(view_cols) {
        if (row_start + view_rows > matrix.rows() || col_start + view_cols > matrix.cols()) {
            throw std::out_of_range("View dimensions exceed matrix bounds");
        }
    }

    // Access element in the view
    float operator()(size_t row, size_t col) const {
        if (row >= view_rows_ || col >= view_cols_) {
            throw std::out_of_range("Index out of range in view");
        }
        return matrix_(row_start_ + row, col_start_ + col);
    }

    // Get number of rows in the view
    size_t rows() const { return view_rows_; }

    // Get number of columns in the view
    size_t cols() const { return view_cols_; }

};

