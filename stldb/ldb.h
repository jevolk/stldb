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

	// Reading
	template<class... Args> const_iterator end(Args&&... args) const;
	template<class... Args> const_iterator begin(Args&&... args) const;
	template<class... Args> const_iterator find(const key_type &key, Args&&... args) const;

	// Utils
	size_t count() const;
	bool exists(const key_type &key) const;
	bool exists(const key_type &key, const bool &cache = false);

	// Writing
	void set(const key_type &key, const mapped_type &value, const bool &sync = false);

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
	const leveldb::Status s = leveldb::DB::Open(options,dir,&ret);
	std::cout << "Leveldb::DB::Open(" << dir << "): " << s.ToString() << std::endl;

	if(!s.ok())
		throw std::runtime_error("Failed to start levelDB");

	return ret;
}())
{


}


template<class Key,
         class T>
void ldb<Key,T>::set(const key_type &key,
                     const mapped_type &value,
                     const bool &sync)
{
	const WriteOptions wopt(sync);
	const leveldb::Status stat = db->Put(wopt,key,value);

	if(!stat.ok())
		throw std::runtime_error(stat.ToString());
}


template<class Key,
         class T>
bool ldb<Key,T>::exists(const key_type &key,
                        const bool &cache)
{
	const auto it = find(key);
	return bool(it);
}


template<class Key,
         class T>
bool ldb<Key,T>::exists(const key_type &key)
const
{
	const auto it = find(key);
	return bool(it);
}


template<class Key,
         class T>
size_t ldb<Key,T>::count()
const
{
	size_t ret = 0;
	const auto end = this->end();
	for(auto it = begin(); it != end; ++it)
		ret++;

	return ret;
}


template<class Key,
         class T>
template<class... Args>
typename ldb<Key,T>::const_iterator ldb<Key,T>::end(Args&&... args)
const
{
	const_iterator it(db.get(),std::forward<Args>(args)...);
	it.seek(END);
	return it;
}


template<class Key,
         class T>
template<class... Args>
typename ldb<Key,T>::const_iterator ldb<Key,T>::begin(Args&&... args)
const
{
	const_iterator it(db.get(),std::forward<Args>(args)...);
	it.seek(FIRST);
	return it;
}


template<class Key,
         class T>
template<class... Args>
typename ldb<Key,T>::const_iterator ldb<Key,T>::find(const key_type &key,
                                                     Args&&... args)
const
{
	const_iterator it(db.get(),std::forward<Args>(args)...);
	it.seek(key);

	if(!it)
		it.seek(END);

	return it;
}
