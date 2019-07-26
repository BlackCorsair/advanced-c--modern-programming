#include <iostream>

template <class T, int N>
class fixed_vector {
 public:
    const T * begin() const {return array;}
    const T * end() const {return array + N;}

    fixed_vector(std::initializer_list<T> list) {
        std::copy(list.begin(), list.end(), array);
    }
    fixed_vector() {}
 private:
    T array[N];
};

// have to run on this
template <typename T, int N>
void print(const fixed_vector<T, N> & v) {
    for (auto const & x : v) {
        std::cout << x << " ";
    }
    std::cout << "\n";
}

int main() {
    fixed_vector<double, 3> v;
    print(v);
    fixed_vector<int, 4> w{1, 2, 3, 4};
    print(w);
}
