#pragma once


/*
��ֲnginx�ڴ�أ�OOPʵ��
*/
//�����ض���
using u_char = unsigned char;
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
	size_t max;
	ngx_pool_s *current; //ָ��ǰ�ڴ�ص�ָ��
	ngx_pool_large_s *large; //ָ�����ڴ��ָ��
	ngx_pool_cleanup_s *cleanup; //ָ����������ָ��
};
class ngx_mem_pool {
public:
private:
	ngx_pool_s*pool_; //ָ��ngx�ڴ�ص�ָ��
};