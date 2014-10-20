/** 
 * PUBLIC DOMAIN 2014 (C) `git blame test.cpp | cut -d" " -f3 | sed s/^\(//`
 */


#include "stldb/stldb.h"

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		std::cout << "usage: " << argv[0] << " <dbdir>" << std::endl;
		return -1;
	}

	stldb::ldb<std::string,std::string> db(argv[1]);

	for(const auto &pair : db)
		std::cout << pair.first << " => " << pair.second << std::endl;
}
