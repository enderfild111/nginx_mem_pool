#include "nginx_mem_pool.h"


//分配内存
void* ngx_mem_pool::ngx_alloc(size_t size) {
	void* p;
	p = malloc(size);
	return p;
}
//清零内存
void ngx_mem_pool::ngx_memzero(void* p, size_t size) {
	std::memset(p, 0, size);
}
//分配内存并清零
void* ngx_mem_pool::ngx_calloc(size_t size) {
	void* p;
	p = ngx_alloc(size);
	if (p) {
		ngx_memzero(p,size);
	}
	return p;
}
//创建内存池
void* ngx_mem_pool::ngx_create_pool(size_t size) {
	ngx_pool_s *p;
	p = (ngx_pool_s*)ngx_alloc(size);
	if (p == nullptr) {
		return nullptr;
	}
	p->d.last = (u_char*)p + sizeof(ngx_pool_s);
	p->d.end = (u_char*)p + size;
	p->d.next = nullptr;
	p->d.failed = 0;
	size = size - sizeof(ngx_pool_s);
	p->max = (size < NGX_DEFAULT_POOL_SIZE) ? NGX_DEFAULT_POOL_SIZE : size;
	p->current = p;
	p->large = nullptr;
	p->cleanup = nullptr;
	return p;
}

void* ngx_mem_pool::ngx_palloc(size_t size) {
	if (size <= pool_->max) {
		return ngx_palloc_small(size,1);
	}
	else {
		return ngx_palloc_large(size);
	}
}

void* ngx_mem_pool::ngx_pnalloc(size_t size) {
	if (size <= pool_->max) {
		return ngx_palloc_small(size, 0);
	}
	else {
		return ngx_palloc_large(size);
	}
}

void* ngx_mem_pool::ngx_palloc_small(size_t size, ngx_uint_t align) {
	u_char* m;
	ngx_pool_s* p;
	p = pool_->current;
	do {
		m = p->d.last;
		if (align) {
			m = ngx_align_ptr(m, NGX_ALIGNMENT);
		}
		if ((size_t)(p->d.end - m) >= size) {
			p->d.last = m + size;
			return m;
		}
		p = p->d.next;

	} while (p);
	return ngx_palloc_block(size);
}

void* ngx_mem_pool::ngx_palloc_block(size_t size) {
	u_char* m;
	size_t psize;
	ngx_pool_s *p, *newpool;

	psize = (size_t)(pool_->d.end - (u_char*)pool_);

	m = (u_char*)ngx_alloc(psize);
	if (m == nullptr) {
		return nullptr;
	}

	newpool = (ngx_pool_s*)m;

	newpool->d.end = m + psize;
	newpool->d.next = nullptr;
	newpool->d.failed = 0;

	m += sizeof(ngx_pool_data_t);
	m = ngx_align_ptr(m, NGX_ALIGNMENT);
	newpool->d.last = m + size;

	for (p = pool_->current; p->d.next; p = p->d.next) {
		if (p->d.failed++ > 4) {
			pool_->current = p->d.next;
		}
	}

	p->d.next = newpool;

	return m;
}

void* ngx_mem_pool::ngx_palloc_large(size_t size) {
	void* p;
	ngx_uint_t         n;
	ngx_pool_large_s* large;

	p = ngx_alloc(size);
	if (p == nullptr) {
		return nullptr;
	}

	n = 0;

	for (large = pool_->large; large; large = large->next) {
		if (large->alloc == nullptr) {
			large->alloc = p;
			return p;
		}

		if (n++ > 3) {
			break;
		}
	}

	large = (ngx_pool_large_s*)ngx_palloc_small(sizeof(ngx_pool_large_s), 1);
	if (large == nullptr) {
		free(p);
		return nullptr;
	}

	large->alloc = p;
	large->next = pool_->large;
	pool_->large = large;

	return p;
}

void ngx_mem_pool::ngx_pfree(void* p) {
	ngx_pool_large_s* l;

	for (l = pool_->large; l; l = l->next) {
		if (p == l->alloc) {
			free(l->alloc);
			l->alloc = nullptr;
			return;
		}
	}
}