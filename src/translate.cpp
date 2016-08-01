/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016, djcj <djcj@gmx.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <string>     /* std::string, std::string::npos, size */
#include <algorithm>  /* find, replace */


/* used by translate() function;
 * finds string FindWord in string ReplaceString and replaces
 * it with string ReplaceWord */
std::string &repstr(      std::string &s,
                    const std::string &from,
                    const std::string &to)
{
  if(!from.empty()) {
    for(size_t pos = 0; (pos = s.find(from, pos)) != std::string::npos; pos += to.size()) {
      s.replace(pos, from.size(), to);
    }
  }
  return s;
}

/* translate \n (\\n) and \t (\\t) characters given through
 * command line arguments to real printf characters, ignoring
 * other occurences of \ (\\) */
std::string translate(const char *inputText)
{
  std::string s(inputText);
  s = repstr(s, "\\n", "\n");
  s = repstr(s, "\\t", "\t");
  return s;
}

