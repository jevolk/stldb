/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


template<class T>
class Slice : public leveldb::Slice
{
	iterator_base *base;

	template<class U> typename
	std::enable_if<std::is_convertible<U,std::string>::value, U>::type
	cast() const
	{
		return {data(),size()};
	}

	template<class U> typename
	std::enable_if<std::is_arithmetic<U>::value, U>::type
	cast() const
	{
		using type_t = typename std::remove_cv<T>::type;
		return boost::lexical_cast<type_t>(data(),size());
	}

  public:
	operator T() const
	{
		return cast<T>();
	}

	// Note: is_trivially_copyable not available GCC ~4.9
	template<class U> typename
	std::enable_if<std::is_trivial<U>::value, Slice>::type
	&operator=(U&& t)
	{
		const auto &key = base->it->key();
		const leveldb::Slice val(reinterpret_cast<const char *>(&t),sizeof(t));
		const WriteOptions wops(base->flags);
		throw_on_error(base->db->Put(wops,key,val));

		if(wops.flush)
			base->flush();

		return *this;
	}

	template<class U> typename
	std::enable_if<!std::is_trivial<U>() && std::is_convertible<U,std::string>(), Slice>::type
	&operator=(U&& t)
	{
		const auto &key = base->it->key();
		const WriteOptions wops(base->flags);
		throw_on_error(base->db->Put(wops,key,leveldb::Slice(t)));

		if(wops.flush)
			base->flush();

		return *this;
	}

	template<class... Args>
	Slice(iterator_base *const &base = nullptr,
	      Args&&... args):
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
