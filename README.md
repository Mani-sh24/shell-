# Custom C++ Shell

Welcome to my custom shell implementation! This project is a lightweight command-line interface built from scratch using C++.

## 🚀 Introduction

This shell was created as a personal project to learn more about system programming, process management, and how shells work under the hood. It serves as a practical exercise to improve my C++ skills and understand the intricacies of command parsing and execution.

## ✨ Features

Currently, the shell supports the following functionalities:

- **Built-in Commands**:
  - `exit`: Terminate the shell session.
  - `echo [args...]`: Display a line of text.
  - `type [command]`: Identify whether a command is a shell builtin or an executable file, and display its location.
  - `pwd`: Print the current working directory.
  - `$PATH`: List all directories in the system's PATH.
  - `history`: List all previous commands used.
  - `cd`: changing and navigating directories.
- **External Command Execution**:
  - Run any executable available in your system's PATH (e.g., `ls`, `cat`, `git`).

## 🛠️ Building and Running

To build the project, ensure you have a C++ compiler (supporting C++17 or later).

1.  **Compile the code**:
    ```bash
    g++ -std=c++17 src/main.cpp src/tools.cpp -o shell
    ```

2.  **Run the shell**:
    ```bash
    ./shell
    ```

## 📚 Learning Goals

This project is a playground for:
- String manipulation and tokenization and parsing in C++.
- Understanding process creation and execution.
- Learning about linux.
