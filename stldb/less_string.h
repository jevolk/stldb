/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


template<>
struct Less<std::string> : public leveldb::Comparator
{
	int Compare(const leveldb::Slice &a, const leveldb::Slice &b) const override;
	void FindShortestSeparator(std::string *, const leveldb::Slice &) const override;
	void FindShortSuccessor(std::string *) const override;
	const char *Name() const override;
};


inline
const char *Less<std::string>::Name()
const
{
	return "Less<string>";
}


inline
int Less<std::string>::Compare(const leveldb::Slice &a, const leveldb::Slice &b)
const
{
	return a.compare(b);
}


inline
void Less<std::string>::FindShortestSeparator(std::string *, const leveldb::Slice &)
const
{

}


inline
void Less<std::string>::FindShortSuccessor(std::string *)
const
{

}
