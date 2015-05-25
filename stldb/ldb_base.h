/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


template<class Key,
         class T,
         class Compare = Less<Key>>
class ldb
{
	Options opts;
	Compare comp;
	std::unique_ptr<leveldb::Cache> cache;
	std::unique_ptr<const leveldb::FilterPolicy> fp;

  protected:
	std::unique_ptr<leveldb::DB> db;

  public:
	using key_type                          = Key;
	using mapped_type                       = T;
	using value_type                        = std::pair<Slice<const key_type>, Slice<mapped_type>>;
	using size_type                         = std::size_t;
	using difference_type                   = size_t;
	using key_compare                       = Compare;
	using reference                         = value_type &;
	using const_reference                   = const value_type &;
	using pointer                           = value_type *;
	using const_pointer                     = const value_type *;
	using iterator                          = stldb::iterator<value_type>;
	using const_iterator                    = stldb::const_iterator<value_type>;
	using reverse_iterator                  = std::reverse_iterator<iterator>;
	using const_reverse_iterator            = std::reverse_iterator<const_iterator>;

	const Options &get_opts() const         { return opts;                                           }
	const Compare &get_comp() const         { return comp;                                           }
	const leveldb::DB *get_ldb() const      { return db.get();                                       }
	leveldb::DB *get_ldb()                  { return db.get();                                       }

	operator const leveldb::DB &() const    { return *get_ldb();                                     }
	operator leveldb::DB &()                { return *get_ldb();                                     }

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

	const_iterator cfind(const key_type &key, const Flag &flags = Flag(0)) const;
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

	template<class... CompareArgs>
	ldb(const std::string &dir = {},   // Directory of the LevelDB (empty string disables this instance)
	    const Options &opts = {},      // Options structure
	    CompareArgs&&... comp);        // Direct emplacement of any custom comparator
};


template<class Key,
         class T,
         class Compare>
template<class... CompareArgs>
ldb<Key,T,Compare>::ldb(const std::string &dir,
                        const Options &opts,
                        CompareArgs&&... comp):
opts   { opts                                                                                      },
comp   { std::forward<CompareArgs>(comp)...                                                        },
cache  { !dir.empty() && opts.cache_size? leveldb::NewLRUCache(opts.cache_size) : nullptr          },
fp     { !dir.empty() && opts.bloom_bits? leveldb::NewBloomFilterPolicy(opts.bloom_bits) : nullptr },
db     { dir.empty()? nullptr : [this,&dir]
{
	this->opts.comparator     = &this->comp;
	this->opts.block_cache    = this->cache.get();
	this->opts.filter_policy  = this->fp.get();

	leveldb::DB *ret;
	throw_on_error(leveldb::DB::Open(this->opts,dir,&ret));
	return ret;
}()}
{

}


template<class Key,
         class T,
         class Compare>
size_t ldb<Key,T,Compare>::size()
const
{
	return std::distance(begin(),end());
}


template<class Key,
         class T,
         class Compare>
size_t ldb<Key,T,Compare>::count(const key_type &key,
                                 const Flag &flags)
{
	return bool(find(key,flags));
}


template<class Key,
         class T,
         class Compare>
size_t ldb<Key,T,Compare>::count(const key_type &key,
                                 const Flag &flags)
const
{
	return bool(find(key,flags));
}


template<class Key,
         class T,
         class Compare>
size_t ldb<Key,T,Compare>::erase(const key_type &key,
                                 const Flag &flags)
{
	const auto it(const_cast<const ldb *>(this)->find(key,flags));
	const bool ret(it);
	erase(it,flags);
	return ret;
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::iterator
ldb<Key,T,Compare>::erase(const_iterator pos,
                          const Flag &flags)
{
	iterator ret(pos++);
	const WriteOptions wops(flags);
	throw_on_error(db->Delete(wops,pos->first));
	return ret;
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::iterator
ldb<Key,T,Compare>::erase(const_iterator first,
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
         class T,
         class Compare>
void ldb<Key,T,Compare>::insert(const key_type &key,
                                const mapped_type &value,
                                const Flag &flags)
{
	const WriteOptions wopt(flags);
	const auto k(boost::lexical_cast<std::string>(key));
	const auto v(boost::lexical_cast<std::string>(value));
	throw_on_error(db->Put(wopt,k,v));
}


template<> inline
void ldb<std::string,std::string>::insert(const key_type &key,
                                          const std::string &value,
                                          const Flag &flags)
{
	const WriteOptions wopt(flags);
	throw_on_error(db->Put(wopt,key,value));
}


template<class Key,
         class T,
         class Compare>
std::pair<typename ldb<Key,T,Compare>::iterator,bool>
ldb<Key,T,Compare>::insert(const value_type &value,
                           const Flag &flags)
{
	auto ret(std::make_pair(find(value.first),false));
	if(ret.first)
		return ret;

	const WriteOptions wopt(flags);
	throw_on_error(db->Put(wopt,value.first,value.second));
	ret.first = find(value.first);
	ret.second = true;
	return ret;
}


template<class Key,
         class T,
         class Compare>
template<class InputIt>
void ldb<Key,T,Compare>::insert(InputIt first,
                                InputIt last,
                                const Flag &flags)
{
	leveldb::WriteBatch batch;
	for(; first != last; ++first)
	{
		const auto keyptr(reinterpret_cast<const char *>(&(first->first)));
		const auto valptr(reinterpret_cast<const char *>(&(first->second)));
		const leveldb::Slice key(keyptr,sizeof(first->first));
		const leveldb::Slice val(valptr,sizeof(first->second));
		batch.Put(key,val);
	}

	const WriteOptions wops(flags);
	throw_on_error(db->Write(wops,&batch));
}


template<>
template<class InputIt>
void ldb<std::string,std::string>::insert(InputIt first,
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
         class T,
         class Compare>
typename ldb<Key,T,Compare>::iterator
ldb<Key,T,Compare>::find(const key_type &key,
                         const Flag &flags)
{
	return {get_ldb(),&get_comp(),key,flags};
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::const_iterator
ldb<Key,T,Compare>::find(const key_type &key,
                         const Flag &flags)
const
{
	return {get_ldb(),&get_comp(),key,flags};
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::const_iterator
ldb<Key,T,Compare>::cfind(const key_type &key,
                          const Flag &flags)
const
{
	return {get_ldb(),&get_comp(),key,flags};
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::iterator
ldb<Key,T,Compare>::lower_bound(const key_type &key,
                                const Flag &flags)
{
	return {get_ldb(),&get_comp(),key,Flag(flags|LOWER)};
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::const_iterator
ldb<Key,T,Compare>::lower_bound(const key_type &key,
                                const Flag &flags)
const
{
	return {get_ldb(),&get_comp(),key,Flag(flags|LOWER)};
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::iterator
ldb<Key,T,Compare>::upper_bound(const key_type &key,
                                const Flag &flags)
{
	return {get_ldb(),&get_comp(),key,Flag(flags|UPPER)};
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::const_iterator
ldb<Key,T,Compare>::upper_bound(const key_type &key,
                                const Flag &flags)
const
{
	return {get_ldb(),&get_comp(),key,Flag(flags|UPPER)};
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::iterator
ldb<Key,T,Compare>::end(const Flag &flags)
{
	return {get_ldb(),&get_comp(),END,flags};
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::iterator
ldb<Key,T,Compare>::begin(const Flag &flags)
{
	return {get_ldb(),&get_comp(),FIRST,flags};
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::const_iterator
ldb<Key,T,Compare>::end(const Flag &flags)
const
{
	return {get_ldb(),&get_comp(),END,flags};
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::const_iterator
ldb<Key,T,Compare>::begin(const Flag &flags)
const
{
	return {get_ldb(),&get_comp(),FIRST,flags};
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::const_iterator
ldb<Key,T,Compare>::cend(const Flag &flags)
const
{
	return {get_ldb(),&get_comp(),END,flags};
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::const_iterator
ldb<Key,T,Compare>::cbegin(const Flag &flags)
const
{
	return {get_ldb(),&get_comp(),FIRST,flags};
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::reverse_iterator
ldb<Key,T,Compare>::rend(const Flag &flags)
{
	return {get_ldb(),&get_comp(),FIRST,flags};
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::reverse_iterator
ldb<Key,T,Compare>::rbegin(const Flag &flags)
{
	return {get_ldb(),&get_comp(),END,flags};
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::const_reverse_iterator
ldb<Key,T,Compare>::rend(const Flag &flags)
const
{
	return {get_ldb(),&get_comp(),FIRST,flags};
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::const_reverse_iterator
ldb<Key,T,Compare>::rbegin(const Flag &flags)
const
{
	return {get_ldb(),&get_comp(),END,flags};
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::const_reverse_iterator
ldb<Key,T,Compare>::crend(const Flag &flags)
const
{
	return {get_ldb(),&get_comp(),FIRST,flags};
}


template<class Key,
         class T,
         class Compare>
typename ldb<Key,T,Compare>::const_reverse_iterator
ldb<Key,T,Compare>::crbegin(const Flag &flags)
const
{
	return {get_ldb(),&get_comp(),END,flags};
}
