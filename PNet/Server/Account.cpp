#include "Account.h"

bool IsAccountValid(std::string username, std::string userpassword)
{
	for (auto & acct : accountsVector)
	{
		if (acct.username == username)
		{
			if (acct.password == userpassword)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}