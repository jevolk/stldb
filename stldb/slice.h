/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


template<class T>
class Slice : public leveldb::Slice
{
	base::iterator *base;

	template<class U> typename
	std::enable_if<std::is_convertible<U,std::string>::value,U>::type
	cast() const
	{
		return {data(),size()};
	}

	template<class U> typename
	std::enable_if<!std::is_convertible<U,std::string>(),U>::type
	cast() const
	{
		assert(sizeof(U) <= size());
		return *reinterpret_cast<const U *>(data());
	}

  public:
	using value_type = T;

	operator T() const
	{
		return cast<T>();
	}

	Slice &operator=(const T &t)
	{
		assert(base != nullptr);                  // trying to write to const_iterator
		const auto &key(base->it->key());
		const WriteOptions wops(base->flags);
		const auto &ptr(reinterpret_cast<const char *>(&t));
		const leveldb::Slice val(ptr,sizeof(t));
		throw_on_error(base->db->Put(wops,key,val));

		if(wops.flush)
			base->flush();

		return *this;
	}

	template<class... Args>
	Slice(base::iterator *const &base = nullptr, Args&&... args):
	leveldb::Slice(std::forward<Args>(args)...),
	base(base)
	{

	}
};


template<class T>
class Slice<T *> : public leveldb::Slice
{
	base::iterator *base;

  public:
	using value_type = T *;

	size_t size() const
	{
		return leveldb::Slice::size() / sizeof(T);
	}

	operator const T *() const
	{
		return reinterpret_cast<const T *>(data());
	}

	Slice &operator=(const std::pair<const T *, size_t> &t)
	{
		using std::get;

		assert(base != nullptr);                  // trying to write to const_iterator
		const auto &key(base->it->key());
		const WriteOptions wops(base->flags);
		const auto &ptr(reinterpret_cast<const char *>(get<0>(t)));
		const size_t size(get<1>(t) * sizeof(T));
		const leveldb::Slice val(ptr,size);
		throw_on_error(base->db->Put(wops,key,val));

		if(wops.flush)
			base->flush();

		return *this;
	}

	template<size_t N>
	Slice &operator=(const T (&t)[N])
	{
		return this->operator=(std::make_pair(t,N));
	}

	template<class... Args>
	Slice(base::iterator *const &base = nullptr, Args&&... args):
	leveldb::Slice(std::forward<Args>(args)...),
	base(base)
	{

	}

	static_assert(std::is_const<T>::value, "Pointer types are never written to and must be const");
};


template<>
class Slice<std::string> : public leveldb::Slice
{
	base::iterator *base;

  public:
	using value_type = std::string;

	operator std::string() const
	{
		return {data(),size()};
	}

	Slice &operator=(const std::string &value)
	{
		assert(base != nullptr);                  // trying to write to const_iterator
		const auto &key(base->it->key());
		const WriteOptions wops(base->flags);
		const leveldb::Slice val(value);
		throw_on_error(base->db->Put(wops,key,val));

		if(wops.flush)
			base->flush();

		return *this;
	}

	template<class... Args>
	Slice(base::iterator *const &base = nullptr, Args&&... args):
	leveldb::Slice(std::forward<Args>(args)...),
	base(base)
	{

	}
};


template<class T>
std::ostream &operator<<(std::ostream &s,
                         const Slice<T> &slice)
{
	s << static_cast<T>(slice);
	return s;
}
