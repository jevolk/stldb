/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


template<class T>
struct Slice : leveldb::Slice
{
	operator T() const;
	template<class... Args>	Slice(Args&&... args);
};


template<class T>
template<class... Args>
Slice<T>::Slice(Args&&... args):
leveldb::Slice(std::forward<Args>(args)...)
{

}


template<class T>
Slice<T>::operator T()
const
{
	using type_t = typename std::remove_cv<T>::type;
	return boost::lexical_cast<type_t>(data(),size());
}


template<class T>
std::ostream &operator<<(std::ostream &s,
                         const Slice<T> &slice)
{
	s << T(slice);
	return s;
}
