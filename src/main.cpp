#include <iostream>
#include "tools.hpp"

int main()
{
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::string input;
  std::vector<std::string> input_tok;
  vector<string> history_buffer;

  while (1)
  {
    std::cout << "$ ";
    std::getline(std::cin, input);

    input_tok = tokenise_u(input);

    if (input_tok.empty())
      continue;
    string flattened_input_tok;
    for (auto &i : input_tok)
    {
      flattened_input_tok += " " + i;
    }
    history_buffer.push_back(flattened_input_tok);
    if (input_tok[0].compare("exit") == 0) // Exit the shell
    {
      return EXIT_SUCCESS;
    }
    else if (input_tok[0].compare("echo") == 0) // echo something
    {
      for (int i = 1; i < input_tok.size(); i++)
      {
        std::cout << input_tok[i] << " ";
      }
      std::cout << std::endl;
    }
    else if (input_tok[0].compare("type") == 0) // check type builtin or external
    {
      if (input_tok.size() < 2)
      {
        std::cout << "type: missing operand\n";
        continue;
      }

      int _type = type_checker(input_tok[1]);
      if (_type == 1)
      {
        std::cout << input_tok[1] << " is a shell builtin\n";
      }
      else
      {
        std::string res = locater(input_tok[1]);
        if (!res.empty())
        {
          std::cout << input_tok[1] << " is " << res << '\n';
        }
        else
        {
          std::cout << input_tok[1] << ": not found\n";
        }
      }
    }
    else if (input_tok[0] == "history")
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
    else if (input_tok[0].compare("$PATH") == 0) // Shows the path
    {
      std::vector<std::string> hello = split_path();
      for (auto &i : hello)
      {
        std::cout << i << '\n';
      }
    }
    else if (input_tok[0].compare("pwd") == 0)
    {
      string res = get_pwd();
      cout << res << endl;
    }
    else if (input_tok[0].compare("cd") == 0)
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
