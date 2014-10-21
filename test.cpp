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

	std::cout << db.count("foo") << std::endl;

	auto it = db.find("foo",stldb::FLUSH);
	auto it2 = db.find("foo");

	std::cout << "it value:  " << it->second << std::endl;
	std::cout << "it2 value:  " << it2->second << std::endl;
	std::cout << std::endl;

	it->second = randstr(22);

	std::cout << "it value:  " << it->second << std::endl;
	std::cout << "it2 value:  " << it2->second << std::endl;
	std::cout << std::endl;

	it = db.find("foo");
	it2 = db.find("foo");

	std::cout << "it value:  " << it->second << std::endl;
	std::cout << "it2 value:  " << it2->second << std::endl;
	std::cout << std::endl;
}
