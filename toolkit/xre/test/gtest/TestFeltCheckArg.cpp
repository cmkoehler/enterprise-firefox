/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <initializer_list>
#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "mozilla/toolkit/components/felt/felt.h"
#include "nsAppRunner.h"

static bool CheckArgWith(std::initializer_list<const char*> aArgs,
                         const char* aArg) {
  std::vector<std::string> storage{"firefox"};
  storage.insert(storage.end(), aArgs.begin(), aArgs.end());
  std::vector<char*> argv;
  for (auto& s : storage) {
    argv.push_back(s.data());
  }
  argv.push_back(nullptr);

  const int savedArgc = gArgc;
  char** const savedArgv = gArgv;
  gArgc = static_cast<int>(storage.size());
  gArgv = argv.data();
  const bool result = felt_check_arg_exists(aArg);
  gArgc = savedArgc;
  gArgv = savedArgv;
  return result;
}

TEST(FeltCheckArg, Forms)
{
  EXPECT_TRUE(
      CheckArgWith({"-chrome", "chrome://foo/content/bar.xhtml"}, "chrome"));
  EXPECT_TRUE(CheckArgWith({"--CHROME"}, "chrome"));
  EXPECT_TRUE(
      CheckArgWith({"--chrome=chrome://foo/content/bar.xhtml"}, "chrome"));
  EXPECT_FALSE(
      CheckArgWith({"-url", "chrome://foo/content/bar.xhtml"}, "chrome"));
  EXPECT_FALSE(CheckArgWith({"--chromium"}, "chrome"));
  EXPECT_FALSE(CheckArgWith({"-chrome:x"}, "chrome"));
}

TEST(FeltCheckArg, WindowsSlashColonForm)
{
#ifdef XP_WIN
  EXPECT_TRUE(
      CheckArgWith({"/chrome:chrome://foo/content/bar.xhtml"}, "chrome"));
  EXPECT_TRUE(CheckArgWith({"/CHROME:x"}, "chrome"));
  EXPECT_TRUE(CheckArgWith({"/felt:socket"}, "felt"));
  EXPECT_FALSE(CheckArgWith({"/chromium:x"}, "chrome"));
#else
  EXPECT_FALSE(
      CheckArgWith({"/chrome:chrome://foo/content/bar.xhtml"}, "chrome"));
#endif
}
