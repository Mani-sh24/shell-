#include "tools.hpp"
#include <sys/wait.h>
#include <cstdlib> // getenv
#include <readline/readline.h>
#include <readline/history.h>
#include <fstream>
std::string input;
std::vector<std::string> input_tok;
vector<string> history_buffer;
vector<string> split_path()
{
  vector<string> paths;
  string path = getenv("PATH");
  int n = path.size();
  string current;
  for (int i = 0; i < n; i++)
  {
    if (path[i] == ':')
    {
      paths.push_back(current);
      current.clear();
    }
    else
    {
      current.push_back(path[i]);
    }
  }
  if (!current.empty())
  {
    paths.push_back(current);
  }

  return paths;
}

void raise_command_error(string input)
{
  std::cout << input << ": command not found\n";
}

int type_checker(string input)
{
  vector<string> types = {"echo", "exit", "type", "pwd", "history"};
  for (string i : types)
  {
    if (input.compare(i) == 0)
    {
      return 1; // for builtin;
    }
  }
  return 0; // for external;
}

std::vector<std::string> tokenise_u(const std::string &s)
{
  std::vector<std::string> words;
  int n = s.size();
  int i = 0;

  while (i < n)
  {
    while (i < n && s[i] == ' ')
      i++;
    int start = i;
    while (i < n && s[i] != ' ')
      i++;
    if (start < i)
    {
      std::string word;
      for (int j = start; j < i; j++)
      {
        word.push_back(s[j]);
      }
      words.push_back(word);
    }
  }

  return words;
}

string locater(const string &input)
{
  if (input.empty())
    // return "No file provided\n";
    return {};
  vector<string> paths = split_path();

  for (const auto &p : paths)
  {
    if (check_exec_exists(p, input))
    {
      return p + "/" + input;
    }
  }

  return {};
}

bool check_exec_exists(const fs::path &parent,
                       const fs::path &child)
{
  fs::path full = parent / child;

  return fs::exists(full) &&
         fs::is_regular_file(full) &&
         access(full.c_str(), X_OK) == 0;
}

int execute_command(vector<string> input)
{
  // vector<string> input_list = tokenise_u(input);
  std::vector<char *> argv;
  for (int i = 0; i < input.size(); i++)
    argv.push_back(const_cast<char *>(input[i].c_str()));

  argv.push_back(nullptr); // VERY IMPORTANT
  string res = locater(input[0]);
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid < 0)
  {
    perror("Error forking: ");
    return 0;
  }
  else if (pid == 0)
  {

    if (execv(res.c_str(), argv.data()) == -1)
    {
      // return 0; // failed // this is wrong and breaks the unix law the child never returns to the parent or caller
      _exit(EXIT_FAILURE);
    }
  }
  else
  {
    do
    {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    if (WIFEXITED(status))
    {
      if (WEXITSTATUS(status) != EXIT_SUCCESS)
        return 0; // command failed
    }
  }
  return 1; // command succeeded
}

string get_pwd()
{
  fs::path current_dir = fs::current_path();
  string current_dir_str = current_dir.string();
  return current_dir_str;
}

int change_dir(string path)
{
  if (path.empty())
  {
    return 1; // No path given
  }
  if (path.compare("~") == 0)
  {
    path = getenv("HOME");
  }

  if (chdir(path.c_str()) == -1)
  {
    return -1; // error no file found
  }
  return 0; // success
}
void push_history()
{
  // string flattened_input_tok;
  // for (auto &i : input_tok)
  // {
  //   flattened_input_tok += " " + i;
  // }
  // history_buffer.push_back(flattened_input_tok);
  // add_history(flattened_input_tok.c_str());
  if (input.empty())
    return;

  history_buffer.push_back(input);
  add_history(input.c_str());
}

size_t history_written = 0;
void print_full_history()
{
  int idx = 1;
  for (const string &h : history_buffer)
    cout << idx++ << " " << h << endl;
}
bool history_write(const string &path)
{
  ofstream file(path);
  if (!file.is_open())
    return false;

  for (const string &h : history_buffer)
    file << h << '\n';

  history_written = history_buffer.size();
  return true;
}
bool history_append(const string &path)
{
  ofstream file(path, ios::app);
  if (!file.is_open())
    return false;

  for (size_t i = history_written; i < history_buffer.size(); i++)
    file << history_buffer[i] << '\n';

  history_written = history_buffer.size();
  return true;
}
bool history_read(const string &path)
{
  ifstream file(path);
  if (!file.is_open())
    return false;

  string line;
  while (getline(file, line))
  {
    if (line.empty())
      continue;

    history_buffer.push_back(line);
    add_history(line.c_str());
  }

  history_written = history_buffer.size();
  return true;
}
bool print_last_n_history(const string &arg)
{
  try
  {
    int n = stoi(arg);
    int start = max(0, (int)history_buffer.size() - n);

    for (int i = start; i < history_buffer.size(); i++)
      cout << (i + 1) << " " << history_buffer[i] << endl;

    return true;
  }
  catch (...)
  {
    return false;
  }
}
void list_history()
{
  if (input_tok.empty())
    return;

  // plain: history
  if (input_tok.size() == 1)
  {
    print_full_history();
  }
  // history -w FILE
  else if (input_tok[1] == "-w")
  {
    if (input_tok.size() < 3 || !history_write(input_tok[2]))
      cerr << "history: cannot write file\n";
  }
  // history -a FILE
  else if (input_tok[1] == "-a")
  {
    if (input_tok.size() < 3 || !history_append(input_tok[2]))
      cerr << "history: cannot append file\n";
  }
  // history -r FILE
  else if (input_tok[1] == "-r")
  {
    if (input_tok.size() < 3 || !history_read(input_tok[2]))
      cerr << "history: cannot read file\n";
  }
  // history N
  else
  {
    if (!print_last_n_history(input_tok[1]))
      cerr << "history: invalid argument\n";
  }
}
const char *hist_env = getenv("HISTFILE");
void load_history_from_env()
{

  if (!hist_env)
    return;

  ifstream file(hist_env);
  if (!file.is_open())
    return;

  string line;
  while (getline(file, line))
  {
    if (line.empty())
      continue;

    history_buffer.push_back(line);
    add_history(line.c_str());
  }

  history_written = history_buffer.size();
}

void save_on_exit()
{
  const char *hist_env = getenv("HISTFILE");
  if (!hist_env)
    return;

  history_append(hist_env);
}