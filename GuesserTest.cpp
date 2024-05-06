/**
 * Unit Tests for the class
**/

#include <gtest/gtest.h>
#include "Guesser.h"

class GuesserTest : public ::testing::Test
{
	protected:
		GuesserTest(){} //constructor runs before each test
		virtual ~GuesserTest(){} //destructor cleans up after tests
		virtual void SetUp(){} //sets up before each test (after constructor)
		virtual void TearDown(){} //clean up after each test, (before destructor)
};

TEST(GuesserTest, truncate_no_trunc) {
  Guesser guesser{ "less" };
  ASSERT_TRUE(guesser.match("less"));
}
TEST(GuesserTest, truncate_more_than_32_fail) {
  Guesser guesser{ "morethanthirtytwocharacterpassword" };
  ASSERT_FALSE(guesser.match("morethanthirtytwocharacterpassword"));
}
TEST(GuesserTest, truncate_more_than_32_success) {
  Guesser guesser{ "morethanthirtytwocharacterpassword" };
  ASSERT_TRUE(guesser.match("morethanthirtytwocharacterpasswo"));
}
TEST(GuesserTest, truncate_exactly_32_success) {
  Guesser guesser{ "exactlythirtytwocharactersphrase" };
  ASSERT_TRUE(guesser.match("exactlythirtytwocharactersphrase"));
}

TEST(GuesserTest, zeropassword_success) {
  Guesser guesser{ "" };
  ASSERT_TRUE(guesser.match(""));
}
TEST(GuesserTest, zeropassword_fail) {
  Guesser guesser{ "" };
  ASSERT_FALSE(guesser.match("_"));
}

TEST(GuesserTest, upper_lower_case) {
  Guesser guesser{ "abc" };
  ASSERT_FALSE(guesser.match("aBc"));
}

TEST(GuesserTest, lockout_match_multiple) {
  Guesser guesser{ "password" };
  guesser.match("password");
  ASSERT_TRUE(guesser.match("password"));
}
TEST(GuesserTest, lockout_one_letter_incorrect) {
  Guesser guesser{ "password" };
  guesser.match("passwor_");
  ASSERT_TRUE(guesser.match("password"));
}
TEST(GuesserTest, lockout_two_letters_incorrect) {
  Guesser guesser{ "password" };
  guesser.match("passwo__");
  ASSERT_TRUE(guesser.match("password"));
}
TEST(GuesserTest, lockout_three_letters_incorrect) {
  Guesser guesser{ "password" };
  guesser.match("passw___");
  ASSERT_FALSE(guesser.match("password"));
}
TEST(GuesserTest, lockout_all_letters_incorrect) {
  Guesser guesser{ "password" };
  guesser.match("________");
  ASSERT_FALSE(guesser.match("password"));
}
TEST(GuesserTest, lockout_first_three_letters_incorrect) {
  Guesser guesser{ "password" };
  guesser.match("___sword");
  ASSERT_FALSE(guesser.match("password"));
}
TEST(GuesserTest, lockout_random_three_letters_incorrect) {
  Guesser guesser{ "password" };
  guesser.match("pas_w__d");
  ASSERT_FALSE(guesser.match("password"));
}

TEST(GuesserTest, no_lockout_if_password_shorter_than_three_longer_length_same_base){
  Guesser guesser{ "ab" };
  guesser.match("abcdef");
  ASSERT_TRUE(guesser.match("ab"));
}
TEST(GuesserTest, no_lockout_if_password_shorter_than_three_longer_length_wrong_base){
  Guesser guesser{ "ab" };
  guesser.match("______");
  ASSERT_TRUE(guesser.match("ab"));
}
TEST(GuesserTest, no_lockout_if_password_shorter_than_three_same_length){
  Guesser guesser{ "ab" };
  guesser.match("__");
  ASSERT_TRUE(guesser.match("ab"));
}
TEST(GuesserTest, lockout_if_password_is_three){
  Guesser guesser{ "abc" };
  guesser.match("___");
  ASSERT_FALSE(guesser.match("abc"));
}
TEST(GuesserTest, lockout_if_password_is_three_long_guess_with_same_base){
  Guesser guesser{ "abc" };
  guesser.match("abcdefghi");
  ASSERT_FALSE(guesser.match("abc"));
}

TEST(GuesserTest, remaining_with_correct_guess){
  Guesser guesser{ "abcd" };
  ASSERT_EQ(guesser.remaining(), 3);
  guesser.match("abcd");
  ASSERT_EQ(guesser.remaining(), 3);
}
TEST(GuesserTest, remaining_with_incorrect_guess){
  Guesser guesser{ "abcd" };
  ASSERT_EQ(guesser.remaining(), 3);
  guesser.match("abc_");
  ASSERT_EQ(guesser.remaining(), 2);
}
TEST(GuesserTest, remaining_with_two_incorrect_guesses){
  Guesser guesser{ "abcd" };
  ASSERT_EQ(guesser.remaining(), 3);
  guesser.match("abc_");
  ASSERT_EQ(guesser.remaining(), 2);
  guesser.match("abcdef");
  ASSERT_EQ(guesser.remaining(), 1);
}
TEST(GuesserTest, remaining_reset){
  Guesser guesser{ "abcd" };
  ASSERT_EQ(guesser.remaining(), 3);
  guesser.match("abc_");
  guesser.match("a_cd");
  ASSERT_EQ(guesser.remaining(), 1);
  guesser.match("abcd");
  ASSERT_EQ(guesser.remaining(), 3);
}
TEST(GuesserTest, remaining_with_lockout){
  Guesser guesser{ "abcd" };
  ASSERT_EQ(guesser.remaining(), 3);
  guesser.match("a___");
  ASSERT_EQ(guesser.remaining(), 2);
  ASSERT_FALSE(guesser.match("abcd"));
  ASSERT_EQ(guesser.remaining(), 1);
}
TEST(GuesserTest, remaining_no_reset_after_no_guesses){
  Guesser guesser{ "abcd" };
  ASSERT_EQ(guesser.remaining(), 3);
  guesser.match("abc_");
  guesser.match("_bc_");
  guesser.match("a_cd");
  ASSERT_EQ(guesser.remaining(), 0);
  ASSERT_FALSE(guesser.match("abcd"));
  ASSERT_EQ(guesser.remaining(), 0);
}

TEST(GuesserTest, no_reset_after_no_guesses){
  Guesser guesser{ "my$Password" };
  guesser.match("my$Password1");
  guesser.match("my$Password2");
  guesser.match("my$Password3");
  ASSERT_EQ(guesser.remaining(), 0);
  ASSERT_FALSE(guesser.match("my$Password"));
  ASSERT_EQ(guesser.remaining(), 0);
  ASSERT_FALSE(guesser.match("my$Password"));
}
