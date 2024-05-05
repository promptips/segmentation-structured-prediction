# Acknowledgment #

This code was developed by @promptips, the new owner of this repository.

Contributors:

- Yunpeng Li
- Pol Monsó Purtí

# How to compile the code #

This tutorial will help you compile the code and run it on your own system. The tutorial is intended for Linux users but Windows or Mac users should also be able to compile the code.

Instructions:

Copy the source code.
Ensure cmaketo is installed (it's a tool to create makefiles).
Compile third-party libraries for your respective systems. Either UNIX or WINDOWS.

Main code:

Edit variables in the "THIRD-PARTY LIBRARIES" section in the `CMakeLists_common.txt`.
Go to the `superpixels_public` directory and compile based on your system (WINDOWS or UNIX).

Edit the `config.txt` configuration file that contains all the parameters needed for training or applying the algorithm to a new dataset.

## Additional information for developers ##

- Multithreading

For ssvm, edit `svm_struct_globals.h` and change `NTHREADS`.