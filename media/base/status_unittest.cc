// Copyright 2014 Google Inc. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "media/base/status.h"

#include <sstream>

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace media {

static void CheckStatus(const Status& s,
                        error::Code code,
                        const std::string& message) {
  EXPECT_EQ(code, s.error_code());
  EXPECT_EQ(message, s.error_message());

  if (code == error::OK) {
    EXPECT_TRUE(s.ok());
    EXPECT_EQ("OK", s.ToString());
  } else {
    EXPECT_TRUE(!s.ok());
    EXPECT_THAT(s.ToString(), testing::HasSubstr(message));

    std::ostringstream string_stream;
    string_stream << code << ":" << message;
    EXPECT_EQ(string_stream.str(), s.ToString());
  }
}

TEST(Status, Empty) {
  CheckStatus(Status(), error::OK, "");
}

TEST(Status, OK) {
  CheckStatus(Status::OK, error::OK, "");
}

TEST(Status, ConstructorOK) {
  CheckStatus(Status(error::OK, "msg"), error::OK, "");
}

TEST(Status, CheckOK) {
  CHECK_OK(Status());
}

TEST(Status, CheckOKDeath) {
  Status status(error::UNKNOWN, "Status Unknown");
  ASSERT_DEATH(CHECK_OK(status), "Status Unknown");
  ASSERT_DEATH(CHECK_OK(status) << "Foo1234", "Foo1234");
}

TEST(Status, SetError) {
  Status status;
  status.SetError(error::CANCELLED, "message");
  CheckStatus(status, error::CANCELLED, "message");
}

TEST(Status, SetErrorOK) {
  Status status(error::CANCELLED, "message");
  status.SetError(error::OK, "msg");
  CheckStatus(status, error::OK, "");
}

TEST(Status, Unknown) {
  CheckStatus(Status::UNKNOWN, error::UNKNOWN, "");
}

TEST(Status, Filled) {
  CheckStatus(Status(error::CANCELLED, "message"), error::CANCELLED, "message");
}

TEST(Status, Clear) {
  Status status(error::CANCELLED, "message");
  status.Clear();
  CheckStatus(status, error::OK, "");
}

TEST(Status, Copy) {
  Status a(error::CANCELLED, "message");
  Status b(a);
  ASSERT_EQ(a, b);
}

TEST(Status, Assign) {
  Status a(error::CANCELLED, "message");
  Status b;
  b = a;
  ASSERT_EQ(a, b);
}

TEST(Status, AssignEmpty) {
  Status a(error::CANCELLED, "message");
  Status b;
  a = b;
  ASSERT_EQ("OK", a.ToString());
  ASSERT_TRUE(b.ok());
  ASSERT_TRUE(a.ok());
}

TEST(Status, Update) {
  Status s;
  s.Update(Status::OK);
  ASSERT_TRUE(s.ok());
  Status a(error::CANCELLED, "message");
  s.Update(a);
  ASSERT_EQ(s, a);
  Status b(error::UNIMPLEMENTED, "other message");
  s.Update(b);
  ASSERT_EQ(s, a);
  s.Update(Status::OK);
  ASSERT_EQ(s, a);
  ASSERT_FALSE(s.ok());
}

TEST(Status, Swap) {
  Status a(error::CANCELLED, "message");
  Status b = a;
  Status c;
  c.Swap(&a);
  ASSERT_EQ(c, b);
  ASSERT_EQ(a, Status::OK);
}

TEST(Status, MatchOK) {
  ASSERT_TRUE(Status().Matches(Status::OK));
}

TEST(Status, MatchSame) {
  Status a(error::UNKNOWN, "message");
  Status b(error::UNKNOWN, "message");
  ASSERT_TRUE(a.Matches(b));
}

TEST(Status, MatchCopy) {
  Status a(error::UNKNOWN, "message");
  Status b = a;
  ASSERT_TRUE(a.Matches(b));
}

TEST(Status, MatchDifferentCode) {
  Status a(error::UNKNOWN, "message");
  Status b(error::CANCELLED, "message");
  ASSERT_TRUE(!a.Matches(b));
}

TEST(Status, MatchDifferentMessage) {
  Status a(error::UNKNOWN, "message");
  Status b(error::UNKNOWN, "another");
  ASSERT_TRUE(a.Matches(b));
}

TEST(Status, EqualsOK) {
  ASSERT_EQ(Status::OK, Status());
}

TEST(Status, EqualsSame) {
  ASSERT_EQ(Status(error::UNKNOWN, "message"),
            Status(error::UNKNOWN, "message"));
}

TEST(Status, EqualsCopy) {
  Status a(error::UNKNOWN, "message");
  Status b = a;
  ASSERT_EQ(a, b);
}

TEST(Status, EqualsDifferentCode) {
  ASSERT_NE(Status(error::UNKNOWN, "message"),
            Status(error::CANCELLED, "message"));
}

TEST(Status, EqualsDifferentMessage) {
  ASSERT_NE(Status(error::UNKNOWN, "message"),
            Status(error::UNKNOWN, "another"));
}

}  // namespace media
