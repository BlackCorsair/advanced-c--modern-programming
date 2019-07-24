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
