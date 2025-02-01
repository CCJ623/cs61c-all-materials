#include "dumbpy.h"
#include <structmember.h>

PyTypeObject Matrix61cType;

/* Helper functions for initalization of matrices and vectors */

/*
 * Return a tuple given rows and cols
 */
PyObject* get_shape(int rows, int cols)
{
    if (rows == 1 || cols == 1)
    {
        return PyTuple_Pack(1, PyLong_FromLong(rows * cols));
    }
    else
    {
        return PyTuple_Pack(2, PyLong_FromLong(rows), PyLong_FromLong(cols));
    }
}
/*
 * Matrix(rows, cols, low, high). Fill a matrix random double values
 */
int init_rand(PyObject* self, int rows, int cols, unsigned int seed, double low,
    double high)
{
    matrix* new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    rand_matrix(new_mat, seed, low, high);
    ((Matrix61c*)self)->mat = new_mat;
    ((Matrix61c*)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * Matrix(rows, cols, val). Fill a matrix of dimension rows * cols with val
 */
int init_fill(PyObject* self, int rows, int cols, double val)
{
    matrix* new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed)
        return alloc_failed;
    else
    {
        fill_matrix(new_mat, val);
        ((Matrix61c*)self)->mat = new_mat;
        ((Matrix61c*)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    }
    return 0;
}

/*
 * Matrix(rows, cols, 1d_list). Fill a matrix with dimension rows * cols with 1d_list values
 */
int init_1d(PyObject* self, int rows, int cols, PyObject* lst)
{
    if (rows * cols != PyList_Size(lst))
    {
        PyErr_SetString(PyExc_ValueError, "Incorrect number of elements in list");
        return -1;
    }
    matrix* new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    int count = 0;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            set(new_mat, i, j, PyFloat_AsDouble(PyList_GetItem(lst, count)));
            count++;
        }
    }
    ((Matrix61c*)self)->mat = new_mat;
    ((Matrix61c*)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * Matrix(2d_list). Fill a matrix with dimension len(2d_list) * len(2d_list[0])
 */
int init_2d(PyObject* self, PyObject* lst)
{
    int rows = PyList_Size(lst);
    if (rows == 0)
    {
        PyErr_SetString(PyExc_ValueError,
            "Cannot initialize dumbpy.Matrix with an empty list");
        return -1;
    }
    int cols;
    if (!PyList_Check(PyList_GetItem(lst, 0)))
    {
        PyErr_SetString(PyExc_ValueError, "List values not valid");
        return -1;
    }
    else
    {
        cols = PyList_Size(PyList_GetItem(lst, 0));
    }
    for (int i = 0; i < rows; i++)
    {
        if (!PyList_Check(PyList_GetItem(lst, i)) ||
            PyList_Size(PyList_GetItem(lst, i)) != cols)
        {
            PyErr_SetString(PyExc_ValueError, "List values not valid");
            return -1;
        }
    }
    matrix* new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            set(new_mat, i, j,
                PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(lst, i), j)));
        }
    }
    ((Matrix61c*)self)->mat = new_mat;
    ((Matrix61c*)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * This deallocation function is called when reference count is 0
 */
void Matrix61c_dealloc(Matrix61c* self)
{
    deallocate_matrix(self->mat);
    Py_TYPE(self)->tp_free(self);
}

/* For immutable types all initializations should take place in tp_new */
PyObject* Matrix61c_new(PyTypeObject* type, PyObject* args,
    PyObject* kwds)
{
    /* size of allocated memory is tp_basicsize + nitems*tp_itemsize*/
    Matrix61c* self = (Matrix61c*)type->tp_alloc(type, 0);
    return (PyObject*)self;
}

/*
 * This matrix61c type is mutable, so needs init function. Return 0 on success otherwise -1
 */
int Matrix61c_init(PyObject* self, PyObject* args, PyObject* kwds)
{
    /* Generate random matrices */
    if (kwds != NULL)
    {
        PyObject* rand = PyDict_GetItemString(kwds, "rand");
        if (!rand)
        {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
        if (!PyBool_Check(rand))
        {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
        if (rand != Py_True)
        {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }

        PyObject* low = PyDict_GetItemString(kwds, "low");
        PyObject* high = PyDict_GetItemString(kwds, "high");
        PyObject* seed = PyDict_GetItemString(kwds, "seed");
        double double_low = 0;
        double double_high = 1;
        unsigned int unsigned_seed = 0;

        if (low)
        {
            if (PyFloat_Check(low))
            {
                double_low = PyFloat_AsDouble(low);
            }
            else if (PyLong_Check(low))
            {
                double_low = PyLong_AsLong(low);
            }
        }

        if (high)
        {
            if (PyFloat_Check(high))
            {
                double_high = PyFloat_AsDouble(high);
            }
            else if (PyLong_Check(high))
            {
                double_high = PyLong_AsLong(high);
            }
        }

        if (double_low >= double_high)
        {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }

        // Set seed if argument exists
        if (seed)
        {
            if (PyLong_Check(seed))
            {
                unsigned_seed = PyLong_AsUnsignedLong(seed);
            }
        }

        PyObject* rows = NULL;
        PyObject* cols = NULL;
        if (PyArg_UnpackTuple(args, "args", 2, 2, &rows, &cols))
        {
            if (rows && cols && PyLong_Check(rows) && PyLong_Check(cols))
            {
                return init_rand(self, PyLong_AsLong(rows), PyLong_AsLong(cols), unsigned_seed, double_low,
                    double_high);
            }
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
    }
    PyObject* arg1 = NULL;
    PyObject* arg2 = NULL;
    PyObject* arg3 = NULL;
    if (PyArg_UnpackTuple(args, "args", 1, 3, &arg1, &arg2, &arg3))
    {
        /* arguments are (rows, cols, val) */
        if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) && (PyLong_Check(arg3)
            || PyFloat_Check(arg3)))
        {
            if (PyLong_Check(arg3))
            {
                return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), PyLong_AsLong(arg3));
            }
            else
                return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), PyFloat_AsDouble(arg3));
        }
        else if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) && PyList_Check(arg3))
        {
            /* Matrix(rows, cols, 1D list) */
            return init_1d(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), arg3);
        }
        else if (arg1 && PyList_Check(arg1) && arg2 == NULL && arg3 == NULL)
        {
            /* Matrix(rows, cols, 1D list) */
            return init_2d(self, arg1);
        }
        else if (arg1 && arg2 && PyLong_Check(arg1) && PyLong_Check(arg2) && arg3 == NULL)
        {
            /* Matrix(rows, cols, 1D list) */
            return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), 0);
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return -1;
    }
}

/*
 * List of lists representations for matrices
 */
PyObject* Matrix61c_to_list(Matrix61c* self)
{
    int rows = self->mat->rows;
    int cols = self->mat->cols;
    PyObject* py_lst = NULL;
    if (self->mat->is_1d)
    {  // If 1D matrix, print as a single list
        py_lst = PyList_New(rows * cols);
        int count = 0;
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                PyList_SetItem(py_lst, count, PyFloat_FromDouble(get(self->mat, i, j)));
                count++;
            }
        }
    }
    else
    {  // if 2D, print as nested list
        py_lst = PyList_New(rows);
        for (int i = 0; i < rows; i++)
        {
            PyList_SetItem(py_lst, i, PyList_New(cols));
            PyObject* curr_row = PyList_GetItem(py_lst, i);
            for (int j = 0; j < cols; j++)
            {
                PyList_SetItem(curr_row, j, PyFloat_FromDouble(get(self->mat, i, j)));
            }
        }
    }
    return py_lst;
}

PyObject* Matrix61c_class_to_list(Matrix61c* self, PyObject* args)
{
    PyObject* mat = NULL;
    if (PyArg_UnpackTuple(args, "args", 1, 1, &mat))
    {
        if (!PyObject_TypeCheck(mat, &Matrix61cType))
        {
            PyErr_SetString(PyExc_TypeError, "Argument must of type dumbpy.Matrix!");
            return NULL;
        }
        Matrix61c* mat61c = (Matrix61c*)mat;
        return Matrix61c_to_list(mat61c);
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }
}

/*
 * Add class methods
 */
PyMethodDef Matrix61c_class_methods[] = {
    {"to_list", (PyCFunction)Matrix61c_class_to_list, METH_VARARGS, "Returns a list representation of dumbpy.Matrix"},
    {NULL, NULL, 0, NULL}
};

/*
 * Matrix61c string representation. For printing purposes.
 */
PyObject* Matrix61c_repr(PyObject* self)
{
    PyObject* py_lst = Matrix61c_to_list((Matrix61c*)self);
    return PyObject_Repr(py_lst);
}

/* NUMBER METHODS */

/*
 * Add the second dumbpy.Matrix (Matrix61c) object to the first one. The first operand is
 * self, and the second operand can be obtained by casting `args`.
 */
PyObject* Matrix61c_add(Matrix61c* self, PyObject* args)
{
    /* TODO: YOUR CODE HERE */
    if (self == NULL || args == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments");
        return NULL;
    }
    if (!PyObject_TypeCheck(args, &Matrix61cType))
    {
        PyErr_SetString(PyExc_TypeError, "Argument must of type dumbpy.Matrix!");
        return NULL;
    }
    Matrix61c* another = (Matrix61c*)args;
    if (self->mat->rows != another->mat->rows || self->mat->cols != another->mat->cols)
    {
        PyErr_SetString(PyExc_ValueError, "Two dumbpy.Matrix must have same dimensions!");
        return NULL;
    }
    Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);
    if (result == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate new dumbpy.Matrix!");
        return NULL;
    }
    if (init_fill((PyObject*)result, self->mat->rows, self->mat->cols, 0) != 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate new dumbpy.Matrix!");
        return NULL;
    }
    if (add_matrix(result->mat, self->mat, another->mat) != 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "Two dumbpy.Matrix must have same dimensions!");
        return NULL;
    }
    return (PyObject*)result;
}

/*
 * Substract the second dumbpy.Matrix (Matrix61c) object from the first one. The first operand is
 * self, and the second operand can be obtained by casting `args`.
 */
PyObject* Matrix61c_sub(Matrix61c* self, PyObject* args)
{
    /* TODO: YOUR CODE HERE */
    if (self == NULL || args == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments");
        return NULL;
    }
    if (!PyObject_TypeCheck(args, &Matrix61cType))
    {
        PyErr_SetString(PyExc_TypeError, "Argument must of type dumbpy.Matrix!");
        return NULL;
    }
    Matrix61c* another = (Matrix61c*)args;
    return Matrix61c_add(self, Matrix61c_neg(another));
}

/*
 * NOT element-wise multiplication. The first operand is self, and the second operand
 * can be obtained by casting `args`.
 */
PyObject* Matrix61c_multiply(Matrix61c* self, PyObject* args)
{
    /* TODO: YOUR CODE HERE */
    if (self == NULL || args == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments");
        return NULL;
    }
    if (!PyObject_TypeCheck(args, &Matrix61cType))
    {
        PyErr_SetString(PyExc_TypeError, "Argument must of type dumbpy.Matrix!");
        return NULL;
    }
    Matrix61c* another = (Matrix61c*)args;
    if (self->mat->cols != another->mat->rows)
    {
        PyErr_SetString(PyExc_ValueError, "Invalid dumbpy.Matrix dimensions!");
        return NULL;
    }
    Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);
    if (result == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate new dumbpy.Matrix!");
        return NULL;
    }
    if (init_fill((PyObject*)result, self->mat->rows, another->mat->cols, 0) != 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate new dumbpy.Matrix!");
        return NULL;
    }
    if (mul_matrix(result->mat, self->mat, another->mat) != 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "Invalid dumbpy.Matrix dimensions!");
        return NULL;
    }
    return (PyObject*)result;
}

/*
 * Negates the given dumbpy.Matrix.
 */
PyObject* Matrix61c_neg(Matrix61c* self)
{
    /* TODO: YOUR CODE HERE */
    if (self == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments");
        return NULL;
    }
    Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);
    if (result == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate new dumbpy.Matrix!");
        return NULL;
    }
    if (init_fill((PyObject*)result, self->mat->rows, self->mat->cols, 0) != 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate new dumbpy.Matrix!");
        return NULL;
    }
    neg_matrix(result->mat, self->mat);
    return (PyObject*)result;
}

/*
 * Take the element-wise absolute value of this dumbpy.Matrix.
 */
PyObject* Matrix61c_abs(Matrix61c* self)
{
    /* TODO: YOUR CODE HERE */
    if (self == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments");
        return NULL;
    }
    Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);
    if (result == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate new dumbpy.Matrix!");
        return NULL;
    }
    if (init_fill((PyObject*)result, self->mat->rows, self->mat->cols, 0) != 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate new dumbpy.Matrix!");
        return NULL;
    }
    abs_matrix(result->mat, self->mat);
    return (PyObject*)result;
}

/*
 * Raise dumbpy.Matrix (Matrix61c) to the `pow`th power. You can ignore the argument `optional`.
 */
PyObject* Matrix61c_pow(Matrix61c* self, PyObject* pow, PyObject* optional)
{
    /* TODO: YOUR CODE HERE */
    if (self == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments");
        return NULL;
    }
    if (!PyLong_Check(pow))
    {
        PyErr_SetString(PyExc_TypeError, "Argument must of type Integer!");
        return NULL;
    }
    if (self->mat->rows != self->mat->cols)
    {
        PyErr_SetString(PyExc_ValueError, "Invalid dumbpy.Matrix dimensions!");
        return NULL;
    }
    Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);
    if (result == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate new dumbpy.Matrix!");
        return NULL;
    }
    if (init_fill((PyObject*)result, self->mat->rows, self->mat->cols, 0) != 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate new dumbpy.Matrix!");
        return NULL;
    }
    if (pow_matrix(result->mat, self->mat, PyLong_AsLong(pow)) != 0)
    {
        PyErr_SetString(PyExc_ValueError, "Invalid dumbpy.Matrix dimensions!");
        return NULL;
    }
    return (PyObject*)result;
}

/*
 * Create a PyNumberMethods struct for overloading operators with all the number methods you have
 * define. You might find this link helpful: https://docs.python.org/3.6/c-api/typeobj.html
 */
PyNumberMethods Matrix61c_as_number = {
    /* TODO: YOUR CODE HERE */
    (binaryfunc)Matrix61c_add,
    (binaryfunc)Matrix61c_sub,
    (binaryfunc)Matrix61c_multiply,
    NULL,
    NULL,
    (ternaryfunc)Matrix61c_pow,
    (unaryfunc)Matrix61c_neg,
    NULL,
    (unaryfunc)Matrix61c_abs,
    NULL
};


/* INSTANCE METHODS */

/*
 * Given a dumbpy.Matrix self, parse `args` to (int) row, (int) col, and (double/int) val.
 * Return None in Python (this is different from returning null).
 */
PyObject* Matrix61c_set_value(Matrix61c* self, PyObject* args)
{
    /* TODO: YOUR CODE HERE */
    if (self == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments");
        return NULL;
    }
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 3)
    {
        PyErr_SetString(PyExc_TypeError, "Incorrect number of elements in list");
        return NULL;
    }
    PyObject* py_row, * py_col, * py_val;
    PyArg_UnpackTuple(args, "args", 3, 3, &py_row, &py_col, &py_val);
    if (!PyLong_Check(py_row) || !PyLong_Check(py_col) || (!PyLong_Check(py_val) && !PyFloat_Check(py_val)))
    {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }
    int row = PyLong_AsLong(py_row), col = PyLong_AsLong(py_col);
    double val = PyFloat_AsDouble(py_val);
    if (row < 0 || row >= self->mat->rows || col < 0 || col >= self->mat->cols)
    {
        PyErr_SetString(PyExc_IndexError, "Index out of range!");
        return NULL;
    }
    set(self->mat, row, col, val);
    Py_RETURN_NONE;
}

/*
 * Given a dumbpy.Matrix `self`, parse `args` to (int) row and (int) col.
 * Return the value at the `row`th row and `col`th column, which is a Python
 * float/int.
 */
PyObject* Matrix61c_get_value(Matrix61c* self, PyObject* args)
{
    /* TODO: YOUR CODE HERE */
    if (self == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments");
        return NULL;
    }
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 2)
    {
        PyErr_SetString(PyExc_TypeError, "Incorrect number of elements in list");
        return NULL;
    }
    PyObject* py_row, * py_col;
    PyArg_UnpackTuple(args, "args", 2, 2, &py_row, &py_col);
    if (!PyLong_Check(py_row) || !PyLong_Check(py_col))
    {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }
    int row = PyLong_AsLong(py_row), col = PyLong_AsLong(py_col);
    if (row < 0 || row >= self->mat->rows || col < 0 || col >= self->mat->cols)
    {
        PyErr_SetString(PyExc_IndexError, "Index out of range!");
        return NULL;
    }
    return PyFloat_FromDouble(get(self->mat, row, col));
}

/*
 * Create an array of PyMethodDef structs to hold the instance methods.
 * Name the python function corresponding to Matrix61c_get_value as "get" and Matrix61c_set_value
 * as "set"
 * You might find this link helpful: https://docs.python.org/3.6/c-api/structures.html
 */
PyMethodDef Matrix61c_methods[] = {
    /* TODO: YOUR CODE HERE */
    {"set", (PyCFunction)Matrix61c_set_value, METH_VARARGS, "set"},
    {"get", (PyCFunction)Matrix61c_get_value, METH_VARARGS, "get"},
    {NULL, NULL, 0, NULL}
};

/* INDEXING */

/*
 * Given a dumbpy.Matrix `self`, index into it with `key`. Return the indexed result.
 */
PyObject* Matrix61c_subscript(Matrix61c* self, PyObject* key)
{
    /* TODO: YOUR CODE HERE */
    if (self == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments");
        return NULL;
    }
    long int row_start, row_stop, row_step = 1, row_length,
        col_start, col_stop, col_step = 1, col_length;
    // setup arguments above depending on different matrix types and key types
    if (self->mat->is_1d)
    {
        if (PyLong_Check(key))
        {
            int index = PyLong_AsLong(key);
            if (self->mat->rows == 1)
            {
                row_start = 0;
                col_start = index;
            }
            else
            {
                row_start = index;
                col_start = 0;
            }
            row_stop = row_start + 1;
            col_stop = col_start + 1;
        }
        else if (PySlice_Check(key))
        {
            long int start, stop, step, length;
            if (PySlice_GetIndicesEx(key, self->mat->rows * self->mat->cols, &start, &stop, &step, &length) != 0)
            {
                PyErr_SetString(PyExc_TypeError, "Invalid arguments");
                return NULL;
            }
            row_step = col_step = step;
            row_start = start / self->mat->cols;
            col_start = start % self->mat->cols;
            if (self->mat->rows == 1)
            {
                row_stop = row_start + 1;
                col_stop = col_start + length;
            }
            else
            {
                row_stop = row_start + length;
                col_stop = col_start + 1;
            }
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return NULL;
        }
    }
    else
    {
        if (PyLong_Check(key))
        {
            row_start = PyLong_AsLong(key);
            row_stop = row_start + 1;
            col_start = 0;
            col_stop = self->mat->cols;
        }
        else if (PySlice_Check(key))
        {
            if (PySlice_GetIndices(key, self->mat->rows, &row_start, &row_stop, &row_step) != 0)
            {
                PyErr_SetString(PyExc_TypeError, "Invalid arguments");
                return NULL;
            }
            col_start = 0, col_stop = self->mat->cols;
        }
        else if (PyTuple_Check(key))
        {
            if (PyTuple_Size(key) != 2)
            {
                PyErr_SetString(PyExc_TypeError, "Invalid arguments");
                return NULL;
            }
            PyObject* row_range, * col_range;
            PyArg_UnpackTuple(key, "key", 2, 2, &row_range, &col_range);
            if (PyLong_Check(row_range))
            {
                row_start = PyLong_AsLong(row_range);
                row_stop = row_start + 1;
            }
            else if (PySlice_Check(row_range))
            {
                if (PySlice_GetIndices(row_range, self->mat->rows, &row_start, &row_stop, &row_step) != 0)
                {
                    PyErr_SetString(PyExc_TypeError, "Invalid arguments");
                    return NULL;
                }
            }
            else
            {
                PyErr_SetString(PyExc_TypeError, "Invalid arguments");
                return NULL;
            }
            if (PyLong_Check(col_range))
            {
                col_start = PyLong_AsLong(col_range);
                col_stop = col_start + 1;
            }
            else if (PySlice_Check(col_range))
            {
                if (PySlice_GetIndices(col_range, self->mat->cols, &col_start, &col_stop, &col_step) != 0)
                {
                    PyErr_SetString(PyExc_TypeError, "Invalid arguments");
                    return NULL;
                }
            }
            else
            {
                PyErr_SetString(PyExc_TypeError, "Invalid arguments");
                return NULL;
            }

        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return NULL;
        }

    }
    row_length = row_stop - row_start;
    col_length = col_stop - col_start;
    // check validity of row range and col range
    if (row_step != 1 || col_step != 1 || row_length < 1 || col_length < 1)
    {
        PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
        return NULL;
    }
    if (row_start < 0 || row_stop > self->mat->rows)
    {
        PyErr_SetString(PyExc_IndexError, "Index out of range!");
        return NULL;
    }
    if (col_start < 0 || col_stop > self->mat->cols)
    {
        PyErr_SetString(PyExc_IndexError, "Index out of range!");
        return NULL;
    }
    // slice matrix basing on row range and col range
    if (row_length == 1 && col_length == 1)
    {
        return PyFloat_FromDouble(get(self->mat, row_start, col_start));
    }
    Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);
    result->shape = get_shape(row_length, col_length);
    if (allocate_matrix_ref(&(result->mat), self->mat, row_start, col_start, row_length, col_length) != 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate new dumbpy.Matrix!");
        return NULL;
    }
    return (PyObject*)result;
}

/*
 * Given a dumbpy.Matrix `self`, index into it with `key`, and set the indexed result to `v`.
 */
int Matrix61c_set_subscript(Matrix61c* self, PyObject* key, PyObject* v)
{
    /* TODO: YOUR CODE HERE */
    if (self == NULL || key == NULL || v == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments");
        return -1;
    }
    // slice target matrix
    Matrix61c* slice = (Matrix61c*)Matrix61c_subscript(self, key);
    if (PyFloat_Check((PyObject*)slice))
    {
        double value = PyFloat_AsDouble((PyObject*)slice);
        slice = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);
        if (init_fill((PyObject*)slice, 1, 1, value) != 0)
        {
            PyErr_SetString(PyExc_RuntimeError, "Failed to allocate new dumbpy.Matrix!");
            return -1;
        }
    }
    if (slice == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments");
        return -1;
    }
    // TypeError check
    if (slice->mat->rows == 1 && slice->mat->cols == 1)
    {
        if (!PyLong_Check(v) && !PyFloat_Check(v))
        {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
    }
    else if (!PyList_Check(v))
    {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return -1;
    }
    // ValueError check
    if (slice->mat->is_1d)
    {
        if (PyList_Check(v))
        {
            int length = PyList_Size(v);
            if (length != slice->mat->rows * slice->mat->cols)
            {
                PyErr_SetString(PyExc_ValueError, "Incorrect number of elements in list!");
                return -1;
            }
            for (int i = 0; i < length; i++)
            {
                PyObject* value = PyList_GetItem(v, i);
                if (!PyFloat_Check(value) && !PyLong_Check(value))
                {
                    PyErr_SetString(PyExc_ValueError, "Element is not a float or int!");
                    return -1;
                }
            }
        }
        else if (!PyLong_Check(v) && !PyFloat_Check(v))
        {
            PyErr_SetString(PyExc_ValueError, "Element is not a float or int!");
            return -1;
        }
    }
    else
    {
        if (PyList_Size(v) != slice->mat->rows)
        {
            PyErr_SetString(PyExc_ValueError, "Incorrect number of elements in list!");
            return -1;
        }
        for (int row = 0; row != slice->mat->rows; row++)
        {
            PyObject* row_list = PyList_GetItem(v, row);
            if (!PyList_Check(row_list))
            {
                PyErr_SetString(PyExc_TypeError, "Element is not a list!");
                return -1;
            }
            if (PyList_Size(row_list) != slice->mat->cols)
            {
                PyErr_SetString(PyExc_ValueError, "Incorrect number of elements in list!");
                return -1;
            }
            for (int col = 0; col != slice->mat->cols; col++)
            {
                PyObject* value = PyList_GetItem(row_list, col);
                if (!PyFloat_Check(value) && !PyLong_Check(value))
                {
                    PyErr_SetString(PyExc_ValueError, "Element is not a float or int!");
                    return -1;
                }
            }
        }
    }
    // set value
    if (PyLong_Check(v) || PyFloat_Check(v))
    {
        double value;
        if (PyLong_Check(v))
            value = PyLong_AsDouble(v);
        else
            value = PyFloat_AsDouble(v);
        set(slice->mat, 0, 0, value);
    }
    else if (slice->mat->is_1d)
    {
        int index = 0;
        for (int row = 0; row != slice->mat->rows; row++)
        {
            for (int col = 0; col != slice->mat->cols; col++)
            {
                double value;
                PyObject* item = PyList_GetItem(v, index);
                if (PyLong_Check(item))
                    value = PyLong_AsDouble(item);
                else
                    value = PyFloat_AsDouble(item);
                set(slice->mat, row, col, value);
                index++;
            }
        }
    }
    else
    {
        for (int row = 0; row != slice->mat->rows; row++)
        {
            for (int col = 0; col != slice->mat->cols; col++)
            {
                double value;
                PyObject* item = PyList_GetItem(PyList_GetItem(v, row), col);
                if (PyLong_Check(item))
                    value = PyLong_AsDouble(item);
                else
                    value = PyFloat_AsDouble(item);
                set(slice->mat, row, col, value);
            }
        }
    }
    return 0;
}


PyMappingMethods Matrix61c_mapping = {
    NULL,
    (binaryfunc)Matrix61c_subscript,
    (objobjargproc)Matrix61c_set_subscript,
};

/* INSTANCE ATTRIBUTES*/
PyMemberDef Matrix61c_members[] = {
    {
        "shape", T_OBJECT_EX, offsetof(Matrix61c, shape), 0,
        "(rows, cols)"
    },
    {NULL}  /* Sentinel */
};

PyTypeObject Matrix61cType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "dumbpy.Matrix",
    .tp_basicsize = sizeof(Matrix61c),
    .tp_dealloc = (destructor)Matrix61c_dealloc,
    .tp_repr = (reprfunc)Matrix61c_repr,
    .tp_as_number = &Matrix61c_as_number,
    .tp_flags = Py_TPFLAGS_DEFAULT |
    Py_TPFLAGS_BASETYPE,
    .tp_doc = "dumbpy.Matrix objects",
    .tp_methods = Matrix61c_methods,
    .tp_members = Matrix61c_members,
    .tp_as_mapping = &Matrix61c_mapping,
    .tp_init = (initproc)Matrix61c_init,
    .tp_new = Matrix61c_new
};


struct PyModuleDef dumbpymodule = {
    PyModuleDef_HEAD_INIT,
    "dumbpy",
    "dumbpy matrix operations",
    -1,
    Matrix61c_class_methods
};

/* Initialize the dumbpy module */
PyMODINIT_FUNC PyInit_dumbpy(void)
{
    PyObject* m;

    if (PyType_Ready(&Matrix61cType) < 0)
        return NULL;

    m = PyModule_Create(&dumbpymodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&Matrix61cType);
    PyModule_AddObject(m, "Matrix", (PyObject*)&Matrix61cType);
    printf("CS61C Fall 2020 Project 4: dumbpy imported!\n");
    fflush(stdout);
    return m;
}