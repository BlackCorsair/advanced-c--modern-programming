# Advanced C++ modern programming
## Purpose
This repo contains all the code and notes for the UC3M *Advanced C++ Modern Programming* course.

## Notes
### What is C++?
* If your application is slow, quit your dynamic polimorphism.
* Metaprogramming does not solve the 90% of the use cases.
* You can do Functional Programming.
* Memory leaks -> C++11 use smart pointers so you'll rarely see memory leaks. Daniel usually does a `grep "new"` (to delete them) so there are no memory leaks.
* Buffer overflows -> use a good static analyzer.
* Macros should be deleted from existence.
* Pragmas, not even once. This is 99% non useful black magic.
* Variadic Macros... vade retro Satan.


## How does it looks like?
* Uniform initiation
`std::vector<int> v {1,2,3,4,5};`
* Range based loop

```cpp
for(auto& x: v) {
    std::cout << x;
}
```

## Uniform Initialization in C++
In C++98 the initilization wasn't uniform, so in C++11 they changed it so it was uniform, easier to read, understand and write.

In one sentence: **all is initialized with `{}`**.

This is the only way to go *generic*.

### When can I use '='

To initialize a variable with copy-initialization (as oposed to direct-initialiation).

### Uniform and explicit initialization
* Uniform initialization with direct initialization allows invoking *explicit* constructor.
* However, using *copy initialization* **does not allow** using explicit constructors.

```cpp
std::ofstream f1("file1.txt") // ok
std::ofstream f1 = "file1.txt" // error
```

### Initializer lists
`std::initializer_list<T>`is a type from the standard library which can be useful while initializing complex classes.

* Use `copy` from the std as much as you can. Stop using `for`loops.
* While overloading constructor, the compiler will prefer the empty constructor over the initializer_list constructor when taking zero arguments.

Warning: there are some caveats to this. If there are two constructors for a class like this:
```cpp
X(int a)
X(std::initializer_list<int> list)
```

To use `X(int a)` you **must** use the `X whatever(9)`to use it. If you use `X whatever {9}` you will call the constructor with the initializer_list. **This happens in the standard library with the std::vector**.
```cpp
std::vector(9) // creates a vector with 9 elements initialized to 0
std::vector{3} // creates a vector with 1 element initialized to 3
```

## Compile-time assertions
In C++11 they introduced static assers, which are "normal" asserts that *while compiling*, they trigger itselfs to inform you.

The static asserts are specially usefull in combination with templates in generic programming.

## Null pointer
In C++03 the null pointer constant is **0**. So this code:
```cpp
void f (int a);
void f(char* ptr);
```
sucks:
```cpp
f(0); // invokes the first function
f(static_cast<char*>(0)) // invokes the second function
```

Now, we have the **NULL** "pointer constant", which in reality is a **macro**:
* NULL == static_cast<int>(0)

So now in C++11 we have the keyword **nullptr**.
```cpp
char* ptr = nullptr; // ok
int n = nullptr; // error

int z = 0;
if (pc == 0) { z = 1; } // ok
if (pc == nullptr) { z = 1; } // ok
```

### Generic Code
This wont compile:
```cpp
template (typename T)
void f (T *t)

f(nullptr); // Error T = ?
```
But this does:
```cpp
f(static_cast<double *>(nullptr));
```

That is because the *nullptr* has a type of it's own.

## Type inference
You know that the STL sucks... but for that there's a solution: `auto`. While using the keyword `auto`, the compiler should infer the variable's type.

```cpp
std::vector<int> v;
auto i = v.begin() // typename std::vector<T>::const_iterator i = v.begin()
```

## Range based for loops
There are two ways to iterate over a container:
* `x.begin()` & `x.end()`
* `begin(x)` & `end(x)`

Between the two, the first one is the preferred.

## Aligment
Some low level tricks:
```cpp
class particle { /∗...∗/ };
typedef std::aligned_storage<sizeof(particle),alignof(particle)>::type buffer;

buffer p1;
new (&p1) particle;
```
This code:
1. Create a buffer type *aligned* to the size of the class *particle*.
2. Creates a buffer (p1).
3. Uses `new` to assign the newly created particle to the *buffer p1* address.

*Wait, what?*

You see, the `new` statement in C++ does the following:
1. Allocates memory for the *soon to be created* object. Like a `malloc` on vitamines.
2. Invokes the constructor function of the object and assigns the newly created object to the memory addres allocated beforehand.

The previous code exploit the second part of the `new` statement, by declaring the memory address where the object should be declared *between* the `new` statement and the object class.
```cpp
address;
new (address) class;
```

## Move semantics
The compiler has two magic weapons: *constant folding* and *inlining*. You are not smarter than the compiler and it's tricky optimizations. Period.

Given how C/C++ works with larger objects, there was a trend to void *pass-by-value* and *return-by-value*, so it transformed into *pass-by-reference* and *return-by-reference*.

So now we have the following situation: we don't want to copy, but passing only references is also a bit ugly. So we have something "better"! C++11 added the idea of **move**.

```cpp
sample s{1000};
sample t{std::move(s)}; // s is empty and now t has the content
s = std::move(t); // t is empty and now s has the content
```

Just like Rust! But wait, there are some caveats... because `move` doesn't *move*!!! it's the combination between `move` and `=`.

```cpp
using name_list = std::vector<std::string>;

name_list generate_names();

void f() {
    name_list list;
    list = generate_names(); // This is a 'copy' operation in C++03, but it's a 'move' operation in C++11
}
```
Now more interesting:
```cpp
std::string get_name() {
    std::string name{"Julio"};
    name += " Iglesias"
    return name;
}
```

Because `name` is a temporal variable and will be destroyed outside the function, C++11 will *move* the value of `name` instead of copying it.

Good news! Most of the STL is `move`compatible:
```cpp
using wordlist = std::vector<std::string>;

wordlist read(std::string filename);

void f() { wordlist wordlist wordlist wordlist wordlist wordlist
    w1; // Empty word list
    w2{w1}; // Copy
    w3 = w1; // Copy w4{read("quijote.txt")}; // Move w5 = read("quijote.txt"); // Move w6 = std::move(w4); // Move
    w1 = w4; // Copy
    w1 = std::move(w4); // Move 
}
```

**Bad news**: by default, the user defined classes are not `move` compatible. To make your class `move` compatible, you'll need a *move constructor*:

A move operation shoud:
* Release resources hold by target.
* Take ownership of resources hold by source.
* Leave source in a valid state.

```cpp
// declaration
class sample {
    public:
        // copy
        sample & operator=(const sample & s);

        // move
        sample & operator=(sample && s);
    private:
        int size_;
        double ∗ vec_;
}
// implementation
sample::sample(sample && s) : size_{s.size}, vec_{s.vec_} {
    s.size_ = 0;
    s.vec_ = nullptr;
}
```
Use:
```cpp
sample read(std::string filename);

void f() {
    sample s1; // Empty sample
    sample s2{read("marks.txt")}; // Move
    s1 = s2; // Copy
    s1 = std::move(s2); // Move
}
```

## Forwarding

R-Value and L-Value Black Magic Fuckery:
```cpp
bool is_temporary(int const & x) { return false; } 
bool is_temporary(int && x) { return true; }

template <typename T> 
void print(T && x) {
    if (is_temporary(x)) std::cout << "Temporary\n";
    else std::cout << "Not temporary\n";
}
void f() {
    print(5); // Prints Not temporary int x = 42;
    print(x); // Prints Not temporary
}
```

*Hold up a minute...*
```cpp
bool is_temporary(int const & x) { return false; } 
bool is_temporary(int && x) { return true; }

template <typename T> 
void print(T && x) {
    if (is_temporary(std::forward<T>(x))) std::cout << "Temporary\n";
    else std::cout << "Not temporary\n";
}
void f() {
    print(5); // Prints Not temporary int x = 42;
    print(x); // Prints Not temporary
}
```

Conclusion: std::forward is black magic and playing with r/l-values is confusing and hurts.

## Exception Safety

If you mark your function as **noexcept** you are stting that the code is not ready to handle exceptions.
```
template <typename T>
T suma(T x, T y) noexcept {
    return x+y;
}
```

### noexcept violations
* Constructor of std::vector might throw if memory allocation fails.
* std::vector::at() might throw if access is out of range.

```cpp
int cube(int x) {
    return x*x*x;
}

std::vector<int> apply_cube (const std::vector<int> &v) noexcept {
    std::vector<int> r(v.size()); // might throw exception
    for(size_t i = 0; i < v.size(); ++i) {
        r.at(i) = cube(v.at(i)); // might throw exception
    }
    return r;
}
```

To "fix" it:
```cpp
bool cond = noexcept(v.at(0));
void f(std::vector<int> & v) noexcept(noexcept(v.at(0)));
```

* A destructor should not throw exceptions. Because of this, all destructors in STL are **noexcept**.
* Move operations should not throw exceptions.

## Compile time constants: constexpr

* Expressions evaluated at compile time (pdf had an errata) -> **constexpr**
* Values that can't be modified (immutable) -> **const**

### Why would I want to use this feature?
**PERFORMANCE BIATCH! (batman-bitchslap-meme)**

**Why the hell should you use C++ in the first place? Because you want that sweet, sweet performance and flexibility.** *But rust...* **RUST IS NOT ISO, IT DOESN'T HAS A STANDARD AND IS CONTROLLED BY AN ENTITY!** *ok, ok... jeez...*.

So why do we want to use compile-time expresions? To calculate things we want ahead of time. Supose that you need to calculate or initialize something every time and that this calculation costs you 2ms because it access the system memory, some hundred of CPU cycles, etc... You can do that *in advance* even before the code runs the first time.

## Non Static Data Member initialization

* Extends static data members to non static data members
* Allows to specify a default value for a data member in the class declaration.

```cpp
class position { 
    private:
        int x {1};
        int y {1}; 
};
// the same... but different
class position { 
    private:
        int x;
        int y;
    public:
        position() : x{1}, y{1} {} 
};
```

## Inline namespaces

The use of *inline namespaces* allows you to omit part of the namespace name in favor of readiness. For example:
```cpp
std::V1::cout
std::V2::coud

// now if V1 has inline
std::cout // it's the V1 cout
std::V2::cout // it's the V2 cout
```

## decltype and declval
* **decltype**
    * Gets the type of a declaration
    * Useful complement to auto
    * Specially useful in *generic programming*

```cpp
T x;
auto t = x;

U y;
auto z = y;

auto a = x + y; // a is the same type as (x + y)
decltype(x + y) b; // b is the same type as (x + y)
b = x + y; // now b has (x + y)
```

The previous example wasn't so much, but here is more:
```cpp
template <class T, class U>
auto add(const std::vector<T> & v1, const std::vector<U> & v2)
            −> std::vector<decltype(T{}+U{})> {
    
    std::vector<decltype(v1[0]+v2[0])> r; auto i=v1.begin();
    auto j=v2.begin();
    auto end = v1.end();
    for (;i!=end;++i, ++j) { 
        r.push_back((∗i)+(∗j));
    }
    return r; 
}
```

Here we return a vector of (T + U). *But why not declaring the type before where you say 'auto'?*. Because in that point, we still don't know T nor U.


## Lambda expressions

Useful when you want to declare a function, but you only use it only once. Mostly, when you do some value transformation using the STL.
```cpp
// C++98
struc square {
    double operator()(double x) {return x*x};
}
...
void f(const std::vector<double> & v, std::vector<double> w) {
    std::transform(v.begin(), v.end(), std::back_inserter(w), square());
}
```

Now:
```cpp
// C++11
void f(const std::vector<double< & v, std::vector<double> & w) { 
    std::transform(v.begin(), v.end(), std::back_inserter(w), [](double x) { return x*x; });
    // [](double x) { return x*x; } would be the lambda
}
```

Some examples:
```cpp
double s = 0.0;

void f(int i) { 
    char c = ’a’; static int n = 0;
    vector<int> v { 1, 2, 3}; 
    for_each(v.begin(), v.end(), [](int x) { cout << x << "\n"; }); // OK for_each(v.begin(), v.end(),
    for_each(v.begin(), v.end(), [](int x) { cout << s << "\n"; }); // OK. s is global for_each(v.begin(), v.end(),
    for_each(v.begin(), v.end(), [](int x) { cout << i << "\n"; }); // Error. i non accessible for_each(v.begin(), v.end(),
    for_each(v.begin(), v.end(), [](int x) { cout << c << "\n"; }); // Error. c non accessible for_each(v.begin(), v.end(),
    for_each(v.begin(), v.end(), [](int x) { cout << n << "\n"; }); // OK. n is static 
}
```

To access the local variables, you'll need to specify the `=` operator in the squares.
```cpp
 for_each(v.begin(), v.end(), [=](int x) { cout << c << "\n"; }); // c is now accessible for_each(v.begin(), v.end(),
```

To access specific variables, you'll need to pass them in the squares:
```cpp
int c {0};
for_each(v.begin(), v.end(), [c](int x) { cout << c << "\n"; }); // c is now accessible for_each(v.begin(), v.end(),
```

## Function wrappers

This is some black magic of C++: with lambdas, we cannot do recursion... until *now* that we can use `std::function`:
```cpp
std::function<int(int)> factorial = [&](int x) { return (x==1) ? 1 : (x ∗ factorial(x−1)); };
```

## Variadic Templates

```cpp
// Classes
template <typename ... Args>
class tuple;

// Functions
void print (const char *s);

template <typename T, typename ... Args>
void print(const char * s, T v, Args ... args); // Args: template parameter pack; args: function argument pack

print("Hi %: you were born %−%−% in %.", "Daniel", 4, 9, 1969, "Spain");
```

## Primitive literals and user defined literals

* Literal operator: transforms a literal with a given suffix into a value of a specific type:

```cpp
operato"" suffix
```

* A *literal operator* may be calculated at **compile time**

```cpp
constexpr complex<double> operator"" _i(long double imag)
{
    return {0.imag};
}

constexpr complex<double> x1 {5.0_i};
complex<double> x2 {2 + 3.6_i}
```

Some examples:
```cpp
class big_number {
public:
    big_number(const char * c);
};

big_number operator""_bn(const char * c)
{
    return big_number{c};
}

void f(big_number x);

f(13298374293740276058736958764958173465_bn);
```

## Strict ownership

* Unique pointers:
    * Simplify management of data members that traditionally were raw pointers
    * If all the pointers in a class are unique pointers, I don't need to specify a destructor. Only the *copy* operations.

```cpp
class base {
    public:
        virtual ~base();
        virtual void f();
};

class derived: public base {
    public:
        virtual void f();
};

void g() {
    unique_ptr<derived> pd{new derived}; 
    unique_ptr<base> pb{pd}; // Error. Cannot copy unique_ptr<base> pb2{std::move(pd)}; // OK
    pb2−>f(); // Virtual call
    // ...
}
```

If you only use *unique pointer* and never use *new* (so you'll never use delete), probably you'll never have to worry about memory leaks.

`make_unique` was forgotten to add in c++11, but the fix is simple:

```cpp
template <typename T, typename ... Args>
unique_ptr<T> make_unique(Args && ... args) {
    return unique_ptr<T>{new T{std::forward<Args>(args)...}}; 
}

auto p = std::make_unique<record>(id, name);
```

## Tuples

As with the pairs, do not overuse them, if you do, maybe you need a `struct` instead of the `tuple`.

Some pythonic examples:
```cpp
std::tuple<int, int> divide(int x, int y) {
    return std::make_tuple(x/y, x%y);
}

int quotient, remainder;
std::tie(quotient, remainder) = divide(10,3);
```

The *tuples* allows us to return more than one value. Tuples can be compared with `==` without problems.

* Elements in a tuple can be accesed by type, but if there are more than one variables with the same type, it wont compile.
```cpp
std::tuple<string, string, int> a { "Daniel", "Garcia", 1969 }; 
auto x = std::get<2>(a); // int x = 1969
auto y = std::get<int>(a); // int x = 1969
auot z = std::get<string>(a) // Error.
```

## Time (chrono) library

In c++1 we have now the *namespace* `std::chrono`.

Example of a timepoint:
```cpp
#include <chrono>
using std;
using chrono;
auto t1 = steady_clock::now();
// ...
auto t2 = steady_clock::now();
auto diff = t2 - t1;
cout << duration_cast<milliseconds>(diff).count() << "\n";
```

## Containers

* Move semantics are supported in all containers
    * SUSPECT of any `std::move` operations you do, because probably you can do that without them (unless you are using it on a *constructor*)
    * Use the move semantics as much as you can, because it will be *so much* cheaper than copying the values over and over again.
* Improved support for const_iterator
* `vector::data()` acess to the underlying array
* immutability in elements in `set` and `multiset`
* `push_back`? nah, better the `emplace_back`. It will accelerate the application so much that you'll notice it.
* with vectors, sometimes your *capacity* is a lot bigger than your actual size, so in that cases you can use the `shrink_to_fit` so the extra capacity is freed.

# Concurrency

## Threads
Since C++11, the language itself has now a *std::thread* solution independent of any OS. This implies:

* A new memory model
* `thread_local` variables
* *Atomic* types
* Concurrency portable abstractions
    * thread
    * mutex
    * condition_variable
    * lock_guard, unique_lock
    * promise, future, packaged_task

```cpp
void f1();

void g() {
    std::thread t {f1}; // starts a thread executing f1
    other_task();
    t.join(); // wait for the thread to finish
}
```

Simplified argument parsing:
```cpp
void f1(int x);

struct f1 {
    f2(int px) : x{px} {} 
    void operator()(); 
    int x;
}

void g() {
    std::thread t1{f1, 10}; // Run f1(10)
    std::thread t2{f2{20}}; // Run f2{20}.operator()() 
    std::thread t3{ [] { f1(42); } }; // Run lambda 
    t1.join();
    t2.join();
    t3.join();
}
```

## Shared objects access

What to do when you have multiple threads messing around with your variables and making your program "unstable"?

```
[MUTEX]: has entered the room.
[SPEED GAINED BY THE THREADS]: has exited the room
```

But if you really have to use them, please use `lock_guard`:
```cpp
std::mutex m;
int x = 0;

void f() {
    std::lock_guard<std::mutex> l{m}; // Acquires the lock
    ++x;
} // releases the lock

void g() {
    using namespace std; 
    thread t1(f); 
    thread t2(f); 
    t1.join(); 
    t2.join();
    cout << x << endl;
}
```
