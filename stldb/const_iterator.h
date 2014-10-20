/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


template<class T>
class const_iterator : public iterator_base<T>
{
	mutable T t;

  public:
	T &operator*() const;
	T *operator->() const;

	const_iterator(const leveldb::DB *const &db,
	               const bool &snap = false,
	               const bool &cache = false);
};


template<class T>
const_iterator<T>::const_iterator(const leveldb::DB *const &db,
                                  const bool &snap,
                                  const bool &cache):
iterator_base<T>(const_cast<leveldb::DB *>(db),snap,cache)
{

}


template<class T>
T *const_iterator<T>::operator->()
const
{
	operator*();
	return &t;
}


template<class T>
T &const_iterator<T>::operator*()
const
{
	this->t = {this->it->key(),this->it->value()};
	return t;
}
