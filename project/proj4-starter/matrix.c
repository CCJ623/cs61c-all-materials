#include "matrix.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

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
    if (mat == NULL)
        return -1;
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
    if (mat == NULL || from == NULL)
        return -1;
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
inline double get(matrix* mat, int row, int col)
{
    /* TODO: YOUR CODE HERE */
    return mat->data[row][col];
}

/*
 * Set the value at the given row and column to val. You may assume `row` and
 * `col` are valid
 */
inline void set(matrix* mat, int row, int col, double val)
{
    /* TODO: YOUR CODE HERE */
    mat->data[row][col] = val;
}

/*
 * Set all entries in mat to val
 */
void fill_matrix(matrix *mat, double val) {
    /* TODO: YOUR CODE HERE */
    if (mat == NULL)
        return;
    omp_set_num_threads(omp_get_num_procs());
    int num_threads = omp_get_num_threads();
    // enable multi-threading if the matrix is large enough
    if (mat->rows > num_threads * 2 || mat->cols > 50)
    {
        int chunk_size = ceil((double)mat->rows / (double)num_threads);
        int start = omp_get_thread_num() * chunk_size,
            end = fmin(start + chunk_size, mat->rows);
        if (mat->cols >= 4)
        {
            __m256d val_vector = _mm256_set1_pd(val);
# pragma omp parallel
            {
                for (; start < end; ++start)
                {
                    int col = 0;
                    for (; col + 4 <= mat->cols; col += 4)
                    {
                        _mm256_storeu_pd(mat->data[start] + col, val_vector);
                    }
                    for (; col != mat->cols; ++col)
                    {
                        mat->data[start][col] = val;
                    }
                }
            }
        }
        else
        {
# pragma omp parallel
            {
                for (;start < end; ++start)
                {
                    for (int col = 0; col != mat->cols; ++col)
                    {
                        mat->data[start][col] = val;
                    }
                }
            }
        }
    }
    else
    {
        if (mat->cols >= 4)
        {
            __m256d val_vector = _mm256_set1_pd(val);
            for (int row = 0; row != mat->rows; ++row)
            {
                int col = 0;
                for (; col + 4 <= mat->cols; col += 4)
                {
                    _mm256_storeu_pd(mat->data[row] + col, val_vector);
                }
                for (; col != mat->cols; ++col)
                {
                    mat->data[row][col] = val;
                }
            }
        }
        else
        {
            for (int row = 0; row != mat->rows; ++row)
            {
                for (int col = 0; col != mat->cols; ++col)
                {
                    mat->data[row][col] = val;
                }
            }
        }
    }
}

/*
 * Store the result of adding mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int add_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */
    if (result == NULL || mat1 == NULL || mat2 == NULL)
    {
        return -1;
    }
    if (result->rows != mat1->rows || mat1->rows != mat2->rows
        || result->cols != mat1->cols || mat1->cols != mat2->cols)
    {
        return -1;
    }
    omp_set_num_threads(omp_get_num_procs());
    int num_threads = omp_get_num_threads();
    // enable multi-threading if the matrix is large enough
    if (result->rows > num_threads * 2 || result->cols > 50)
    {
        int chunk_size = ceil((double)result->rows / (double)num_threads);
        int start = omp_get_thread_num() * chunk_size,
            end = fmin(start + chunk_size, result->rows);
        if (result->cols >= 4)
        {
# pragma omp parallel
            {
                for (; start < end; ++start)
                {
                    int col = 0;
                    for (; col + 4 <= result->cols; col += 4)
                    {
                        __m256d vec1 = _mm256_loadu_pd(mat1->data[start] + col),
                            vec2 = _mm256_loadu_pd(mat2->data[start] + col);
                        _mm256_storeu_pd(result->data[start] + col, _mm256_add_pd(vec1, vec2));
                    }
                    for (; col != result->cols; ++col)
                    {
                        result->data[start][col] = mat1->data[start][col] + mat2->data[start][col];
                    }
                }
            }
        }
        else
        {
# pragma omp parallel
            {
                for (;start < end; ++start)
                {
                    for (int col = 0; col != result->cols; ++col)
                    {
                        result->data[start][col] = mat1->data[start][col] + mat2->data[start][col];
                    }
                }
            }
        }
    }
    else
    {
        if (result->cols >= 4)
        {
            for (int row = 0; row != result->rows; ++row)
            {
                int col = 0;
                for (; col + 4 <= result->cols; col += 4)
                {
                    __m256d vec1 = _mm256_loadu_pd(mat1->data[row] + col),
                        vec2 = _mm256_loadu_pd(mat2->data[row] + col);
                    _mm256_storeu_pd(result->data[row] + col, _mm256_add_pd(vec1, vec2));
                }
                for (; col != result->cols; ++col)
                {
                    result->data[row][col] = mat1->data[row][col] + mat2->data[row][col];
                }
            }
        }
        else
        {
            for (int row = 0; row != result->rows; ++row)
            {
                for (int col = 0; col != result->cols; ++col)
                {
                    result->data[row][col] = mat1->data[row][col] + mat2->data[row][col];
                }
            }
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
    omp_set_num_threads(omp_get_num_procs());
    int num_threads = omp_get_num_threads();
    // enable multi-threading if the matrix is large enough
    if (result->rows > num_threads * 2 || result->cols > 50)
    {
# pragma omp parallel
        {
            int chunk_size = ceil((double)result->rows / (double)num_threads);
            int start = omp_get_thread_num() * chunk_size,
                end = fmin(start + chunk_size, result->rows);
            if (result->cols >= 4)
            {
                for (; start < end; ++start)
                {
                    int col = 0;
                    for (; col + 4 <= result->cols; col += 4)
                    {
                        __m256d vec1 = _mm256_loadu_pd(mat1->data[start] + col),
                            vec2 = _mm256_loadu_pd(mat2->data[start] + col);
                        _mm256_storeu_pd(result->data[start] + col, _mm256_sub_pd(vec1, vec2));
                    }
                    for (; col != result->cols; ++col)
                    {
                        result->data[start][col] = mat1->data[start][col] - mat2->data[start][col];
                    }
                }
            }
            else
            {
                for (;start < end; ++start)
                {
                    for (int col = 0; col != result->cols; ++col)
                    {
                        result->data[start][col] = mat1->data[start][col] - mat2->data[start][col];
                    }
                }
            }
        }
    }
    else
    {
        if (result->cols >= 4)
        {
            for (int row = 0; row != result->rows; ++row)
            {
                int col = 0;
                for (; col + 4 <= result->cols; col += 4)
                {
                    __m256d vec1 = _mm256_loadu_pd(mat1->data[row] + col),
                        vec2 = _mm256_loadu_pd(mat2->data[row] + col);
                    _mm256_storeu_pd(result->data[row] + col, _mm256_sub_pd(vec1, vec2));
                }
                for (; col != result->cols; ++col)
                {
                    result->data[row][col] = mat1->data[row][col] - mat2->data[row][col];
                }
            }
        }
        else
        {
            for (int row = 0; row != result->rows; ++row)
            {
                for (int col = 0; col != result->cols; ++col)
                {
                    result->data[row][col] = mat1->data[row][col] - mat2->data[row][col];
                }
            }
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
    fill_matrix(result, 0);
    omp_set_num_threads(omp_get_num_procs());
    int num_threads = omp_get_num_threads();
    // enable multi-threading if the matrix is large enough
    if (result->rows > num_threads * 2 || mat1->cols > 50)
    {
#pragma omp parallel 
        {
            int chunk_size = ceil((double)mat1->rows / (double)num_threads);
            int start = omp_get_thread_num() * chunk_size,
                end = fmin(start + chunk_size, mat1->rows);
            for (; start < end; ++start)
            {
                for (int col = 0; col != mat1->cols; ++col)
                {
                    double value = mat1->data[start][col];
                    __m256d vec1 = _mm256_set1_pd(value);
                    int i = 0;
                    for (; i + 4 <= mat2->cols; i += 4)
                    {
                        __m256d vec2 = _mm256_loadu_pd(mat2->data[col] + i);
                        __m256d result_vec = _mm256_fmadd_pd(vec1, vec2, _mm256_loadu_pd(result->data[start] + i));
                        _mm256_storeu_pd(result->data[start] + i, result_vec);
                    }
                    for (; i != mat2->cols; ++i)
                    {
                        result->data[start][i] += value * mat2->data[col][i];
                    }
                }
            }
        }
    }
    else
    {
        for (int row = 0; row != mat1->rows; ++row)
        {
            for (int col = 0; col != mat1->cols; ++col)
            {
                double value = mat1->data[row][col];
                __m256d vec1 = _mm256_set1_pd(value);
                int i = 0;
                for (; i + 4 <= mat2->cols; i += 4)
                {
                    __m256d vec2 = _mm256_loadu_pd(mat2->data[col] + i);
                    __m256d result_vec = _mm256_fmadd_pd(vec1, vec2, _mm256_loadu_pd(result->data[row] + i));
                    _mm256_storeu_pd(result->data[row] + i, result_vec);
                }
                for (; i != mat2->cols; ++i)
            {
                    result->data[row][i] += value * mat2->data[col][i];
            }
            }
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
    {
        set(temp, i, i, 1);
        set(result, i, i, 1);
    }
    for (int i = 0; i != pow; ++i)
    {
        mul_matrix(result, temp, mat);
        if (result->cols >= 4)
        {
            for (int row = 0; row != result->rows; ++row)
            {
                int col = 0;
                for (; col + 4 <= result->cols; col += 4)
                {
                    __m256d vec = _mm256_loadu_pd(result->data[row] + col);
                    _mm256_storeu_pd(temp->data[row] + col, vec);
                }
                for (; col != result->cols; ++col)
                {
                    temp->data[row][col] = result->data[row][col];
                }
            }
        }
        else
        {
            for (int row = 0; row != result->rows; ++row)
            {
                for (int col = 0; col != result->cols; ++col)
                {
                    temp->data[row][col] = result->data[row][col];
                }
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
    if (mat->cols >= 4)
    {
        __m256d zero_vec = _mm256_set1_pd(0);
        for (int row = 0; row != result->rows; ++row)
        {
            int col = 0;
            for (; col + 4 <= result->cols; col += 4)
            {
                __m256d vec = _mm256_loadu_pd(mat->data[row] + col);
                _mm256_storeu_pd(result->data[row] + col, _mm256_sub_pd(zero_vec, vec));
            }
            for (; col != result->cols; ++col)
            {
                result->data[row][col] = -(mat->data[row][col]);
            }
        }
    }
    else
    {
        for (int row = 0; row != mat->rows; ++row)
        {
            for (int col = 0; col != mat->cols; ++col)
                result->data[row][col] = -(mat->data[row][col]);
        }
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
    if (result->cols >= 4)
    {
        __m256d zero_vec = _mm256_set1_pd(0);
        for (int row = 0; row != result->rows; ++row)
        {
            int col = 0;
            for (; col + 4 <= result->cols; col += 4)
            {
                __m256d vec = _mm256_loadu_pd(mat->data[row] + col);
                vec = _mm256_max_pd(vec, _mm256_sub_pd(zero_vec, vec));
                _mm256_storeu_pd(result->data[row] + col, vec);
            }
            for (; col != result->cols; ++col)
            {
                result->data[row][col] = fabs(mat->data[row][col]);
            }
        }
    }
    else
    {
        for (int row = 0; row != mat->rows; ++row)
        {
            for (int col = 0; col != mat->cols; ++col)
                result->data[row][col] = fabs(mat->data[row][col]);
        }
    }
    return 0;
}
