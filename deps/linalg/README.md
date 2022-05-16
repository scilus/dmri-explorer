# Linear Algebra
A big matrix class written in C++.


## Supported Operators

- **+ :** Either adds two same sized matrices, or adds a constant to each element of the matrix

- **- :** Either subtracts from one matrix to other (same sized matrices), or subtracts a constant from each element of the matrix

- **\* :** Either multiply two matrices (mxn * nxk = mxk), or scale the matrix by a constant

- **/ :** Divides each element of the matrix by a constant

- **== :** Checks if two matrices are same

- **!= :** Checks if two matrices are different

- **= :** Assignment operator


### Syntactic Sugars

- **+= operator**

- **-= operator**

- **\*= operator**

- **/= operator**


## Getters

- **getRows():** Returns the number of rows

- **getCols():** Returns the number of columns

- **return_col(i):** Returns the ith column of the matrix

- **return_row(i):** Returns the ith row of the matrix


## Linear Algebra

- **T(self=false) :** Returns the transpose of the matrix, if self (default false), updates the matrix

- **gaussianElimination(self=false) :** Returns the row-echolon form of the matrix, if self (default false), updates the matrix

- **inverse() :** Returns the inverse of the matrix

- **determinant() :** Returns the determinant of the matrix (defined only on square matrices)

- **qr_decomposition() :** Returns matrices Q(orthogonal matrix) and R(upper triangular matrix) that satisfies Q*R = object using Gram-Schmidt equations(analytically). The algorithm fails to deliver correct Q and R when the object is singular.

- **singular_value_decomposition() :** Returns matrices U, E, V_T where U contains A*A_T's eigenvectors, E contains the eigenvalues of A and  V contains A_T*A's eigenvectors using Jacobi Eigenvalue Algorithm (numerically).

- **eigs() :** Returns the eigenvalues of the matrix (utilizing Jacobi Eigenvalue Algorithm)


## Util Functions

- **clipCols(start, end, self=false) :** Returns the columns between start and end (M[start:end]), if self (default false), updates the matrix

- **clipRows(start, end, self=false) :** Returns the rows between start and end, if self (default false), updates the matrix

- **swapRows(i,j) :** Swaps ith row with jth.

- **swapCols(i,j) :** Swaps ith column with jth.

- **remove_row_col(i,j, self=false) :** Returns a matrix that does not contain ith row and jth column of the object, if self (default false), updates the matrix (utilized while calculating the cofactor matrix)

- **sum(axis=0, self=false) :** Returns the sum of the elements in given axis

- **frobeniusNorm() :** Returns the Frobenius Norm of the matrix

- **assignCol(m, col_num) :** Assigns m to col_numth column of the matrix

- **assignRow(m, row_num) :** Assigns m to row_numth row of the matrix

- **apply(function) :** Applies the function to the each element of the matrix (mutates the object)

- **printMatrix() :** Prints the matrix with 2 precision


## Free Functions

- **eye(n) :** Returns an identity matrix with size nxn

- **zeros(n) :** Returns a matrix of zeros with size nxn

- **ones(n) :** Returns a matrix of ones with size nxn

- **diag(m) :** Returns a matrix whose diagonal values are m

- **matrixtoVector(m) :** Returns the vector representation of matrix m
