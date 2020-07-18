#pragma once
#include <string>
#include <vector>
struct Account
{
	std::string username;
	std::string password;
};

bool IsAccountValid(std::string username, std::string userpassword);

static std::vector<Account> accountsVector =
{
	{"pindrought", "badpassword"},
	{"egor", "betterpassword9681"},
};