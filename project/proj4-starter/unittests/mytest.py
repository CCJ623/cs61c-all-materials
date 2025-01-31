from utils import *
import numc as nc

def main():
    a = nc.Matrix(2, 2, rand=True)
    a[0][0]
    dp_mat1, nc_mat1 = rand_dp_nc_matrix(2, 2, seed=0)
    dp_mat2, nc_mat2 = rand_dp_nc_matrix(2, 2, seed=1)
    is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
    self.assertTrue(is_correct)
    print_speedup(speed_up)


if __name__ == "__main__":
    main()