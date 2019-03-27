#include <cstddef>
#include <memory>

template <typename T>
struct VectorBase {
    T* data = nullptr;
    size_t cp = 0;

    VectorBase() = default;

    VectorBase(size_t n) {
        data = reinterpret_cast<T*>(new char[n * sizeof(T)]);
        cp = n;
    }

    ~VectorBase() {
        delete[] reinterpret_cast<char*>(data);
    }
};

template <typename T>
class Vector {
private:
    size_t sz = 0;
    VectorBase<T> base;

public:
    Vector() {}

    Vector(size_t n): base(n) {
        std::uninitialized_value_construct_n(base.data, n);
        sz = n;
    }

    Vector(const Vector& other): base(other.sz) {
        std::uninitialized_copy_n(other.base.data, other.sz, base.data);
        sz = other.sz;
    }

    Vector<T> operator= (const Vector<T>& other) {
        // not effective
        for (size_t i = 0; i != sz; ++i) {
            base.data[i].~T();
        }
        delete[] reinterpret_cast<char*>(base.data);
        base.data = reinterpret_cast<T*>(new char[other.sz * sizeof(T)]);
        std::uninitialized_copy_n(other.base.data, other.sz, base.data);
        base.cp = other.sz;
        sz = other.sz;
    }
 
    void reserve(size_t n) {
        if (n > base.cp) {
            T* NewData = reinterpret_cast<T*>(new char[n * sizeof(T)]);
            std::uninitialized_copy_n(base.data, sz, NewData);
            for (size_t i = 0; i != sz; ++i) {
                base.data[i].~T();
            }
            delete[] reinterpret_cast<char*>(base.data);
            base.data = NewData;
            base.cp = n;
        }
    }

    void resize(size_t n) {
        if (n > sz) {
            if (n > base.cp) {
                T* NewData = reinterpret_cast<T*>(new char[n * sizeof(T)]);
                std::uninitialized_copy_n(base.data, sz, NewData);
                std::uninitialized_value_construct_n(NewData + sz, n - sz);
                for (size_t i = 0; i != sz; ++i) {
                    base.data[i].~T();
                }
                delete[] reinterpret_cast<char*>(base.data);
                base.data = NewData;
                base.cp = n;
            } else {
                std::uninitialized_value_construct_n(base.data + sz, n - sz);
            }
        } else if (n < sz) {
            for (size_t i = sz-1; i >= n; --i) {
                base.data[i].~T();
            }
        }
        sz = n;
    } 
    
    void push_back(const T& elem) {
        if (sz == base.cp) {
            if (base.cp == 0) {
                reserve(1);
            } else {
                reserve(2 * base.cp);
            }
        }
        new(base.data + sz)T(elem);
        ++sz;
    }

    void push_back(T&& elem) {
        if (sz == base.cp) {
            if (base.cp == 0) {
                base.data = reinterpret_cast<T*>(new char[sizeof(T)]);
                base.cp = 1; 
            } else {
                reserve(2 * base.cp);
            }
        }
        new(base.data + sz)T(std::move(elem));
        ++sz;
    }

    void pop_back() {
        if (sz > 0) {
            base.data[sz - 1].~T();
            --sz;
        }
    }

    size_t size() const {
        return sz;
    }

    size_t capacity() const {
        return base.cp;
    }

    void clear() {
        for (size_t i = 0; i != sz; ++i) {
            base.data[i].~T();
        }
        sz = 0;
    }

    void swap(Vector<T>& other) {
        std::swap(base, other.base);
        std::swap(sz, other.sz);
    }

    T* begin() {
        return base.data;
    }

    T* end() {
        return base.data + sz;
    }

    T& operator[](size_t i) {
        return base.data[i];
    }
    
    const T& operator[](size_t i) const {
        return base.data[i];
    }

    ~Vector() {
        for (size_t i = 0; i != sz; ++i) {
            base.data[i].~T();
        }
    }
};

