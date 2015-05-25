/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


struct Options : leveldb::Options
{
	Options(leveldb::Cache *const &cache              = nullptr,
	        const leveldb::Comparator *const &cmp     = nullptr,
	        const leveldb::FilterPolicy *const &fp    = nullptr,
	        const leveldb::CompressionType &ct        = leveldb::kSnappyCompression,
	        const size_t &block_size                  = 32768,
	        const bool &create_if_missing             = true,
	        const size_t &write_buffer_size           = 4 * (1024 * 1024),
	        const size_t &max_open_files              = 1024)
	{
		this->comparator         = cmp;
		this->filter_policy      = fp;
		this->compression        = ct;
		this->block_cache        = cache;
		this->block_size         = block_size;
		this->write_buffer_size  = write_buffer_size;
		this->max_open_files     = max_open_files;
		this->create_if_missing  = create_if_missing;
	}
};


enum Flag : uint_fast8_t
{
	CACHE      = 0x01,          // ReadOptions.fill_cache
	SNAPSHOT   = 0x02,          // ReadOptions.snapshot
	VERIFY     = 0x04,          // ReadOptions.verify_checksums
	FLUSH      = 0x08,          // WriteOptions.flush
	SYNC       = 0x10,          // WriteOptions.sync
	LOWER      = 0x20,          // lower_bound indicator when seeking by key
	UPPER      = 0x40,          // upper_bound indicator when seeking by key
};


struct WriteOptions : leveldb::WriteOptions
{
	// Added for toggling iterator reset() or any similar semantic
	bool flush;

	WriteOptions(const Flag &flags):
	flush(flags & Flag::FLUSH)
	{
		this->sync = flags & Flag::SYNC;
	}
};


struct ReadOptions : leveldb::ReadOptions
{
	ReadOptions(const Flag &flags, const leveldb::Snapshot *const &snap = nullptr)
	{
		this->verify_checksums  = flags & Flag::VERIFY;
		this->fill_cache        = flags & Flag::CACHE;
		this->snapshot          = (flags & Flag::SNAPSHOT)? snap : nullptr;
	}
};
