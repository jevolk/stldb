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
	using difference_type         = size_t;
	using key_compare             = leveldb::Comparator;
	using reference               = value_type &;
	using const_reference         = const value_type &;
	using pointer                 = value_type *;
	using const_pointer           = const value_type *;
	using iterator                = stldb::iterator<value_type>;
	using const_iterator          = stldb::const_iterator<value_type>;
	using reverse_iterator        = std::reverse_iterator<iterator>;
	using const_reverse_iterator  = std::reverse_iterator<const_iterator>;

	const leveldb::DB *get_ldb() const    { return db.get();  }
	const Options &get_options() const    { return options;   }
	leveldb::DB *get_ldb()                { return db.get();  }

	const_reverse_iterator crbegin(const Flag &flags = Flag(0)) const;
	const_reverse_iterator crend(const Flag &flags = Flag(0)) const;
	const_reverse_iterator rbegin(const Flag &flags = Flag(0)) const;
	const_reverse_iterator rend(const Flag &flags = Flag(0)) const;
	reverse_iterator rbegin(const Flag &flags = Flag(0));
	reverse_iterator rend(const Flag &flags = Flag(0));

	const_iterator cbegin(const Flag &flags = Flag(0)) const;
	const_iterator cend(const Flag &flags = Flag(0)) const;
	const_iterator begin(const Flag &flags = Flag(0)) const;
	const_iterator end(const Flag &flags = Flag(0)) const;
	iterator begin(const Flag &flags = Flag(0));
	iterator end(const Flag &flags = Flag(0));

	const_iterator upper_bound(const key_type &key, const Flag &flags = Flag(0)) const;
	iterator upper_bound(const key_type &key, const Flag &flags = Flag(0));

	const_iterator lower_bound(const key_type &key, const Flag &flags = Flag(0)) const;
	iterator lower_bound(const key_type &key, const Flag &flags = Flag(0));

	const_iterator find(const key_type &key, const Flag &flags = Flag(0)) const;
	iterator find(const key_type &key, const Flag &flags = Flag(0));

	template<class InputIt> void insert(InputIt first, InputIt last, const Flag &flags = Flag(0));
	std::pair<iterator,bool> insert(const value_type &value, const Flag &flags = Flag(0));
	void insert(const key_type &key, const mapped_type &value, const Flag &flags = Flag(0));

	iterator erase(const_iterator first, const_iterator last, const Flag &flags = Flag(0));
	iterator erase(const_iterator pos, const Flag &flags = Flag(0));
	size_t erase(const key_type &key, const Flag &flags = Flag(0));

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
	return std::distance(begin(),end());
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
size_t ldb<Key,T>::erase(const key_type &key,
                         const Flag &flags)
{
	const auto it = const_cast<const ldb *>(this)->find(key,flags);
	const bool ret = bool(it);
	erase(it,flags);
	return ret;
}


template<class Key,
         class T>
typename ldb<Key,T>::iterator ldb<Key,T>::erase(const_iterator pos,
                                                const Flag &flags)
{
	iterator ret = pos++;
	const WriteOptions wops(flags);
	throw_on_error(db->Delete(wops,pos->first));
	return ret;
}


template<class Key,
         class T>
typename ldb<Key,T>::iterator ldb<Key,T>::erase(const_iterator first,
                                                const_iterator last,
                                                const Flag &flags)
{
	leveldb::WriteBatch batch;
	for(; first != last; ++first)
		batch.Delete(first->first);

	const WriteOptions wops(flags);
	throw_on_error(db->Write(wops,&batch));
	return first;
}


template<class Key,
         class T>
void ldb<Key,T>::insert(const key_type &key,
                        const mapped_type &value,
                        const Flag &flags)
{
	const WriteOptions wopt(flags);
	throw_on_error(db->Put(wopt,key,value));
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
template<class InputIt>
void ldb<Key,T>::insert(InputIt first,
                        InputIt last,
                        const Flag &flags)
{
	leveldb::WriteBatch batch;
	for(; first != last; ++first)
		batch.Put(first->first,first->second);

	const WriteOptions wops(flags);
	throw_on_error(db->Write(wops,&batch));
}


template<class Key,
         class T>
typename ldb<Key,T>::iterator ldb<Key,T>::find(const key_type &key,
                                               const Flag &flags)
{
	return {db.get(),key,flags};
}


template<class Key,
         class T>
typename ldb<Key,T>::const_iterator ldb<Key,T>::find(const key_type &key,
                                                     const Flag &flags)
const
{
	return {db.get(),key,flags};
}


template<class Key,
         class T>
typename ldb<Key,T>::iterator ldb<Key,T>::lower_bound(const key_type &key,
                                                      const Flag &flags)
{
	return {db.get(),key,flags|LOWER};
}


template<class Key,
         class T>
typename ldb<Key,T>::const_iterator ldb<Key,T>::lower_bound(const key_type &key,
                                                            const Flag &flags)
const
{
	return {db.get(),key,flags|LOWER};
}


template<class Key,
         class T>
typename ldb<Key,T>::iterator ldb<Key,T>::upper_bound(const key_type &key,
                                                      const Flag &flags)
{
	return {db.get(),key,flags|UPPER};
}


template<class Key,
         class T>
typename ldb<Key,T>::const_iterator ldb<Key,T>::upper_bound(const key_type &key,
                                                            const Flag &flags)
const
{
	return {db.get(),key,flags|UPPER};
}


template<class Key,
         class T>
typename ldb<Key,T>::iterator ldb<Key,T>::end(const Flag &flags)
{
	return {db.get(),END,flags};
}


template<class Key,
         class T>
typename ldb<Key,T>::iterator ldb<Key,T>::begin(const Flag &flags)
{
	return {db.get(),FIRST,flags};
}


template<class Key,
         class T>
typename ldb<Key,T>::const_iterator ldb<Key,T>::end(const Flag &flags)
const
{
	return {db.get(),END,flags};
}


template<class Key,
         class T>
typename ldb<Key,T>::const_iterator ldb<Key,T>::begin(const Flag &flags)
const
{
	return {db.get(),FIRST,flags};
}


template<class Key,
         class T>
typename ldb<Key,T>::const_iterator ldb<Key,T>::cend(const Flag &flags)
const
{
	return {db.get(),END,flags};
}


template<class Key,
         class T>
typename ldb<Key,T>::const_iterator ldb<Key,T>::cbegin(const Flag &flags)
const
{
	return {db.get(),FIRST,flags};
}


template<class Key,
         class T>
typename ldb<Key,T>::reverse_iterator ldb<Key,T>::rend(const Flag &flags)
{
	return {db.get(),FIRST,flags};
}


template<class Key,
         class T>
typename ldb<Key,T>::reverse_iterator ldb<Key,T>::rbegin(const Flag &flags)
{
	return {db.get(),END,flags};
}


template<class Key,
         class T>
typename ldb<Key,T>::const_reverse_iterator ldb<Key,T>::rend(const Flag &flags)
const
{
	return {db.get(),FIRST,flags};
}


template<class Key,
         class T>
typename ldb<Key,T>::const_reverse_iterator ldb<Key,T>::rbegin(const Flag &flags)
const
{
	return {db.get(),END,flags};
}


template<class Key,
         class T>
typename ldb<Key,T>::const_reverse_iterator ldb<Key,T>::crend(const Flag &flags)
const
{
	return {db.get(),FIRST,flags};
}


template<class Key,
         class T>
typename ldb<Key,T>::const_reverse_iterator ldb<Key,T>::crbegin(const Flag &flags)
const
{
	return {db.get(),END,flags};
}
