/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


template<class Key,
         class T>
class ldb
{
	std::unique_ptr<leveldb::Cache> cache;
	std::unique_ptr<const leveldb::FilterPolicy> fp;
	Options options;

  protected:
	std::unique_ptr<leveldb::DB> db;

  public:
	using key_type                = Key;
	using mapped_type             = T;
	using value_type              = std::pair<Slice<const key_type>, Slice<mapped_type>>;
	using size_type               = std::size_t;
	using difference_type         = void;
	using key_compare             = leveldb::Comparator;
	using reference               = value_type &;
	using const_reference         = const value_type &;
	using pointer                 = value_type *;
	using const_pointer           = const value_type *;
	using iterator                = stldb::iterator<value_type>;
	using const_iterator          = stldb::const_iterator<value_type>;
	using reverse_iterator        = std::reverse_iterator<iterator>;
	using const_reverse_iterator  = std::reverse_iterator<const_iterator>;

	template<class... Args> const_reverse_iterator crbegin(Args&&... args) const;
	template<class... Args> const_reverse_iterator crend(Args&&... args) const;
	template<class... Args> const_iterator cbegin(Args&&... args) const;
	template<class... Args> const_iterator cend(Args&&... args) const;
	template<class... Args> reverse_iterator rbegin(Args&&... args);
	template<class... Args> reverse_iterator rend(Args&&... args);
	template<class... Args> iterator begin(Args&&... args);
	template<class... Args> iterator end(Args&&... args);

	template<class... Args> const_iterator find(const key_type &key, Args&&... args) const;
	template<class... Args> iterator find(const key_type &key, Args&&... args);

	// Note: Expected behavior but performance is flawed at this time. Advise against using.
	std::pair<iterator,bool> insert(const value_type &value, const Flag &flags = Flag(0));
	template<class P> std::pair<iterator,bool> insert(P&& value, const Flag &flags = Flag(0));
	template<class InputIt> void insert(InputIt first, InputIt last, const Flag &flags = Flag(0));

	// Note: Non-STL behavior. Advise against using.
	template<class... Args> std::pair<iterator,bool> emplace(Args&&... args);

	void set(const key_type &key, const mapped_type &value, const Flag &flags = Flag(0));

	size_t count(const key_type &key, const Flag &flags = Flag(0)) const;
	size_t count(const key_type &key, const Flag &flags = Flag(0));

	size_t size() const;

	ldb(const std::string &dir,
	    const size_t &cache_size  = (32UL * 1024UL * 1024UL),
	    const size_t &bloom_bits  = 0);
};


template<class Key,
         class T>
ldb<Key,T>::ldb(const std::string &dir,
                const size_t &cache_size,
                const size_t &bloom_bits):
cache(cache_size? leveldb::NewLRUCache(cache_size) : nullptr),
fp(bloom_bits? leveldb::NewBloomFilterPolicy(bloom_bits) : nullptr),
options(cache.get(),fp.get(),leveldb::kSnappyCompression),
db([&]() -> leveldb::DB *
{
	leveldb::DB *ret;
	throw_on_error(leveldb::DB::Open(options,dir,&ret));
	return ret;
}())
{


}


template<class Key,
         class T>
size_t ldb<Key,T>::size()
const
{
	size_t ret = 0;
	const auto cend = this->cend();
	for(auto it = cbegin(); it != cend; ++it)
		ret++;

	return ret;
}


template<class Key,
         class T>
size_t ldb<Key,T>::count(const key_type &key,
                         const Flag &flags)
{
	return bool(find(key,flags));
}


template<class Key,
         class T>
size_t ldb<Key,T>::count(const key_type &key,
                         const Flag &flags)
const
{
	return bool(find(key,flags));
}


template<class Key,
         class T>
template<class... Args>
std::pair<typename ldb<Key,T>::iterator,bool> ldb<Key,T>::emplace(Args&&... args)
{
	static const WriteOptions wopt(false);
	throw_on_error(db->Put(wopt,std::forward<Args>(args)...));
	return {end(),true};
}


template<class Key,
         class T>
template<class InputIt>
void ldb<Key,T>::insert(InputIt first,
                        InputIt last,
                        const Flag &flags)
{
	const WriteOptions wopt(flags);
	std::for_each(first,last,[&]
	(InputIt &it)
	{
		throw_on_error(db->Put(wopt,it.first,it.second));
	});
}


template<class Key,
         class T>
template<class P>
std::pair<typename ldb<Key,T>::iterator,bool> ldb<Key,T>::insert(P&& value,
                                                                 const Flag &flags)
{
	auto ret = std::make_pair(find(value.first),false);
	if(ret.first)
		return ret;

	const WriteOptions wopt(flags);
	throw_on_error(db->Put(wopt,value.first,value.second));
	ret.first = find(value.first);
	ret.second = true;
	return ret;
}


template<class Key,
         class T>
std::pair<typename ldb<Key,T>::iterator,bool> ldb<Key,T>::insert(const value_type &value,
                                                                 const Flag &flags)
{
	auto ret = std::make_pair(find(value.first),false);
	if(ret.first)
		return ret;

	const WriteOptions wopt(flags);
	throw_on_error(db->Put(wopt,value.first,value.second));
	ret.first = find(value.first);
	ret.second = true;
	return ret;
}


template<class Key,
         class T>
void ldb<Key,T>::set(const key_type &key,
                     const mapped_type &value,
                     const Flag &flags)
{
	const WriteOptions wopt(flags);
	const leveldb::Status stat = db->Put(wopt,key,value);

	if(!stat.ok())
		throw std::runtime_error(stat.ToString());
}


template<class Key,
         class T>
template<class... Args>
typename ldb<Key,T>::iterator ldb<Key,T>::find(const key_type &key,
                                               Args&&... args)
{
	return {db.get(),key,std::forward<Args>(args)...};
}


template<class Key,
         class T>
template<class... Args>
typename ldb<Key,T>::const_iterator ldb<Key,T>::find(const key_type &key,
                                                     Args&&... args)
const
{
	return {db.get(),key,std::forward<Args>(args)...};
}


template<class Key,
         class T>
template<class... Args>
typename ldb<Key,T>::iterator ldb<Key,T>::end(Args&&... args)
{
	return {db.get(),END,std::forward<Args>(args)...};
}


template<class Key,
         class T>
template<class... Args>
typename ldb<Key,T>::iterator ldb<Key,T>::begin(Args&&... args)
{
	return {db.get(),FIRST,std::forward<Args>(args)...};
}


template<class Key,
         class T>
template<class... Args>
typename ldb<Key,T>::reverse_iterator ldb<Key,T>::rend(Args&&... args)
{
	return {db.get(),FIRST,std::forward<Args>(args)...};
}


template<class Key,
         class T>
template<class... Args>
typename ldb<Key,T>::reverse_iterator ldb<Key,T>::rbegin(Args&&... args)
{
	return {db.get(),END,std::forward<Args>(args)...};
}


template<class Key,
         class T>
template<class... Args>
typename ldb<Key,T>::const_iterator ldb<Key,T>::cend(Args&&... args)
const
{
	return {db.get(),END,std::forward<Args>(args)...};
}


template<class Key,
         class T>
template<class... Args>
typename ldb<Key,T>::const_iterator ldb<Key,T>::cbegin(Args&&... args)
const
{
	return {db.get(),FIRST,std::forward<Args>(args)...};
}


template<class Key,
         class T>
template<class... Args>
typename ldb<Key,T>::const_reverse_iterator ldb<Key,T>::crend(Args&&... args)
const
{
	return {db.get(),FIRST,std::forward<Args>(args)...};
}


template<class Key,
         class T>
template<class... Args>
typename ldb<Key,T>::const_reverse_iterator ldb<Key,T>::crbegin(Args&&... args)
const
{
	return {db.get(),END,std::forward<Args>(args)...};
}
