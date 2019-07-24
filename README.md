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
```
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

```
std::ofstream f1("file1.txt") // ok
std::ofstream f1 = "file1.txt" // error
```

### Initializer lists
`std::initializer_list<T>`is a type from the standard library which can be useful while initializing complex classes.

* Use `copy` from the std as much as you can. Stop using `for`loops.
* While overloading constructor, the compiler will prefer the empty constructor over the initializer_list constructor when taking zero arguments.

Warning: there are some caveats to this. If there are two constructors for a class like this:
```
X(int a)
X(std::initializer_list<int> list)
```

To use `X(int a)` you **must** use the `X whatever(9)`to use it. If you use `X whatever {9}` you will call the constructor with the initializer_list. **This happens in the standard library with the std::vector**.
```
std::vector(9) // creates a vector with 9 elements initialized to 0
std::vector{3} // creates a vector with 1 element initialized to 3
```

## Compile-time assertions
In C++11 they introduced static assers, which are "normal" asserts that *while compiling*, they trigger itselfs to inform you.

The static asserts are specially usefull in combination with templates in generic programming.

## Null pointer
In C++03 the null pointer constant is **0**. So this code:
```
void f (int a);
void f(char* ptr);
```
sucks:
```
f(0); // invokes the first function
f(static_cast<char*>(0)) // invokes the second function
```

Now, we have the **NULL** "pointer constant", which in reality is a **macro**:
* NULL == static_cast<int>(0)

So now in C++11 we have the keyword **nullptr**.
```
char* ptr = nullptr; // ok
int n = nullptr; // error

int z = 0;
if (pc == 0) { z = 1; } // ok
if (pc == nullptr) { z = 1; } // ok
```

### Generic Code
This wont compile:
```
template (typename T)
void f (T *t)

f(nullptr); // Error T = ?
```
But this does:
```
f(static_cast<double *>(nullptr));
```

That is because the *nullptr* has a type of it's own.

## Type inference
You know that the STL sucks... but for that there's a solution: `auto`. While using the keyword `auto`, the compiler should infer the variable's type.

```
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
```
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
```
address;
new (address) class;
```

## Move semantics
The compiler has two magic weapons: *constant folding* and *inlining*. You are not smarter than the compiler and it's tricky optimizations. Period.

Given how C/C++ works with larger objects, there was a trend to void *pass-by-value* and *return-by-value*, so it transformed into *pass-by-reference* and *return-by-reference*.

So now we have the following situation: we don't want to copy, but passing only references is also a bit ugly. So we have something "better"! C++11 added the idea of **move**.

```
sample s{1000};
sample t{std::move(s)}; // s is empty and now t has the content
s = std::move(t); // t is empty and now s has the content
```

Just like Rust! But wait, there are some caveats... because `move` doesn't *move*!!! it's the combination between `move` and `=`.

```
using name_list = std::vector<std::string>;

name_list generate_names();

void f() {
    name_list list;
    list = generate_names(); // This is a 'copy' operation in C++03, but it's a 'move' operation in C++11
}
```
Now more interesting:
```
std::string get_name() {
    std::string name{"Julio"};
    name += " Iglesias"
    return name;
}
```

Because `name` is a temporal variable and will be destroyed outside the function, C++11 will *move* the value of `name` instead of copying it.

Good news! Most of the STL is `move`compatible:
```
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

```
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
```
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
```
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
```
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