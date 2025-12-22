#include "tools.hpp"
#include <sys/wait.h>
#include <unistd.h> // access()
#include <cstdlib>  // getenv
#include <vector>
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
  string flattened_input_tok;
  for (auto &i : input_tok)
  {
    flattened_input_tok += " " + i;
  }
  history_buffer.push_back(flattened_input_tok);
}

void list_history()
{
  int idx = 1;

  // Case 1: plain "history"
  if (input_tok.size() == 1)
  {
    for (const string &h : history_buffer)
    {
      cout << idx++ << " " << h << endl;
    }
  }
  // Case 2: "history N"
  else
  {
    try
    {
      int n = std::stoi(input_tok[1]);

      int start = std::max(0, (int)history_buffer.size() - n);

      for (int i = start; i < history_buffer.size(); i++)
      {
        cout << (i + 1) << " " << history_buffer[i] << endl;
      }
    }
    catch (const std::exception &e)
    {
      cerr << "history: invalid argument\n";
    }
  }
}