/** 
 * PUBLIC DOMAIN 2014 (C) `git blame test.cpp | cut -d" " -f3 | sed s/^\(//`
 */


#include "stldb/stldb.h"

inline
std::string randstr(const size_t &len)
{
	std::string buf;
	buf.resize(len);
	std::generate(buf.begin(),buf.end(),[]
	{
		static const char *const randy = "abcdefghijklmnopqrstuvwxyz";
		return randy[rand() % strlen(randy)];
	});

	return buf;
}


int main(int argc, char **argv)
{
	if(argc < 2)
	{
		std::cout << "usage: " << argv[0] << " <dbdir>" << std::endl;
		return -1;
	}

	srand(getpid());

	stldb::ldb<std::string,std::string> db(argv[1]);

	auto bit = db.begin();
	std::advance(bit,2);
	for(; bit; ++bit)
	{
		std::cout << bit->second << std::endl;
	}

	std::cout << std::endl;

	auto it = db.find("baz",stldb::UPPER);
	if(!it)
	{
		std::cout << "invalid" << std::endl;
	}
	else std::cout << it->second << std::endl;
}
