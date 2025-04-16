#include <iostream>
#include <vector>

std::vector<int> prefixFunction(const std::string& str) //взята четсно с презентации 
{
  std::vector<int> p(str.size(), 0);
  for(size_t i = 1; i < str.size(); i++) 
  {
    int k = p[i - 1];
    while (k > 0 && str[i] != str[k])
    {
      k = p[k - 1];
    }
    if (str[i] == str[k])
    {
      k++;
    }
    p[i] = k;
  }
  return p;
}

std::vector<int> kmp(const std::string& pattern, const std::string& text) 
{
  std::vector<int> p = prefixFunction(pattern);
  int number_of_match = 0;
  std::vector<int> result;
  if (pattern.length() == 0) return result;
  if (text.length() < pattern.length()) return result;

  for (size_t i = 0; i < text.length(); i++)
  {
    while (number_of_match > 0 && pattern[number_of_match] != text[i])
    {
      number_of_match = p[number_of_match - 1];
    }
    if (pattern[number_of_match] == text[i])
    {
      number_of_match++;
    }
    if (number_of_match == pattern.length())
    {
      result.push_back(i - pattern.length() + 1);
      number_of_match = p[number_of_match - 1];
    }
  }
  return result;
}

void print(std::vector<int> result)
{
  for(size_t i = 0; i < result.size(); i++)
  {
    std::cout << result[i] << "\n";
  } 
}

int main(int argc, const char* argv[])
{
  std::string text, pattern; 
  std::cin >> text >> pattern; 

  std::vector<int> result = kmp(pattern, text);

  print(result);

  return 0;
}