/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


template<class T>
class const_iterator : public iterator_base,
                       public std::iterator<std::bidirectional_iterator_tag,T>
{
	mutable T t;

  public:
	const T &operator*() const;
	const T *operator->() const;

	T &operator*();
	T *operator->();

	template<class... Args> const_iterator(const leveldb::DB *const &db, Args&&... args);
};


template<class T>
template<class... Args>
const_iterator<T>::const_iterator(const leveldb::DB *const &db,
                                  Args&&... args):
iterator_base(const_cast<leveldb::DB *>(db),std::forward<Args>(args)...)
{

}


template<class T>
T *const_iterator<T>::operator->()
{
	operator*();
	return &t;
}


template<class T>
T &const_iterator<T>::operator*()
{
	t =
	{
		{ nullptr, this->it->key()   },
		{ nullptr, this->it->value() },
	};

	return t;
}


template<class T>
const T *const_iterator<T>::operator->()
const
{
	operator*();
	return &t;
}


template<class T>
const T &const_iterator<T>::operator*()
const
{
	t =
	{
		{ nullptr, this->it->key()   },
		{ nullptr, this->it->value() },
	};

	return t;
}
