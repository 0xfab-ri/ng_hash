/*
**********************************************************************
*      ������� ��� ������ � ���-���������
*  
*
* (c) Copyright
*---------------------------------------------------------------------
* ���� hash_funcs.c - ������ �������
* �����: ������� �������
*
* $Id$
**********************************************************************

*/
#include "hash_tcp.h"


/**
 * ������� ���������� ��������.
 */
static int add_tcp_hash_element_internal __P((struct tcp_hash_table *, u_int32_t hash , int , int ));

/*
 *������� ������������� ��� ������� �������� ��������
 *� ������ �������� ���������� ���������� ����
 */
int init_tcp_hash_table(struct tcp_hash_table *t, size_t size) {

    if (size < MIN_TCP_SIZE || size > MAX_TCP_SIZE) {
        size = MIN_TCP_SIZE;
    }

	if ((N_MALLOC(t->elements, struct tcp_hash_el** , size * sizeof(struct tcp_hash_el*)) ) == NULL) {
		return -1;
	}
	
	t->hash_size = size;
	t->num_elements = 0;
	t->loadfactor = 0.9;
	t->hashMask = size - 1;
	return 0;
}

/*
 * ������������� �� ���������
 */
int init_default_tcp_hash_table(struct tcp_hash_table *t) {
	return init_tcp_hash_table(t, MIN_TCP_SIZE);
}

/*
 *������� ��������� ��������� ��������. � ������ ���������� ����� ���������,
 *���������� ��������� ������� �������;
 */
int setloadfactor_tcp_hash_table(struct tcp_hash_table *t, double loadfactor) {
	if(loadfactor <= 0 || loadfactor > 1) {
		return -1;
	}
	t->loadfactor = loadfactor;
	return 0;
}

/*
 *������������ ������, ������� ��� ��� �������
 */
int delete_tcp_hash_table(struct tcp_hash_table *t) {
    int i = 0;
	struct tcp_hash_el* el;
	for(i = 0; i < t->hash_size; i++) {
		el = t->elements[i];
		N_FREE(el);
	}

	N_FREE(t->elements);
	return 0;
}


/*
 * ������� ���������� �������� � ��� �������.
 *  -1 � ������ �������.
 *  -2 E��� ������� �������.
 *  TODO ������� � ���������
 */
int add_tcp_hash_element(struct tcp_hash_table *t, u_int32_t hash, int value) {
	return add_tcp_hash_element_internal(t, hash, value, 1);
}

/*
 * ������� ���������� �������� � ��� �������.
 *  -1 �� ��������� ��� ������������ �������.
 *  -2 E��� ������� �������.
 *  -3 ������ ��������� ������.
 *  TODO ������� � ���������
 * isCheck - ���� ���� ���������, �� ���������� ��������� ���������������
 */
static int add_tcp_hash_element_internal(struct tcp_hash_table *t, u_int32_t hash, int value, int isCheck) {
		if (t->num_elements >= MAX_TCP_SIZE) {
		return -1;
	}

    //������ ��������� ����� ������� �� �������, ���� ��� ���, �� ��������, ���� ����, �� ��������
    //� �������
    struct tcp_hash_el *el = t->elements[hash];
	struct tcp_hash_el* new;

    if(el != NULL) {
	if (el->value == value) {
		return -1; // �� ��������� ��� ������������ �������
	} else {
		el->value = value; //�������� ������� � �������
		return -2;
	}
    
    } else {
	//�������� ������� ��� ������
	new = create_tcp_hash_element(hash, value);
        if (new == NULL){
            return -3; //������ ��������� ������
        }
    	t->elements[hash] = new;
    }

    t->num_elements++;

	if (isCheck) {
		try_rehash_tcp(t);
	}
    return 0;
}

/**
 * ������� ��������� ������ � ���� ���� �������� ������ ��� �������,
 * �� �������� ������� rehash();
 */
void try_rehash_tcp(struct tcp_hash_table *t) {
	double factor = (double) t->num_elements / t->hash_size;
	if( (factor > t->loadfactor) && (t->hash_size < MAX_TCP_SIZE)) {
        rehash_tcp_hash_table(t, HASH_TCP_SIZE_INCREASE);
    } 
/*
	else if (t->hash_size > MIN_TCP_SIZE && (factor < (1.0 - t->loadfactor)) ){
        rehash(t, HASH_TCP_SIZE_DECREASE);
    }
*/
}

/*
 * ������� ������� � ��� ������� � �������� ������ � ���������
 */
struct tcp_hash_el* create_tcp_hash_element(u_int32_t hash, int value) {
    struct tcp_hash_el *new_el;

    if(N_MALLOC(new_el, struct tcp_hash_el*, sizeof(struct tcp_hash_el)) == NULL ){
      return NULL;
    }
    //�������� ����� �������
    new_el->value = value;
    new_el->hash = hash; // ��������� ���
    return new_el;
}


/*
 *������� �������� �������� �� �������. � ������ ������ ���������� 0
 */
int remove_tcp_hash_element(struct tcp_hash_table *t, u_int32_t hash) {


    struct tcp_hash_el* el = *(t->elements + hash);
    N_FREE(el);
	t->elements[hash] = NULL;
	
	t->num_elements--;

	try_rehash_tcp(t);

	return 0;
}

//�������� ���� ���������
void remove_all_tcp_hash_elements (struct tcp_hash_table *t) {
	int i;
   
	for(i = 0; i < t->hash_size; i++) {
		N_FREE(t->elements[i]);
		t->elements[i] = NULL;
	}
}


/*
 * ������� ���������� �������� ������ � ������� �� �����.
 * � ��������� ����������� ��� ������� ��� ����� ����������, �� 
 * ������� ���������� ��������� �����.
 * ���������� -1 � ������ ����������
 */
int get_queue_num(struct tcp_hash_table *t, u_int32_t hash) {
    struct tcp_hash_el* el = t->elements[hash];
	if (el != NULL)
		return el->value;
	else
		return -1;
}

/*
 *������� ��������������� � ��������� ������� �������
 *���������� ���� � ������ ��������� ����������
 */
int rehash_tcp_hash_table (struct tcp_hash_table *t, int direction) {

	if(t->hash_size >= MAX_TCP_SIZE) {
		return -1;
    }

	int oldsize = t->hash_size;
	int newsize;

	if(direction == HASH_TCP_SIZE_DECREASE) {
		newsize = t->hash_size >> 1;
		newsize = (newsize < MIN_TCP_SIZE) ? MIN_TCP_SIZE: newsize;
	} else if(direction == HASH_TCP_SIZE_INCREASE) {
		newsize = t->hash_size << 1;
		newsize = (newsize > MAX_TCP_SIZE) ? MAX_TCP_SIZE: newsize;
	} else {
		return -1;
	}

	struct tcp_hash_el **oldelements = t->elements;
	struct tcp_hash_el **newelements;
	
//������� ������ ��� ����� �������
	if(N_MALLOC(newelements, struct tcp_hash_el**, newsize*sizeof(struct tcp_hash_el*)) == NULL) {
		return -1;
	}	
	printf("rehashing to %d", newsize);

	t->elements = newelements;
	t->hash_size = newsize;
	t->num_elements = 0;
	t->hashMask = newsize - 1;
	
	int i;
	struct tcp_hash_el* el;

//��������� �������� �� ������ � �����
	for(i = 0; i < oldsize; i++) {
        el = oldelements[i];
		if(el != NULL) {
			add_tcp_hash_element_internal(t, el->hash, el->value, 0);
		}
	}
//��������� ����y� �������
	N_FREE(oldelements);

	return 0;
}
