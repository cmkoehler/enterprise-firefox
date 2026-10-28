/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <initializer_list>
#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "mozilla/toolkit/components/felt/felt.h"
#include "nsAppRunner.h"

// Runs aCheck(aArg) with gArgc/gArgv temporarily set to aArgs.
static bool CheckWith(bool (*aCheck)(const char*),
                      std::initializer_list<const char*> aArgs,
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
  const bool result = aCheck(aArg);
  gArgc = savedArgc;
  gArgv = savedArgv;
  return result;
}

static bool HasArg(std::initializer_list<const char*> aArgs, const char* aArg) {
  return CheckWith(felt_check_arg_exists, aArgs, aArg);
}

static bool HasColonArg(std::initializer_list<const char*> aArgs,
                        const char* aArg) {
  return CheckWith(felt_check_windows_colon_arg, aArgs, aArg);
}

TEST(FeltCheckArg, Forms)
{
  EXPECT_TRUE(HasArg({"-chrome", "chrome://foo/content/bar.xhtml"}, "chrome"));
  EXPECT_TRUE(HasArg({"--CHROME"}, "chrome"));
  EXPECT_TRUE(HasArg({"--chrome=chrome://foo/content/bar.xhtml"}, "chrome"));
  EXPECT_FALSE(HasArg({"-url", "chrome://foo/content/bar.xhtml"}, "chrome"));
  EXPECT_FALSE(HasArg({"--chromium"}, "chrome"));
  EXPECT_FALSE(HasArg({"-chrome:x"}, "chrome"));
  EXPECT_FALSE(HasArg({"/felt:socket"}, "felt"));
  EXPECT_FALSE(HasArg({"/safe-mode:x"}, "safe-mode"));
#ifdef XP_WIN
  EXPECT_TRUE(HasArg({"/chrome", "chrome://foo/content/bar.xhtml"}, "chrome"));
#endif
}

TEST(FeltCheckArg, WindowsColonForm)
{
#ifdef XP_WIN
  EXPECT_TRUE(
      HasColonArg({"/chrome:chrome://foo/content/bar.xhtml"}, "chrome"));
  EXPECT_TRUE(HasColonArg({"/CHROME:x"}, "chrome"));
  EXPECT_TRUE(HasColonArg({"/chrome:"}, "chrome"));
  EXPECT_FALSE(HasColonArg({"/chromium:x"}, "chrome"));
  EXPECT_FALSE(HasColonArg({"/chrome", "x"}, "chrome"));
  EXPECT_FALSE(HasColonArg({"-chrome:x"}, "chrome"));
#else
  EXPECT_FALSE(
      HasColonArg({"/chrome:chrome://foo/content/bar.xhtml"}, "chrome"));
#endif
}
