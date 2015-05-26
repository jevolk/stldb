/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


enum Flag : uint_fast8_t
{
	NONE       = 0x00,
	CACHE      = 0x01,          // ReadOptions.fill_cache
	SNAPSHOT   = 0x02,          // ReadOptions.snapshot
	VERIFY     = 0x04,          // ReadOptions.verify_checksums
	FLUSH      = 0x08,          // WriteOptions.flush
	SYNC       = 0x10,          // WriteOptions.sync
	LOWER      = 0x20,          // lower_bound indicator when seeking by key
	UPPER      = 0x40,          // upper_bound indicator when seeking by key
};


inline
Flag operator~(const Flag &a)
{
	using flag_t = std::underlying_type<Flag>::type;

	return Flag(~static_cast<const flag_t &>(a));
}


inline
Flag operator|(const Flag &a, const Flag &b)
{
	using flag_t = std::underlying_type<Flag>::type;

	return Flag(static_cast<const flag_t &>(a) | static_cast<const flag_t &>(b));
}


inline
Flag operator&(const Flag &a, const Flag &b)
{
	using flag_t = std::underlying_type<Flag>::type;

	return Flag(static_cast<const flag_t &>(a) & static_cast<const flag_t &>(b));
}
