/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


template<class T>
class Slice : public leveldb::Slice
{
	iterator_base *base;

	template<class U> typename
	std::enable_if<std::is_convertible<U,std::string>::value,U>::type
	cast() const
	{
		return {data(),size()};
	}

	template<class U> typename
	std::enable_if<std::is_trivial<U>::value,U>::type
	cast() const
	{
		assert(sizeof(T) <= size());
		return *reinterpret_cast<const T *>(data());
	}

  public:
	using value_type = T;

	operator T() const
	{
		return cast<T>();
	}

	// Note: is_trivially_copyable not available GCC ~4.9
	Slice &operator=(T&& t)
	{
		const WriteOptions wops(base->flags);
		const auto &key(base->it->key());
		const leveldb::Slice val(reinterpret_cast<const char *>(&t),sizeof(t));
		throw_on_error(base->db->Put(wops,key,val));

		if(wops.flush)
			base->flush();

		return *this;
	}

	template<class... Args>
	Slice(iterator_base *const &base = nullptr, Args&&... args):
	leveldb::Slice(std::forward<Args>(args)...),
	base(base)
	{

	}
};


template<>
class Slice<std::string> : public leveldb::Slice
{
	iterator_base *base;

  public:
	using value_type = std::string;

	operator std::string() const
	{
		return {data(),size()};
	}

	Slice &operator=(const std::string &value)
	{
		const WriteOptions wops(base->flags);
		const auto &key(base->it->key());
		const leveldb::Slice val(value);
		throw_on_error(base->db->Put(wops,key,val));

		if(wops.flush)
			base->flush();

		return *this;
	}

	template<class... Args>
	Slice(iterator_base *const &base = nullptr, Args&&... args):
	leveldb::Slice(std::forward<Args>(args)...),
	base(base)
	{

	}
};


template<class T>
std::ostream &operator<<(std::ostream &s,
                         const Slice<T> &slice)
{
	s << T(slice);
	return s;
}
