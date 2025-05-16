#pragma once

#include<cstdlib>
#include<cstring>
/*
移植nginx内存池，OOP实现
*/
//类型重定义
using u_char = unsigned char;
//ngx_uint_t
using ngx_uint_t = unsigned int;
/*
分配小块内存的内存池数据头部信息
*/
//类型前置声明
struct ngx_pool_s;

struct ngx_pool_data_t {
	u_char* last; //小块内存池可用内存的起始地址
	u_char* end;  //小块内存池的结束地址
	ngx_pool_s* next;//指向下一个小块内存池的指针
	ngx_uint_t failed;//记录分配失败的次数
};
/*
大块内存池的头部信息
*/

struct ngx_pool_large_s {
	ngx_pool_large_s* next;//指向下一个大块内存的指针
	void* alloc;//保存分配出去的大块内存的起始地址
};
/*
清理函数（回调函数）的类型
*/
typedef void(*ngx_pool_cleanup_pt)(void* data);

struct ngx_pool_cleanup_s {
	ngx_pool_cleanup_pt handler;//定义了一个函数指针，保存清理操作的回调函数
	void* data;//传递给回调函数的参数
	ngx_pool_cleanup_s* next;//所有的cleanup操作都被串在一个链表中，next指针指向下一个cleanup操作
};
/*
ngx内存池的头部信息和管理成员信息
*/
struct ngx_pool_s {
	ngx_pool_data_t d; //内存池数据结构
	size_t max; //存储的是小块内存还是大块内存的分界线
	ngx_pool_s *current; //指向第一个提供小块内存分配的内存池
	ngx_pool_large_s *large; //指向大块内存的指针
	ngx_pool_cleanup_s *cleanup; //指向所有预制的清理操作回调函数的入口
};
//内存对齐的宏定义
//ngx_align(d,a) 计算d对齐到a的倍数
#define ngx_align(d,a) (((d) + (a - 1)) & ~(a - 1))
//ngx_align_ptr(p,a) 计算指针p对齐到a的倍数
#define ngx_align_ptr(p,a) (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))
//默认一个页面的大小 4k
const int ngx_pagesize = 4096;
//ngx小块内存池的最大可用内存
const int NGX_MAX_ALLOC_FROM_POOL = ngx_pagesize - 1;
//默认的内存池大小
const int NGX_DEFAULT_POOL_SIZE = 16 * 1024; // 16k
//内存池大小按照16字节对齐
const int NGX_POOL_ALIGNMENT = 16; // 16字节对齐
//ngx小块内存池最小的size调整成NGX_MIN_POOL_ALIGNMENT的倍数
const int NGX_MIN_POOL_SIZE = ngx_align((sizeof(ngx_pool_s) + 2 * sizeof(ngx_pool_large_s)), NGX_POOL_ALIGNMENT);
//小块内存池的最小分配大小
const int NGX_ALIGNMENT = sizeof(unsigned long);
class ngx_mem_pool {
public:
	//创建指定size的内存池，但是小块内存池的大小不超过一个页面的大小
	void* ngx_create_pool(size_t size);
	//考虑内存字节对齐，从内存池申请size大小的内存
	void *ngx_palloc(size_t size); //分配指定大小的内存
	//不考虑内存字节对齐
	void* ngx_pnalloc(size_t size);
	//调用ngx_palloc实现内存分配，并且初始化0
	void* ngx_pcalloc(size_t size);
	//释放大块内存
	void ngx_pfree(void* p);
	//内存重置函数
	void ngx_reset_pool();
	//内存池销毁函数
	void ngx_destroy_pool();
	//添加清理回调操作函数
	ngx_pool_cleanup_s* ngx_pool_cleanup_add(size_t size);



private:
	ngx_pool_s* pool_; //指向ngx内存池的指针
	//小块内存分配
	void* ngx_palloc_small(size_t size, ngx_uint_t align);
	//大块内存分配
	void* ngx_palloc_large(size_t size);
	//分配新的小块内存池
	void* ngx_palloc_block(size_t size);
	//分配内存
	void* ngx_alloc(size_t size);
	
	void* ngx_calloc(size_t size);

	void ngx_memzero(void* p, size_t size);
};