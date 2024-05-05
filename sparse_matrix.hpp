#ifndef SPARSE_MATRIX_HPP
#define SPARSE_MATRIX_HPP

#include <cassert>
#include <iomanip>
#include <iostream>
#include <map>
#include <vector>

enum class StorageOrder
{
  Row_wise,
  Column_wise
};

/// Templated class for sparse matrix.
template <class T, StorageOrder St>
class SparseMatrix {
public:
  /// Default constructor.
  SparseMatrix(const size_t n_, const size_t m_) 
  : m_n(n_) //number of rows 
  , m_m(m_) //number of columns
  , compressed(false) //boolean that says if its compressed
  , val()
  , idx()
  , ptr()
    {}
    
  SparseMatrix() 
  : m_n() //number of rows 
  , m_m() //number of columns
  , compressed(false) //boolean that says if its compressed
  , val()
  , idx()
  , ptr()
    {}
  
  inline bool is_compressed(){
    return compressed;
  }

  inline const T &operator[](std::array<std::size_t,2> index) const{
    if(!compressed){
      auto it = m_data.find(index); // Find the iterator for the key
      if (it != m_data.end())
        return m_data.at(index);
      else return 0;
    }
    else{
      size_t i = index[0], j = index[1];
      for(size_t k = ptr[i]; k < ptr[i+1]; k++){
        if(idx[k] == j) return val[k];
      }
      return 0;      
    }
  }

  inline T &operator[](std::array<std::size_t,2> index) {
    if(!compressed)
      return m_data[index];
    else{
      size_t i = index[0], j = index[1];
      for(size_t k = ptr[i]; k < ptr[i+1]; k++){
        if(idx[k] == j) return val[k];
      }
      throw std::out_of_range("You can't add new non zero elements in compressed mode");      
    }
  }
  
  inline auto upper_bound(std::array<std::size_t,2> u) const {
    return m_data.upper_bound(u);
  } 
  inline auto lower_bound(std::array<std::size_t,2> u) const {
    return m_data.lower_bound(u);
  } 

  inline void resize(size_t n, size_t m) {
    m_m = m;
    m_n = n;
  }

  /// Stream operator.
  template <class T1, StorageOrder St1>
  friend std::ostream &
  operator<<(std::ostream &stream, SparseMatrix<T1, St1> &M);

  ///Reader of matrices in matrix market format

  template <class T1, StorageOrder St1>
  friend void
  readmatrix(const std::string& file_name, SparseMatrix<T1, St1> &M);

  /// Extracting row/column function
  void
  line(SparseMatrix<T, St> &M, const size_t k);

  void compress();

  void uncompress();

  template <class T1, StorageOrder St1>
  friend std::vector<T1> 
  operator*(SparseMatrix<T1,St1> &M, const std::vector<T1> &x);

  
struct CustomComparator {
  bool operator() (const std::array<std::size_t,2>& a, const std::array<std::size_t,2>& b) const {
      if(St == StorageOrder::Row_wise) 
       return a < b; // Sort in row-wise
      else{
        if(a[1] != b[1]) return a[1] < b[1];
        else return a[0] < b[0];
      }
  }
};

private:
  std::map<std::array<std::size_t,2>,T,CustomComparator> m_data;
  size_t m_n;
  size_t m_m;
  bool compressed;
  std::vector<T> val;
  std::vector<int> idx;
  std::vector<int> ptr;
};

#endif /* SPARSE_MATRIX_HPP */
