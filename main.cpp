#include "sparse_matrix.hpp"
#include "chrono.hpp"
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cmath>


int
main(int argc, char **argv)
{
  //-----------------------------------------------------------------------------
  // Now I'm gonna put a little sized matrix example to see how the class works
  // We start with a matrix with storage order row-wise

   std::cout<< "Example of matrix row-wise uncompressed:" << std::endl;
   SparseMatrix<double, StorageOrder::Row_wise> M_row(4, 4);
   SparseMatrix<double, StorageOrder::Row_wise> M_column(4, 4);

   M_row[{0,0}] = 4.0; M_column[{0,0}] = 4.0;
   M_row[{0,1}] = -1.0; M_column[{0,1}] = -1.0;
   M_row[{1,0}] = -1.0; M_column[{1,0}] = -1.0;
   M_row[{1,1}] = 4.0; M_column[{1,1}] = 4.0;
   M_row[{1,2}] = -1.0; M_column[{1,2}] = -1.0;
   M_row[{2,1}] = -1.0; M_column[{2,1}] = -1.0;
   M_row[{2,2}] = 4.0; M_column[{2,2}] = 4.0;
   M_row[{2,3}] = -1.0; M_column[{2,3}] = -1.0;
   M_row[{3,2}] = -1.0; M_column[{3,2}] = -1.0;
   M_row[{3,3}] = 4.0; M_column[{3,3}] = 4.0;
   std::cout<< M_row << std::endl;


   std::cout<< "Example of matrix row-wise compressed:" << std::endl;
   M_row.compress();
   std::cout<< M_row << std::endl;

   std::vector<double> x(4, 2); //an example of vector just to try the multiplication
   std::vector<double> zz = M_row*x;
   std::cout<< "Matrix (row-wise compressed) times vector result: ";
   for(const double & i : zz) std::cout<< i << ' ';
   std::cout<< std::endl << std::endl;
   
   std::cout<< "Matrix (row-wise uncompressed) times vector result: ";
   M_row.uncompress(); 
   zz = M_row*x;  
   for(const double & i : zz) std::cout<< i << ' ';
   std::cout<< std::endl << std::endl;

  // Now with a matrix with storage order column-wise (the same one as before)

   std::cout<< "Example of matrix (column-wise compressed):" << std::endl;
   M_column.compress();
   std::cout<< M_column << std::endl;

   zz = M_column*x;
   std::cout<< "Matrix (column-wise compressed) times vector result: ";
   for(const double & i : zz) std::cout<< i << ' ';
   std::cout<< std::endl << std::endl;
   
   std::cout<< "Matrix (column-wise uncompressed) times vector result: ";
   M_column.uncompress(); 
   zz = M_column*x;  
   for(const double & i : zz) std::cout<< i << ' ';
   std::cout<< std::endl << std::endl;

  //---------------------------------------------------------------------------------
  //Now I'm reading the matrix from the file lnsp_131.mtx (a 'big' sparse matrix) 

  std::string file_name = "./lnsp_131.mtx";
  
  SparseMatrix<double, StorageOrder::Row_wise> lnsp_131_row; 
  SparseMatrix<double, StorageOrder::Column_wise> lnsp_131_column; 

  //I made a friend function of the class called readmatrix but I don't understand why it doesn't work
  //The function just does the same I'm doing in the following lines in the main
  //readmatrix(file_name, lnsp_131); (this is the line that doesn't work)
  
  std::ifstream file(file_name);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + file_name);
  }

  std::string line;

  while (file.peek() == '%') {
    file.ignore(2048, '\n');
  }

  size_t num_rows, num_cols, num_elements;

  file >> num_rows >> num_cols >> num_elements;

  lnsp_131_row.resize(num_rows,num_cols);
  lnsp_131_column.resize(num_rows,num_cols);

  for (size_t i = 0; i < num_elements; ++i) {
    size_t row, col;
    double value;
    file >> row >> col >> value;
    // we always use the format (row, col) -> value
    lnsp_131_row[{row - 1, col - 1}] = value;
    lnsp_131_column[{row - 1, col - 1}] = value;
  }

  //I can print the matrix, but its way too big
  //std::cout<< lnsp_131_column << std::endl;

  //---------------------------------------------------------------------------------
  // MATRIX TIMES VECTOR EFFICIENCY
  // Now I'm gonna see how much time it takes to do the product matrix x vector

  //I create a random vector with the right size
  std::vector<double> v(131);
  for(int i = 0; i < 131; i++) 
    v[i] = 38756 - pow(1.3,125-i);
  
  //I'm using the chrono.hpp to time the operations
  Timings::Chrono timer;
  timer.start();
  std::vector<double> za = lnsp_131_row*v;
  timer.stop();
  std::cout<< "Time spent in doing the product matrix x vector (row-wise uncompressed):";
  std::cout<< std::endl;
  std::cout << timer << std::endl;

  lnsp_131_row.compress();
  timer.start();
  std::vector<double> za_compressed = lnsp_131_row*v;
  timer.stop();
  std::cout<< "Time spent in doing the product matrix x vector (row-wise compressed):";
  std::cout<< std::endl;
  std::cout << timer << std::endl;

  timer.start();
  std::vector<double> zaa = lnsp_131_column*v;
  timer.stop(); 
  std::cout<< "Time spent in doing the product matrix x vector (column-wise uncompressed):";
  std::cout<< std::endl;
  std::cout << timer << std::endl;

  lnsp_131_column.compress();
  timer.start();
  std::vector<double> zaa_compressed = lnsp_131_column*v;
  timer.stop();
  std::cout<< "Time spent in doing the product matrix x vector (column-wise compressed):";
  std::cout<< std::endl;
  std::cout << timer << std::endl;

  return 0; 
}