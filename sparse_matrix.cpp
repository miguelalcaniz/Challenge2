#include "sparse_matrix.hpp"
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

template <class T, StorageOrder St>
std::ostream &
operator<<(std::ostream &stream, SparseMatrix<T, St> &M)
{
  stream << "nrows = " << M.m_n
   << "; ncols = " << M.m_m << ";" << std::endl;
  std::array<std::size_t,2> element;
  if(M.compressed){
    stream << "The matrix is compressed." << std::endl;
    stream<< "Values : ";
    for(const auto& i : M.val) stream << i << ' ';
    stream << std::endl;
    stream<< "Column indices : ";
    for(const auto& i : M.idx) stream << i << ' ';
    stream << std::endl;
    stream<< "Row offset : ";
    for(const auto& i : M.ptr) stream << i << ' ';
    stream << std::endl;
  }
  else{
    stream << "The matrix is uncompressed." << std::endl;
    stream << "matrix:  ";
      for (size_t i = 0; i < M.m_n; ++i){
        if(i != 0) stream << "         ";
        stream << "[";
        for (size_t j = 0; j < M.m_m; ++j){
          element = {i, j};
          auto it = M.m_data.find(element)    ;
          if(it != M.m_data.end()){
            stream << it->second;
          }
          else stream << 0;
          if(j+1 != M.m_m) stream << ", ";
          else stream << ']' << std::endl;
        }
      }
  }

  return stream;
}

template <class T, StorageOrder St>
void
readmatrix(const std::string& file_name,SparseMatrix<T, St> &M){

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

  M.resize(num_rows,num_cols);

  std::cout<< num_elements << std::endl;

  for (size_t i = 0; i < num_elements; ++i) {
    size_t row, col;
    double value;
    file >> row >> col >> value;
    std::cout<< row << ' ' << col << ' ' << value << std::endl;
    // we always use the format (row, col) -> value
    M[{row - 1, col - 1}] = value;
  }
}


template <class T, StorageOrder St>
void
SparseMatrix<T, St>::line(SparseMatrix<T, St> &x, const size_t k)
{
    x.resize(1,m_m);
    if(St == StorageOrder::Row_wise){
      auto lower = m_data.lower_bound({k,0});
      auto upper = m_data.upper_bound({k,m_m-1});
      for (auto it = lower; it != upper; ++it)
        x[it->first] = it->second;
    }
    else{
      auto lower = m_data.lower_bound({0,k});
      auto upper = m_data.upper_bound({m_n-1,0});
      for (auto it = lower; it != upper; ++it)
        x[it->first] = it->second;
    }
}

template <class T, StorageOrder St>
void
SparseMatrix<T, St>::compress()
{
  if(St == StorageOrder::Row_wise){
    compressed = true;
    val = std::vector<T>();
    idx = std::vector<int>();
    ptr = std::vector<int>(m_n+1);
    size_t index = 0, pointer = 1, k = 0;
    ptr[0] = 0;
    for(const auto& pair : m_data){
      while(pair.first[0] > k) {
          ptr[pointer] = index;
          pointer++;
          k++;
      }
      index++;
      idx.push_back(pair.first[1]);
      val.push_back(pair.second);
    }
    ptr[m_n] = val.size();
    
    m_data.clear();
  }
  else{
    compressed = true;
    val = std::vector<T>();
    idx = std::vector<int>();
    ptr = std::vector<int>(m_m+1);
    size_t index = 0, pointer = 1, k = 0;
    ptr[0] = 0;
    for(const auto& pair : m_data){
      while(pair.first[1] > k) {
          ptr[pointer] = index;
          pointer++;
          k++;
      }
      index++;
      idx.push_back(pair.first[0]);
      val.push_back(pair.second);
    }
    ptr[m_m] = val.size();
    m_data.clear();

  }
}

template <class T, StorageOrder St>
void
SparseMatrix<T, St>::uncompress()
{
  if(!compressed) return;
  compressed = false;
  if(St == StorageOrder::Row_wise){
    for(size_t i = 0; i < m_n; i++){
      for(size_t j = ptr[i]; j < ptr[i+1]; j++){
          std::array<std::size_t,2> index = {i,idx[j]};
          m_data.emplace(index, val[j]);
      }
    }
  }
  else{
    for(size_t j = 0; j < m_m; j++){
      for(size_t i = ptr[j]; j < ptr[j+1]; j++){
          std::array<std::size_t,2> index = {idx[i],j};
          m_data.emplace(index, val[j]);
      }
    }
  }
  val.clear();
  idx.clear();
  ptr.clear();
  return;
}

template <class T, StorageOrder St>
std::vector<T> 
operator*(SparseMatrix<T,St> &M, const std::vector<T> &x)
{
  // Uncompressed
  if(!M.compressed){
    std::vector<T> y(M.m_n, 0);
    size_t i, j;
    for (const auto &pair: M.m_data){
      i = pair.first[0]; j = pair.first[1]; 
      y[i] += M.m_data[{i,j}]*x[j];
    }
    return y;
  }
  //StorageOrder::Row_wise and compressed
  if(St == StorageOrder::Row_wise){ 
    std::vector<T> y(M.m_n, 0);
    for (size_t row_idx = 0; row_idx < M.m_n; ++row_idx) {
      for (size_t col_idx = M.ptr[row_idx]; col_idx < M.ptr[row_idx + 1]; ++col_idx) {
        y[row_idx] += (x[M.idx[col_idx]] * M.val[col_idx]);
      }
    }
    return y;
  }
  //StorageOrder::Column_wise and compressed
  else{
    std::vector<T> y(M.m_n, 0);
    for (int col_idx = 0; col_idx < M.m_m; ++col_idx) {
      for (size_t row_idx = M.ptr[col_idx]; row_idx < M.ptr[col_idx + 1]; ++row_idx) {
        y[M.idx[row_idx]] += (x[col_idx] * M.val[row_idx]);
      }
      return y;
    }
  }
}


template class SparseMatrix<double, StorageOrder::Row_wise>;
template std::vector<double> operator*(SparseMatrix<double, StorageOrder::Row_wise> &M, const std::vector<double> &x);
template std::ostream &operator<<(std::ostream &stream, SparseMatrix<double, StorageOrder::Row_wise> &M);
template class SparseMatrix<double, StorageOrder::Column_wise>;
template std::vector<double> operator*(SparseMatrix<double, StorageOrder::Column_wise> &M, const std::vector<double> &x);
template std::ostream &operator<<(std::ostream &stream, SparseMatrix<double, StorageOrder::Column_wise> &M);





