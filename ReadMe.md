CHALLENGE 2: A sparse matrix
--------------------------------------
This GitBub repository contains the second challenge from PACS course made by Miguel Alcañiz Moya.
 
The repository contains the code in the main.cpp file, the sparse matrix class defined in the sparse_matrix.hpp, the data from the example matrix in the lnsp_131.mtx file and the Makefile.

Summary of the main
--------------------------------------
Although the main is already commented, if you execute the code it will print several examples of matrix using the class created for sparse matrix.

At first it sets a 4x4 matrix stored row-wise and it prints it in its uncompressed and compressed state.
Then it set the same matrix but stored column-wise and print it uncompressed and compressed.
After it gives an example of the product of matrices times vector.

Then it reads from the file lnsp_131 the matrix given as example for the challenge.

Finally it prints the time that it takes to do the product between a big sparse matrix times a vector in the different tipes of matrix forms (compressed/uncompressed and row-wise/column-wise).