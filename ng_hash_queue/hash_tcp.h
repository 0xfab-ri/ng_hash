#ifndef _HASH_TCP_H_
#define _HASH_TCP_H_

#ifdef _KERNEL
    #include "hash.h"
#endif

#define HASH_TCP_SIZE_INCREASE 1
#define HASH_TCP_SIZE_DECREASE 0

#define MIN_TCP_SIZE (1 << 8) //256. ������ ������ ���� �������� ������!
#define MAX_TCP_SIZE (1 << 14) //16384. ������ ������ ���� �������� ������!

/*
 * ���������� ����. 
 */
#define TCP_HASH(saddr,sport,daddr,dport,mask)		( (((const u_int32_t )(saddr)) \
							^((const u_int32_t )(sport)) \
							^((const u_int32_t )(daddr)) \
							^((const u_int32_t )(dport)))&(mask) )

// ����������� �������� ������

/*
*��������� �������� �������, � ������� 
*�������� ������ � ���� ����-��������,
*��������� �� ��������� ������� �������
*/
struct tcp_hash_el {
    u_int32_t hash; //��� ��������
    int value; //����� �������, � ������� ���������� ��������� �����
};

/*
*C��������, ����������� ���-�������.
*������������� � ���� ������ ���������� �� �������
*���������, ������ �������, ���������� ��������� � ������
*��������
*/
struct tcp_hash_table {
    struct tcp_hash_el **elements; //�������� �������
    size_t hash_size;	//������� ������
    unsigned int hashMask; //����� ��� �����������
    int num_elements; //������� ���������� ���������
    double loadfactor; //�������� �������� �������
};

// ��������� �������

/*
 *������������� ���-������� �������� ���������
 */
int init_tcp_hash_table __P((struct tcp_hash_table*, size_t ));

/*
 * ������������� �� ���������
 */
int init_default_tcp_hash_table __P((struct tcp_hash_table*));

/*
 *������� �������� ������, ������������ ��� �������
 */
int delete_tcp_hash_table __P((struct tcp_hash_table*));

/*
 *���������� �������� � ��� �������. � ������ �������� ���������� ���������� ����
 */
int add_tcp_hash_element __P((struct tcp_hash_table*, u_int32_t hash, int));

/*
 * ������� ������� � ��� ������� � �������� ������ � ���������
 */
struct tcp_hash_el* create_tcp_hash_element __P((u_int32_t, int));

/**
 * ������� ��������� ������ � ���� ���� �������� ������ ��� �������,
 * �� �������� ������� rehash();
 */   
void try_rehash_tcp(struct tcp_hash_table *t);

/*������� �������� �������� � �������� ����� �� �������. 
 * � ������ ������ ���������� ����
 */
int remove_tcp_hash_element __P((struct tcp_hash_table*, u_int32_t));

/*
 * ������� ��� ��������
 */

void remove_all_tcp_hash_elements __P((struct tcp_hash_table *));

/*
 * ������� ���������� �������� ������ � ������� �� �����.
 * � ��������� ����������� ��� ������� ��� ����� ����������, ��
 * ������� ���������� ��������� �����.
 * ���������� -1 � ������ ����������
 */
int get_queue_num __P((struct tcp_hash_table*, u_int32_t));

/*
 *������� ��������������� � ��������� ������� �������
 *���������� ���� � ������ ��������� ����������
 */
int rehash_tcp_hash_table __P((struct tcp_hash_table *, int));

/*
 *������� ��������� ��������� ��������. � ������ ���������� ����� ���������,
 *���������� ��������� ������� �������; ���������� 0 � ������ ������;
 */
int setloadfactor_tcp_hash_table __P((struct tcp_hash_table *, double));

#endif /* _HASH_TCP_H_ */
