#include <iostream>
#include "tools.hpp"
#include <readline/readline.h>
#include <readline/history.h>
int main()
{
  load_history_from_env();
  cout << unitbuf;
  cerr << unitbuf;
  using_history();
  while (1)
  {

    // cout << "$ ";
    // getline(cin, input);
    input = readline("$ ");
    input_tok = tokenise_u(input);

    if (input_tok.empty())
      continue;
    push_history();
    
    if (input_tok[0].compare("exit") == 0) // Exit the shell
    {
      save_on_exit();    
      return EXIT_SUCCESS;
    }
    else if (input_tok[0].compare("echo") == 0) // echo something
    {
      for (int i = 1; i < input_tok.size(); i++)
      {
        cout << input_tok[i] << " ";
      }
      cout << endl;
    }
    else if (input_tok[0].compare("type") == 0) // check type builtin or external
    {
      if (input_tok.size() < 2)
      {
        cout << "type: missing operand\n";
        continue;
      }

      int _type = type_checker(input_tok[1]);
      if (_type == 1)
      {
        cout << input_tok[1] << " is a shell builtin\n";
      }
      else
      {
        string res = locater(input_tok[1]);
        if (!res.empty())
        {
          cout << input_tok[1] << " is " << res << '\n';
        }
        else
        {
          cout << input_tok[1] << ": not found\n";
        }
      }
    }
    else if (input_tok[0] == "history")
    {
      list_history();
    }
    else if (input_tok[0].compare("$PATH") == 0) // Shows the path
    {
      vector<string> hello = split_path();
      for (auto &i : hello)
      {
        cout << i << '\n';
      }
    }
    else if (input_tok[0].compare("pwd") == 0) // shows current working directory
    {
      string res = get_pwd();
      cout << res << endl;
    }
    else if (input_tok[0].compare("cd") == 0) // changes dir
    {
      int res = change_dir(input_tok[1]);
      if (res == 1)
      {
        cout << "No path given" << endl;
      }
      if (res == -1)
      {
        cout << input_tok[0] << ": " << input_tok[1] << ": No such file or directory" << endl;
      }
    }
    else
    {
      int res = execute_command(input_tok); // executes external commands
      if (res == 0)
      {
        raise_command_error(input_tok[0]);
      }
    }
  }
}
