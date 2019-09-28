//James Vu
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cerrno>
#include <ctype.h>
using namespace std;

string keywords[] = { "int", "float", "bool", "if", "else", "then", "endif",
                      "while", "whileend", "do", "doend", "for", "forend",
                      "input", "output", "and", "or", "function" };
char sep[] = { '\'', '(', ')', '{', '}', ',', ':', ';' };
char op[]  = { '*', '+', '-', '=', '/', '>', '<', '%'  };
vector<vector<string>> tok_lex;

// copies file content to string
string get_file_contents(const string filename) {
  ifstream in(filename, ios::in | ios::binary); // open stream
  if (in) {
    string contents; // create contents string and resizes it
    in.seekg(0, in.end);
    contents.resize(in.tellg());
    in.seekg(0, in.beg); // reads file and store in contents
    in.read(&contents[0], contents.size());
    in.close();
    return contents; // return file contents
  }
  throw(errno); // throw error if stream doesn't work
}

// returns character lex_state depending on what ch is
int cur_char(const char ch) {
  if (isalpha(ch)) { return 1; }
  if (isdigit(ch)) { return 2; }
  if (ch == '.')   { return 3; }
  if (ch == '$')   { return 4; }
  for (unsigned int i = 0; i < sizeof(sep); ++i) {
    if (ch == sep[i]) { return 5; }
  }
  for (unsigned int i = 0; i < sizeof(op); ++i) {
    if (ch == op[i]) { return 6; }
  }
  if (ch == '!') { return 7; }
  return 0;
}

// iterates through key_lex and prints out tokens and lexemes
void print_token_lexeme() {
  cout << "\nTOKENS\t\tLEXEMES\n";
  for (auto i = tok_lex.begin(); i != tok_lex.end(); ++i) {
      cout << i->front() << " \t" << i->back() << "\n";
  }
}

int main(int argc, char* argv[]) {
  string content, char_buffer = "";
  int lex_state = 1, char_state = 0, backup = 0;
  int key_flag = 0, int_flag = 0, end_flag1 = 0, end_flag2 = 0;
  vector<string> token_lexeme;
  string filename;
  cout << "Please input a file name to be analyzed.\n";
  cin >> filename;
  content = get_file_contents(filename);
  // process file content until end of string
  auto it = content.begin();
  while (!end_flag2) {
    if (end_flag1) { end_flag2 = 1; } // makes sure loop process last character
    if (it == content.end()) { end_flag1 = 1; } // mark end of string
    char_state = cur_char(*it); // sets character state of it
    switch (lex_state) {
      case 1: // starting lex_state
        switch (char_state) {
          case 1:  lex_state = 2; break; // alphabet
          case 2:  lex_state = 4; break; // digits
          case 3:  lex_state = 7; break; // period
          case 4:  lex_state = 1; break; // $
          case 5:  lex_state = 7; break; // separators
          case 6:  lex_state = 8; break; // operators
          case 7:  lex_state = 9; break; // ! comments
          default: lex_state = 1;        // other
        }
        char_buffer = *it; // sets character buffer with it
        backup = 0; // resets backup
        break;
      case 2: // in identifier or possible keyword
        if (char_state == 1 || char_state == 2 || char_state == 4) {
          lex_state = 2;      // alpha, digit, or $
          char_buffer += *it; // add to buffer
        } else {
          lex_state = 3;      // end of identifier
          backup = 1;         // do not increment it
        }
        break;
      case 3: // end of identifier
        lex_state = 1; // reset lex state
        // checks if char_buffer is a keyword or identifier
        for (unsigned int i = 0; i < sizeof(keywords); ++i) {
          if (keywords[i] == char_buffer) {
            key_flag = 1; // set flag
            break;
          }
        }
        if (key_flag) { // check if keyword or identifier
          token_lexeme = { "KEYWORD", char_buffer };
          key_flag = 0; // reset flag
        } else {
          token_lexeme = { "IDENTIFIER", char_buffer };
        }
        tok_lex.push_back(token_lexeme); // add to tok_lex vector
        break;
      case 4: // in number
        if (char_state == 2) {        // digits
          lex_state = 4;              // in number
          char_buffer += *it;         // add to buffer
        } else if (char_state == 3) { // period
          lex_state = 5;              // in number after decimal
          char_buffer += *it;         // add to buffer
          int_flag = 1;               // sets flag
        } else {
          lex_state = 6;              // end of number
          backup = 1;                 // do not increment it
        }
        break;
      case 5: // in number after decimal
        if (char_state == 2) { // digits
          lex_state = 5;       // stay in same state
          char_buffer += *it;  // add to buffer
        } else{                // not digit
          lex_state = 6;       // end of number
          backup = 1;          // do not increment it
        }
        break;
      case 6: // end of number
        lex_state = 1; // reset lex state
        if (int_flag) { // checks if real float or int
          token_lexeme = { "REAL FLOAT", char_buffer };
          int_flag = 0; // resets flag
        } else {
          token_lexeme = { "INTEGER", char_buffer };
        }
        tok_lex.push_back(token_lexeme); // add to tok_lex vector
        break;
      case 7: // separator
        lex_state = 1; // reset lex state
        token_lexeme = { "SEPARATOR", char_buffer };
        tok_lex.push_back(token_lexeme); // add to tok_lex vector
        backup = 1; // set flag
        break;
      case 8: // operator
        lex_state = 1; // reset lex state
        token_lexeme = { "OPERATOR", char_buffer };
        tok_lex.push_back(token_lexeme); // add to tok_lex vector
        backup = 1; // set flag
        break;
      case 9: // in commment
        if (char_state == 7) {
          lex_state = 10; // end comment
        } else {
          lex_state = 9; // stay in comment
        }
        break;
      case 10: // end comment
        lex_state = 1; // reset lex state
        break;
      default:
        cout << "Invalid lex state\n";
    }
    // does not increment it if backup or reach end of string
    if (!backup && !end_flag1) { ++it; }
    // checks if last character is in final state
    if (end_flag2 && (lex_state == 2 || lex_state == 4 || lex_state == 5 ||
                      lex_state == 9)) {
      cout << "Ending in a non final state.\n";
      break;
    }
  }
  print_token_lexeme();
  return 0;
}
