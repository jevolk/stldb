/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


struct Options : public leveldb::Options
{
	Options(leveldb::Cache *const cache         = nullptr,
	        const leveldb::FilterPolicy *fp     = nullptr,
	        const leveldb::CompressionType ct   = leveldb::kSnappyCompression,
	        const size_t block_size             = 16384,
	        const bool create_if_missing        = true,
	        const size_t write_buffer_size      = 4 * (1024 * 1024),
	        const size_t max_open_files         = 1024)
	{
		//this->comparator         = nullptr;
		this->filter_policy      = fp;
		this->compression        = ct;
		this->block_cache        = cache;
		this->block_size         = block_size;
		this->write_buffer_size  = write_buffer_size;
		this->max_open_files     = max_open_files;
		this->create_if_missing  = create_if_missing;
	}
};


struct WriteOptions : public leveldb::WriteOptions
{
	WriteOptions(const bool &sync = false)
	{
		this->sync = sync;
	}
};


struct ReadOptions : public leveldb::ReadOptions
{
	ReadOptions(const bool &cache                      = false,
	            const bool &verify                     = false,
	            const leveldb::Snapshot *const &snap   = nullptr)
	{
		this->verify_checksums  = verify;
		this->fill_cache        = fill_cache;
		this->snapshot          = snapshot;
	}
};
