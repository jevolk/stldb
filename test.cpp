/** 
 * PUBLIC DOMAIN 2014 (C) `git blame test.cpp | cut -d" " -f3 | sed s/^\(//`
 */


#include "stldb/stldb.h"

inline
std::string randstr(const size_t &len = 5)
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


template<class Db>
void print_db(const Db &db)
{
	std::cout << "--- db ---" << std::endl;

	for(const auto &it : db)
		std::cout << it.first << " => " << it.second << std::endl;

	std::cout << "----------" << std::endl;
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

	print_db(db);


	std::vector<std::pair<std::string,std::string>> foo(16);
	std::generate(foo.begin(),foo.end(),[&]
	{
		return std::make_pair(randstr(5),randstr(10));
	});

	db.insert(foo.begin(),foo.end());
	print_db(db);

	db.erase(db.cbegin(),db.cend());
	print_db(db);
}
