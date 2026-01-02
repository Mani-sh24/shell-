#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <filesystem>
namespace fs = std::filesystem;
using namespace std;

extern std::string input;
extern std::vector<std::string> input_tok;
extern vector<string> history_buffer;


vector<string> split_path();
void raise_command_error(string input);
std::vector<std::string> tokenise_u(const std::string &s);
int type_checker(string input);
string locater(const string &input);
bool check_exec_exists(const fs::path &parent, const fs::path &child);
int execute_command(vector<string> input);
string get_pwd();
int change_dir(string path);
void push_history();
void list_history();
void load_history_from_env();
bool history_append(const string &path);
void save_on_exit();