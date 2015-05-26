/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


struct Options : leveldb::Options
{
	size_t cache_size = 0;
	size_t bloom_bits = 0;

	// This constructor is available for arguments from the user
	Options(const size_t &cache_size                  = 32 * 1024 * 1024,
	        const size_t &block_size                  = 32768,
	        const size_t &write_size                  = 4 * (1024 * 1024),
	        const size_t &bloom_bits                  = 0,
	        const size_t &max_open_files              = 1024,
	        const bool &create_if_missing             = true,
	        const leveldb::CompressionType &ct        = leveldb::kSnappyCompression):
	Options(nullptr,
	        nullptr,
	        nullptr,
	        ct,
	        cache_size,
	        block_size,
	        write_size,
	        bloom_bits,
	        max_open_files,
	        create_if_missing)
	{
	}

	// This constructor is available but generally used internally after the one above
	Options(leveldb::Cache *const &cache,
	        const leveldb::Comparator *const &cmp,
	        const leveldb::FilterPolicy *const &fp,
	        const leveldb::CompressionType &ct,
	        const size_t &cache_size,
	        const size_t &block_size,
	        const size_t &write_size,
	        const size_t &bloom_bits,
	        const size_t &max_open_files,
	        const bool &create_if_missing):
	cache_size(cache_size),
	bloom_bits(bloom_bits)
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
