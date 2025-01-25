#include "matrix.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <immintrin.h>
#include <x86intrin.h>
#endif

/* Below are some intel intrinsics that might be useful
 * void _mm256_storeu_pd (double * mem_addr, __m256d a)
 * __m256d _mm256_set1_pd (double a)
 * __m256d _mm256_set_pd (double e3, double e2, double e1, double e0)
 * __m256d _mm256_loadu_pd (double const * mem_addr)
 * __m256d _mm256_add_pd (__m256d a, __m256d b)
 * __m256d _mm256_sub_pd (__m256d a, __m256d b)
 * __m256d _mm256_fmadd_pd (__m256d a, __m256d b, __m256d c)
 * __m256d _mm256_mul_pd (__m256d a, __m256d b)
 * __m256d _mm256_cmp_pd (__m256d a, __m256d b, const int imm8)
 * __m256d _mm256_and_pd (__m256d a, __m256d b)
 * __m256d _mm256_max_pd (__m256d a, __m256d b)
*/

/*
 * Generates a random double between `low` and `high`.
 */
double rand_double(double low, double high) {
    double range = (high - low);
    double div = RAND_MAX / range;
    return low + (rand() / div);
}

/*
 * Generates a random matrix with `seed`.
 */
void rand_matrix(matrix *result, unsigned int seed, double low, double high) {
    srand(seed);
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            set(result, i, j, rand_double(low, high));
        }
    }
}

/*
 * Allocate space for a matrix struct pointed to by the double pointer mat with
 * `rows` rows and `cols` columns. You should also allocate memory for the data array
 * and initialize all entries to be zeros. Remember to set all fieds of the matrix struct.
 * `parent` should be set to NULL to indicate that this matrix is not a slice.
 * You should return -1 if either `rows` or `cols` or both have invalid values, or if any
 * call to allocate memory in this function fails. If you don't set python error messages here upon
 * failure, then remember to set it in numc.c.
 * Return 0 upon success and non-zero upon failure.
 */
int allocate_matrix(matrix **mat, int rows, int cols) {
    /* TODO: YOUR CODE HERE */
    if (rows <= 0 || cols <= 0)
        return -1;
    *mat = malloc(sizeof(matrix));
    matrix* matrix_ptr = *mat;
    if (!matrix_ptr)
        return -1;
    matrix_ptr->rows = rows;
    matrix_ptr->cols = cols;
    matrix_ptr->data = malloc(sizeof(matrix*) * rows);
    if (matrix_ptr->data == NULL)
        return -1;
    for (int i = 0; i != rows; ++i)
    {
        matrix_ptr->data[i] = malloc(sizeof(double) * cols);
        if (matrix_ptr->data[i] == NULL)
            return -1;
    }
    if (rows == 1 || cols == 1)
        matrix_ptr->is_1d = 1;
    else
        matrix_ptr->is_1d = 0;
    matrix_ptr->ref_cnt = 1;
    matrix_ptr->parent = NULL;
    fill_matrix(matrix_ptr, 0);
    return 0;
}

/*
 * Allocate space for a matrix struct pointed to by `mat` with `rows` rows and `cols` columns.
 * This is equivalent to setting the new matrix to be
 * from[row_offset:row_offset + rows, col_offset:col_offset + cols]
 * If you don't set python error messages here upon failure, then remember to set it in numc.c.
 * Return 0 upon success and non-zero upon failure.
 */
int allocate_matrix_ref(matrix **mat, matrix *from, int row_offset, int col_offset,
                        int rows, int cols) {
    /* TODO: YOUR CODE HERE */
    if (rows <= 0 || cols <= 0)
        return -1;
    if ((row_offset + rows > from->rows) || (col_offset + cols > from->cols))
        return -1;
    *mat = malloc(sizeof(matrix));
    matrix* matrix_ptr = *mat;
    if (matrix_ptr == NULL)
        return -1;
    matrix_ptr->data = malloc(sizeof(matrix*) * rows);
    if (matrix_ptr->data == NULL)
        return -1;
    for (int i = 0; i != rows; ++i)
    {
        matrix_ptr->data[i] = from->data[i + row_offset] + col_offset;
    }
    matrix_ptr->rows = rows;
    matrix_ptr->cols = cols;
    if (rows == 1 || cols == 1)
        matrix_ptr->is_1d = 1;
    else
        matrix_ptr->is_1d = 0;
    matrix_ptr->ref_cnt = 0;
    matrix_ptr->parent = from;
    ++(from->ref_cnt);
    return 0;
}

/*
 * This function will be called automatically by Python when a numc matrix loses all of its
 * reference pointers.
 * You need to make sure that you only free `mat->data` if no other existing matrices are also
 * referring this data array.
 * See the spec for more information.
 */
void deallocate_matrix(matrix *mat) {
    /* TODO: YOUR CODE HERE */
    if (mat == NULL)
        return;
    --(mat->ref_cnt);
    // not top matrix, have no child
    if (mat->parent && mat->ref_cnt == 0)
    {
        deallocate_matrix(mat->parent);
        free(mat->data);
        free(mat);
    }
    // top matrix and have no child
    else if (mat->ref_cnt == 0)
    {
        for (int i = 0; i != mat->rows; ++i)
            free(mat->data[i]);
        free(mat->data);
        free(mat);
    }
}

/*
 * Return the double value of the matrix at the given row and column.
 * You may assume `row` and `col` are valid.
 */
double get(matrix *mat, int row, int col) {
    /* TODO: YOUR CODE HERE */
    return mat->data[row][col];
}

/*
 * Set the value at the given row and column to val. You may assume `row` and
 * `col` are valid
 */
void set(matrix *mat, int row, int col, double val) {
    /* TODO: YOUR CODE HERE */
    mat->data[row][col] = val;
}

/*
 * Set all entries in mat to val
 */
void fill_matrix(matrix *mat, double val) {
    /* TODO: YOUR CODE HERE */
    for (int row = 0; row != mat->rows; ++row)
    {
        for (int col = 0; col != mat->cols; ++col)
        {
            mat->data[row][col] = val;
        }
    }
}

/*
 * Store the result of adding mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int add_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */
    if (result->rows != mat1->rows || mat1->rows != mat2->rows
        || result->cols != mat1->cols || mat1->cols != mat2->cols)
    {
        return -1;
    }
    for (int row = 0; row != result->rows; ++row)
    {
        for (int col = 0; col != result->cols; ++col)
        {
            result->data[row][col] = mat1->data[row][col] + mat2->data[row][col];
        }
    }
    return 0;
}

/*
 * Store the result of subtracting mat2 from mat1 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int sub_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */
    if (result->rows != mat1->rows || mat1->rows != mat2->rows
        || result->cols != mat1->cols || mat1->cols != mat2->cols)
    {
        return -1;
    }
    for (int row = 0; row != result->rows; ++row)
    {
        for (int col = 0; col != result->cols; ++col)
        {
            result->data[row][col] = mat1->data[row][col] - mat2->data[row][col];
        }
    }
    return 0;
}

/*
 * Store the result of multiplying mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that matrix multiplication is not the same as multiplying individual elements.
 */
int mul_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */
    if (mat1->cols != mat2->rows || result->rows != mat1->rows || result->cols != mat2->cols)
        return -1;
    for (int row = 0; row != result->rows; ++row)
    {
        for (int col = 0; col != result->cols; ++col)
        {
            int temp = 0;
            for (int i = 0; i != mat1->cols; ++i)
            {
                temp += mat1->data[row][i] * mat2->data[i][col];
            }
            result->data[row][col] = temp;
        }
    }
    return 0;
}

/*
 * Store the result of raising mat to the (pow)th power to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that pow is defined with matrix multiplication, not element-wise multiplication.
 */
int pow_matrix(matrix *result, matrix *mat, int pow) {
    /* TODO: YOUR CODE HERE */
    if (mat->rows != mat->cols || pow < 0)
        return -1;
    matrix* temp = malloc(sizeof(matrix));
    allocate_matrix(&temp, result->rows, result->cols);
    for (int i = 0; i != result->rows; ++i)
        set(temp, i, i, 1);
    for (int i = 0; i != pow; ++i)
    {
        mul_matrix(result, temp, mat);
        for (int row = 0; row != result->rows; ++row)
        {
            for (int col = 0; col != result->cols; ++col)
            {
                temp->data[row][col] = result->data[row][col];
            }
        }
    }
    deallocate_matrix(temp);
    return 0;
}

/*
 * Store the result of element-wise negating mat's entries to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int neg_matrix(matrix *result, matrix *mat) {
    /* TODO: YOUR CODE HERE */
    if (result->rows != mat->rows || result->cols != mat->cols)
        return -1;
    for (int row = 0; row != mat->rows; ++row)
    {
        for (int col = 0; col != mat->cols; ++col)
            result->data[row][col] = -(mat->data[row][col]);
    }
    return 0;
}

/*
 * Store the result of taking the absolute value element-wise to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int abs_matrix(matrix *result, matrix *mat) {
    /* TODO: YOUR CODE HERE */
    if (result->rows != mat->rows || result->cols != mat->cols)
        return -1;
    for (int row = 0; row != mat->rows; ++row)
    {
        for (int col = 0; col != mat->cols; ++col)
        {
            result->data[row][col] = abs(mat->data[row][col]);
        }
    }
    return 0;
}
