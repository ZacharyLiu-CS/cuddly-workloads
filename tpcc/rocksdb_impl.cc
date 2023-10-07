//
// rocksdb_impl.cc
//
// Created by Zacharyliu-CS on 09/28/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//

#include "rocksdb_impl.h"
#include "rocksdb/cache.h"
#include "rocksdb/filter_policy.h"
#include "rocksdb/iterator.h"
#include "rocksdb/statistics.h"
#include "rocksdb/table.h"
#include "logging.h"

using namespace TPCC;

RocksDBImpl::RocksDBImpl(std::string dbpath) {

  options.create_if_missing = true;
  options.use_direct_reads = true;
  options.use_direct_io_for_flush_and_compaction = true;
  options.disable_auto_compactions = false;
  options.max_background_compactions = 4;
  options.max_background_jobs = 4;
  options.write_buffer_size = 64 << 20;
  options.target_file_size_base = 16 << 20;
  options.env = rocksdb::NewDCPMMEnv(rocksdb::DCPMMEnvOptions());

  // setup the pmem configuration
  std::string pmem_rocksdb_path = dbpath;
  // configure for pmem kv sepeartion
  options.wal_dir = pmem_rocksdb_path + "/wal";
  options.dcpmm_kvs_enable = true;
  options.dcpmm_kvs_mmapped_file_fullpath = pmem_rocksdb_path + "/kvs";
  options.dcpmm_kvs_mmapped_file_size = 1024*1024*1024;
  options.dcpmm_kvs_value_thres = 64;  // minimal size to do kv sep
  options.dcpmm_compress_value = false;
  options.allow_mmap_reads = true;
  options.allow_mmap_writes = true;
  options.allow_dcpmm_writes = true;

  rocksdb::BlockBasedTableOptions block_options;
  block_options.filter_policy.reset(
      rocksdb::NewBloomFilterPolicy(10));
  block_options.block_cache = rocksdb::NewLRUCache(500<<20);

  options.table_factory.reset(
      rocksdb::NewBlockBasedTableFactory(block_options));
  options.statistics = rocksdb::CreateDBStatistics();

  rocksdb::Status s = rocksdb::DB::Open(options, dbpath, &db_);
  if (!s.ok()) {
    LOG("init rocksdb failed!");
  }
}
int RocksDBImpl::Put(uint64_t key, uint8_t* value, uint32_t size) {return 1;}
int RocksDBImpl::Get(uint64_t key, uint8_t* value) {return 1;}