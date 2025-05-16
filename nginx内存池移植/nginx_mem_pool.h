#pragma once

#include<cstdlib>
#include<cstring>
/*
��ֲnginx�ڴ�أ�OOPʵ��
*/
//�����ض���
using u_char = unsigned char;
//ngx_uint_t
using ngx_uint_t = unsigned int;
/*
����С���ڴ���ڴ������ͷ����Ϣ
*/
//����ǰ������
struct ngx_pool_s;

struct ngx_pool_data_t {
	u_char* last; //С���ڴ�ؿ����ڴ����ʼ��ַ
	u_char* end;  //С���ڴ�صĽ�����ַ
	ngx_pool_s* next;//ָ����һ��С���ڴ�ص�ָ��
	ngx_uint_t failed;//��¼����ʧ�ܵĴ���
};
/*
����ڴ�ص�ͷ����Ϣ
*/

struct ngx_pool_large_s {
	ngx_pool_large_s* next;//ָ����һ������ڴ��ָ��
	void* alloc;//��������ȥ�Ĵ���ڴ����ʼ��ַ
};
/*
���������ص�������������
*/
typedef void(*ngx_pool_cleanup_pt)(void* data);

struct ngx_pool_cleanup_s {
	ngx_pool_cleanup_pt handler;//������һ������ָ�룬������������Ļص�����
	void* data;//���ݸ��ص������Ĳ���
	ngx_pool_cleanup_s* next;//���е�cleanup������������һ�������У�nextָ��ָ����һ��cleanup����
};
/*
ngx�ڴ�ص�ͷ����Ϣ�͹����Ա��Ϣ
*/
struct ngx_pool_s {
	ngx_pool_data_t d; //�ڴ�����ݽṹ
	size_t max; //�洢����С���ڴ滹�Ǵ���ڴ�ķֽ���
	ngx_pool_s *current; //ָ���һ���ṩС���ڴ������ڴ��
	ngx_pool_large_s *large; //ָ�����ڴ��ָ��
	ngx_pool_cleanup_s *cleanup; //ָ������Ԥ�Ƶ���������ص����������
};
//�ڴ����ĺ궨��
//ngx_align(d,a) ����d���뵽a�ı���
#define ngx_align(d,a) (((d) + (a - 1)) & ~(a - 1))
//ngx_align_ptr(p,a) ����ָ��p���뵽a�ı���
#define ngx_align_ptr(p,a) (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))
//Ĭ��һ��ҳ��Ĵ�С 4k
const int ngx_pagesize = 4096;
//ngxС���ڴ�ص��������ڴ�
const int NGX_MAX_ALLOC_FROM_POOL = ngx_pagesize - 1;
//Ĭ�ϵ��ڴ�ش�С
const int NGX_DEFAULT_POOL_SIZE = 16 * 1024; // 16k
//�ڴ�ش�С����16�ֽڶ���
const int NGX_POOL_ALIGNMENT = 16; // 16�ֽڶ���
//ngxС���ڴ����С��size������NGX_MIN_POOL_ALIGNMENT�ı���
const int NGX_MIN_POOL_SIZE = ngx_align((sizeof(ngx_pool_s) + 2 * sizeof(ngx_pool_large_s)), NGX_POOL_ALIGNMENT);
//С���ڴ�ص���С�����С
const int NGX_ALIGNMENT = sizeof(unsigned long);
class ngx_mem_pool {
public:
	//����ָ��size���ڴ�أ�����С���ڴ�صĴ�С������һ��ҳ��Ĵ�С
	void* ngx_create_pool(size_t size);
	//�����ڴ��ֽڶ��룬���ڴ������size��С���ڴ�
	void *ngx_palloc(size_t size); //����ָ����С���ڴ�
	//�������ڴ��ֽڶ���
	void* ngx_pnalloc(size_t size);
	//����ngx_pallocʵ���ڴ���䣬���ҳ�ʼ��0
	void* ngx_pcalloc(size_t size);
	//�ͷŴ���ڴ�
	void ngx_pfree(void* p);
	//�ڴ����ú���
	void ngx_reset_pool();
	//�ڴ�����ٺ���
	void ngx_destroy_pool();
	//�������ص���������
	ngx_pool_cleanup_s* ngx_pool_cleanup_add(size_t size);



private:
	ngx_pool_s* pool_; //ָ��ngx�ڴ�ص�ָ��
	//С���ڴ����
	void* ngx_palloc_small(size_t size, ngx_uint_t align);
	//����ڴ����
	void* ngx_palloc_large(size_t size);
	//�����µ�С���ڴ��
	void* ngx_palloc_block(size_t size);
	//�����ڴ�
	void* ngx_alloc(size_t size);
	
	void* ngx_calloc(size_t size);

	void ngx_memzero(void* p, size_t size);
};