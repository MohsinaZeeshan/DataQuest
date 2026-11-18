// DynArray.h - Module A (Mohsina Zeeshan)
// 1D dynamic array (grows by doubling), 2D dynamic array and jagged array.
// Templates live in the header so any type can be used.
#ifndef DATAQUEST_DYNARRAY_H
#define DATAQUEST_DYNARRAY_H

#include <stdexcept>

// ============================================================
// 1D dynamic array
// pushBack: amortised O(1) (doubling)   get/set: O(1)
// insertAt / removeAt: O(n)             space: O(capacity)
// ============================================================
template <typename T>
class DynArray {
private:
    T* data;
    int count;      // elements in use
    int cap;        // allocated slots

    void grow() {
        int newCap = (cap == 0) ? 4 : cap * 2;   // doubling
        T* fresh = new T[newCap];
        for (int i = 0; i < count; i++) fresh[i] = data[i];
        delete[] data;
        data = fresh;
        cap = newCap;
    }

public:
    DynArray() : data(nullptr), count(0), cap(0) {}

    explicit DynArray(int initialCapacity) : data(nullptr), count(0), cap(0) {
        if (initialCapacity > 0) {
            data = new T[initialCapacity];
            cap = initialCapacity;
        }
    }

    DynArray(const DynArray& other) : data(nullptr), count(other.count), cap(other.cap) {
        if (cap > 0) {
            data = new T[cap];
            for (int i = 0; i < count; i++) data[i] = other.data[i];
        }
    }

    DynArray& operator=(const DynArray& other) {
        if (this == &other) return *this;
        T* fresh = (other.cap > 0) ? new T[other.cap] : nullptr;
        for (int i = 0; i < other.count; i++) fresh[i] = other.data[i];
        delete[] data;
        data = fresh;
        count = other.count;
        cap = other.cap;
        return *this;
    }

    ~DynArray() { delete[] data; }

    void pushBack(const T& value) {
        if (count == cap) grow();
        data[count++] = value;
    }

    void popBack() {
        if (count == 0) throw std::out_of_range("popBack on empty DynArray");
        count--;
    }

    void insertAt(int index, const T& value) {
        if (index < 0 || index > count) throw std::out_of_range("DynArray insert index");
        if (count == cap) grow();
        for (int i = count; i > index; i--) data[i] = data[i - 1];   // shift right
        data[index] = value;
        count++;
    }

    void removeAt(int index) {
        if (index < 0 || index >= count) throw std::out_of_range("DynArray remove index");
        for (int i = index; i < count - 1; i++) data[i] = data[i + 1]; // shift left
        count--;
    }

    T& operator[](int index) {
        if (index < 0 || index >= count) throw std::out_of_range("DynArray index");
        return data[index];
    }

    const T& operator[](int index) const {
        if (index < 0 || index >= count) throw std::out_of_range("DynArray index");
        return data[index];
    }

    T& back() { return (*this)[count - 1]; }

    int size() const { return count; }
    int capacity() const { return cap; }
    bool isEmpty() const { return count == 0; }
    void clear() { count = 0; }        // keeps memory for reuse

    // Raw pointer so Module B's sorts/searches can work on a plain array.
    T* rawData() { return data; }
    const T* rawData() const { return data; }
};

// ============================================================
// 2D dynamic array (rows x cols), e.g. the 20 x 20 dungeon grid
// at(r, c): O(1)   space: O(rows * cols)
// ============================================================
template <typename T>
class Array2D {
private:
    T** grid;
    int rows;
    int cols;

    void allocate(int r, int c, const T& fill) {
        rows = r;
        cols = c;
        grid = (r > 0) ? new T*[r] : nullptr;
        for (int i = 0; i < r; i++) {
            grid[i] = new T[c];
            for (int j = 0; j < c; j++) grid[i][j] = fill;
        }
    }

    void release() {
        for (int i = 0; i < rows; i++) delete[] grid[i];
        delete[] grid;
        grid = nullptr;
        rows = cols = 0;
    }

public:
    Array2D() : grid(nullptr), rows(0), cols(0) {}
    Array2D(int r, int c, const T& fill = T()) : grid(nullptr), rows(0), cols(0) { allocate(r, c, fill); }

    Array2D(const Array2D& other) : grid(nullptr), rows(0), cols(0) {
        allocate(other.rows, other.cols, T());
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++) grid[i][j] = other.grid[i][j];
    }

    Array2D& operator=(const Array2D& other) {
        if (this == &other) return *this;
        release();
        allocate(other.rows, other.cols, T());
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++) grid[i][j] = other.grid[i][j];
        return *this;
    }

    ~Array2D() { release(); }

    // Throws away the old contents and makes a new r x c grid.
    void resize(int r, int c, const T& fill = T()) {
        release();
        allocate(r, c, fill);
    }

    bool inBounds(int r, int c) const { return r >= 0 && r < rows && c >= 0 && c < cols; }

    T& at(int r, int c) {
        if (!inBounds(r, c)) throw std::out_of_range("Array2D index");
        return grid[r][c];
    }

    const T& at(int r, int c) const {
        if (!inBounds(r, c)) throw std::out_of_range("Array2D index");
        return grid[r][c];
    }

    void fill(const T& value) {
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++) grid[i][j] = value;
    }

    int getRows() const { return rows; }
    int getCols() const { return cols; }
};

// ============================================================
// Jagged array: each row can have a different length.
// Used for per-room item lists (row = room number).
// addToRow: amortised O(1)   removeFromRow: O(row length)
// ============================================================
template <typename T>
class JaggedArray {
private:
    DynArray<T>* rowsData;   // array of rows, each row grows on its own
    int rowCount;

public:
    explicit JaggedArray(int numRows = 0) : rowsData(nullptr), rowCount(numRows) {
        if (numRows > 0) rowsData = new DynArray<T>[numRows];
    }

    JaggedArray(const JaggedArray& other) : rowsData(nullptr), rowCount(other.rowCount) {
        if (rowCount > 0) {
            rowsData = new DynArray<T>[rowCount];
            for (int i = 0; i < rowCount; i++) rowsData[i] = other.rowsData[i];
        }
    }

    JaggedArray& operator=(const JaggedArray& other) {
        if (this == &other) return *this;
        delete[] rowsData;
        rowCount = other.rowCount;
        rowsData = (rowCount > 0) ? new DynArray<T>[rowCount] : nullptr;
        for (int i = 0; i < rowCount; i++) rowsData[i] = other.rowsData[i];
        return *this;
    }

    ~JaggedArray() { delete[] rowsData; }

    void reset(int numRows) {
        delete[] rowsData;
        rowCount = numRows;
        rowsData = (numRows > 0) ? new DynArray<T>[numRows] : nullptr;
    }

    void addToRow(int row, const T& value) { checkRow(row); rowsData[row].pushBack(value); }
    void removeFromRow(int row, int index) { checkRow(row); rowsData[row].removeAt(index); }
    T& get(int row, int index) { checkRow(row); return rowsData[row][index]; }
    const T& get(int row, int index) const { checkRow(row); return rowsData[row][index]; }
    int rowSize(int row) const { checkRow(row); return rowsData[row].size(); }
    int numRows() const { return rowCount; }

    // Returns the index of value in the row, or -1 (linear search).
    int findInRow(int row, const T& value) const {
        checkRow(row);
        for (int i = 0; i < rowsData[row].size(); i++)
            if (rowsData[row][i] == value) return i;
        return -1;
    }

private:
    void checkRow(int row) const {
        if (row < 0 || row >= rowCount) throw std::out_of_range("JaggedArray row");
    }
};

#endif
