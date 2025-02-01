from utils import *
from unittest import TestCase

"""
For each operation, you should write tests to test  on matrices of different sizes.
Hint: use dp_mc_matrix to generate dumbpy and numc matrices with the same data and use
      cmp_dp_nc_matrix to compare the results
"""
class TestAdd(TestCase):
    def test_small_add(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(2, 2, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(2, 2, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_add(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(100, 100, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(100, 100, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_add(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(1000, 1000, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(1000, 1000, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_value_error(self):
        nc_mat1 = nc.Matrix(2,3, rand=True)
        nc_mat2 = nc.Matrix(3,2, rand=True)
        with self.assertRaises(ValueError):
            nc_mat1 + nc_mat2

    def test_type_error(self):
        nc_mat = nc.Matrix(2,3, rand=True)
        with self.assertRaises(TypeError):
            nc_mat + 3
        with self.assertRaises(TypeError):
            nc_mat + 3.1
        with self.assertRaises(TypeError):
            nc_mat + [1,2,3]

class TestSub(TestCase):
    def test_small_sub(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(2, 2, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(2, 2, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_sub(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(100, 100, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(100, 100, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_sub(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(1000, 1000, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(1000, 1000, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_value_error(self):
        nc_mat1 = nc.Matrix(2,3, rand=True)
        nc_mat2 = nc.Matrix(3,2, rand=True)
        with self.assertRaises(ValueError):
            nc_mat1 - nc_mat2

    def test_type_error(self):
        nc_mat = nc.Matrix(2,3, rand=True)
        with self.assertRaises(TypeError):
            nc_mat - 3
        with self.assertRaises(TypeError):
            nc_mat - 3.1
        with self.assertRaises(TypeError):
            nc_mat - [1,2,3]

class TestAbs(TestCase):
    def test_small_abs(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_abs(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(100, 100, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_abs(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(1000, 1000, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

class TestNeg(TestCase):
    def test_small_neg(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_neg(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(10, 10, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_neg(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(100, 100, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

class TestMul(TestCase):
    def test_small_mul(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(2, 2, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(2, 2, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_mul(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(10, 5, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(5, 10, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_mul(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(100, 100, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(100, 100, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_value_error(self):
        nc_mat1 = nc.Matrix(2,4, rand=True)
        nc_mat2 = nc.Matrix(3,2, rand=True)
        with self.assertRaises(ValueError):
            nc_mat1 * nc_mat2

    def test_type_error(self):
        nc_mat = nc.Matrix(2,3, rand=True)
        with self.assertRaises(TypeError):
            nc_mat * 3
        with self.assertRaises(TypeError):
            nc_mat * 3.1
        with self.assertRaises(TypeError):
            nc_mat * [1,2,3]

class TestPow(TestCase):
    def test_small_pow(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        is_correct, speed_up = compute([dp_mat, 3], [nc_mat, 3], "pow")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_pow(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(10, 10, seed=0)
        is_correct, speed_up = compute([dp_mat, 9], [nc_mat, 9], "pow")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_pow(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(100, 100, seed=0)
        is_correct, speed_up = compute([dp_mat, 81], [nc_mat, 81], "pow")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_value_error(self):
        nc_mat = nc.Matrix(2,3, rand=True)
        with self.assertRaises(ValueError):
            nc_mat ** 2

    def test_type_error(self):
        nc_mat = nc.Matrix(2,3, rand=True)
        with self.assertRaises(TypeError):
            nc_mat ** nc.Matrix(2,3, rand=True)
        with self.assertRaises(TypeError):
            nc_mat ** 3.1
        with self.assertRaises(TypeError):
            nc_mat ** [1,2,3]

class TestGet(TestCase):
    def test_get(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        rand_row = np.random.randint(dp_mat.shape[0])
        rand_col = np.random.randint(dp_mat.shape[1])
        self.assertEqual(round(dp_mat[rand_row][rand_col], decimal_places),
            round(nc_mat[rand_row][rand_col], decimal_places))

    def test_index_error(self):
        nc_mat = nc.Matrix(2,3, rand=True)
        with self.assertRaises(IndexError):
            nc_mat.get(0,3)
        with self.assertRaises(IndexError):
            nc_mat.get(2,0)
        with self.assertRaises(IndexError):
            nc_mat.get(-1,-1)

    def test_type_error(self):
        nc_mat = nc.Matrix(2,3, rand=True)
        with self.assertRaises(TypeError):
            nc_mat.get(0,3.1)
        with self.assertRaises(TypeError):
            nc_mat.get(0,0,0)
        with self.assertRaises(TypeError):
            nc_mat.get(0)

class TestSet(TestCase):
    def test_set(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        rand_row = np.random.randint(dp_mat.shape[0])
        rand_col = np.random.randint(dp_mat.shape[1])
        self.assertEquals(round(dp_mat[rand_row][rand_col], decimal_places),
            round(nc_mat[rand_row][rand_col], decimal_places))
    
    def test_index_error(self):
        nc_mat = nc.Matrix(2,3, rand=True)
        with self.assertRaises(IndexError):
            nc_mat.set(0,3,0)
        with self.assertRaises(IndexError):
            nc_mat.set(2,0,0)
        with self.assertRaises(IndexError):
            nc_mat.set(-1,-1,0)

    def test_type_error(self):
        nc_mat = nc.Matrix(2,3, rand=True)
        with self.assertRaises(TypeError):
            nc_mat.set(0,3.1)
        with self.assertRaises(TypeError):
            nc_mat.set(0.1,0,0)
        with self.assertRaises(TypeError):
            nc_mat.set(0,0.1,0)
        with self.assertRaises(TypeError):
            nc_mat.set(0,0,nc.Matrix(1,1,rand=True))

class TestShape(TestCase):
    def test_shape(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        self.assertTrue(dp_mat.shape == nc_mat.shape)
