/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


template<class Key,
         class T,
         class Compare = Less<Key>>
struct ldb : base::ldb<Key,T,Compare>
{
	template<class... Args> ldb(Args&&... args): base::ldb<Key,T,Compare>(std::forward<Args>(args)...) {}
};


template<class Key,
         class T,
         class Compare>
struct ldb<Key,T *,Compare> : base::ldb<Key,T *,Compare>
{
	template<size_t N> using array_type = std::array<typename std::remove_const<T>::type,N>;

	void insert(const Key &key, const std::pair<const T *, size_t> &val, const Flag &flags = NONE);
	template<size_t N> void insert(const Key &key, const array_type<N> &val, const Flag &flags = NONE);
	template<size_t N> void insert(const Key &key, const T (&val)[N], const Flag &flags = NONE);
	void insert(const Key &key, const T &val, const Flag &flags = NONE);

	template<class... Args> ldb(Args&&... args): base::ldb<Key,T *,Compare>(std::forward<Args>(args)...) {}
};



template<class Key,
         class T,
         class Compare>
void ldb<Key,T *,Compare>::insert(const Key &key,
                                  const T &val,
                                  const Flag &flags)
{
	insert(key,std::make_pair(&val,1),flags);
}


template<class Key,
         class T,
         class Compare>
template<size_t N>
void ldb<Key,T *,Compare>::insert(const Key &key,
                                  const T (&value)[N],
                                  const Flag &flags)
{
	insert(key,std::make_pair(value,N),flags);
}


template<class Key,
         class T,
         class Compare>
template<size_t N>
void ldb<Key,T *,Compare>::insert(const Key &key,
                                  const array_type<N> &value,
                                  const Flag &flags)
{
	insert(key,std::make_pair(value.data(),value.size()),flags);
}


template<class Key,
         class T,
         class Compare>
void ldb<Key,T *,Compare>::insert(const Key &key,
                                  const std::pair<const T *, size_t> &value,
                                  const Flag &flags)
{
	using std::get;

	const WriteOptions wopt(flags);
	const auto &key_ptr(reinterpret_cast<const char *>(&key));
	const auto &val_ptr(reinterpret_cast<const char *>(std::get<0>(value)));
	const leveldb::Slice v(val_ptr,sizeof(T) * std::get<1>(value));
	const leveldb::Slice k(key_ptr,sizeof(key));
	throw_on_error(this->db->Put(wopt,k,v));
}
