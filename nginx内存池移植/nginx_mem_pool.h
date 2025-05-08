#pragma once


/*
移植nginx内存池，OOP实现
*/
//类型重定义
using u_char = unsigned char;
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
	size_t max;
	ngx_pool_s *current; //指向当前内存池的指针
	ngx_pool_large_s *large; //指向大块内存的指针
	ngx_pool_cleanup_s *cleanup; //指向清理函数的指针
};
class ngx_mem_pool {
public:
private:
	ngx_pool_s*pool_; //指向ngx内存池的指针
};