/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


template<class Key>
struct Less : public leveldb::Comparator
{
	int Compare(const leveldb::Slice &a, const leveldb::Slice &b) const override;
	void FindShortestSeparator(std::string *, const leveldb::Slice &) const override;
	void FindShortSuccessor(std::string *) const override;
	const char *Name() const override;
};


template<class Key>
const char *Less<Key>::Name()
const
{
	return "Less";
}


template<class Key>
int Less<Key>::Compare(const leveldb::Slice &a, const leveldb::Slice &b)
const
{
	const Key &ak(*reinterpret_cast<const Key *>(a.data()));
	const Key &bk(*reinterpret_cast<const Key *>(b.data()));
	return ak < bk?  -1:
	       bk < ak?   1:
	                  0;
}


template<class Key>
void Less<Key>::FindShortestSeparator(std::string *, const leveldb::Slice &)
const
{

}


template<class Key>
void Less<Key>::FindShortSuccessor(std::string *)
const
{

}
