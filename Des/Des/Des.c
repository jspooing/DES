#include<stdio.h>


int KEY = 0b1100110101;
int subKey[2];
int P10[10] = { 3,5,2,7,4,10,1,9,8,6 };
int P8[8] = { 6,3,7,4,8,5,10,9 };

int IP[8] = { 2,6,3,1,4,8,5,7 };
int IPINVERSE[8] = { 4,1,3,5,7,2,8,6 };
int EP[8] = { 4,1,2,3,2,3,4,1 };
int sample[8] = { 1,2,3,4,5,6,7,8 };

int S0[4][4] = { { 1,0,3,2 },{ 3,2,1,0 },{ 0,2,1,3 },{ 3,1,3,2 } };
int S1[4][4] = { { 0,1,2,3 },{ 2,0,1,3 },{ 3,0,1,0 },{ 2,1,0,3 } };

int P4[4] = { 2,4,3,1 };

int getBit(int n, int r_size, int p) {
	int i;
	int c = r_size - p;
	int mask = 1;
	for (i = 0; i < c; i++)
		mask <<= 1;
	if (n&mask)return 1;
	else return 0;
}

int permutation(int n, int r_size, int n_size, int *p) {
	int i = 0;
	int newn = 0;
	for (i = 0; i < n_size; i++) {
		newn <<= 1;
		newn += getBit(n, r_size, p[i]);
	}

	return newn;
}

int leftCircularShift(int n, int n_size, int shift) {
	int carry = 0;
	int mask = 0;
	int i;
	for (i = 0; i < shift; i++) {
		mask <<= 1;
		mask++;
	}
	mask <<= (n_size - shift);
	carry = (n&mask);
	carry >>= (n_size - shift);
	n <<= shift;
	mask = 0;
	for (i = 0; i < n_size; i++) {
		mask <<= 1;
		mask++;
	}

	n = n & mask;
	n += carry;
	return n;
}

void getSubKey(int Key, int* subKey, int count) {
	Key = permutation(Key, 10, 10, P10);
	int rk, lk;
	rk = Key & 0b0000011111;
	lk = (Key & 0b1111100000) / 32;

	////////////////////////////////////////// 아래 수정 필요 
	rk = leftCircularShift(rk, 5, 1);
	lk = leftCircularShift(lk, 5, 1);
	subKey[0] = lk;
	subKey[0] <<= 5;
	subKey[0] += rk;
	subKey[0] = permutation(subKey[0], 10, 8, P8);

	rk = leftCircularShift(rk, 5, 2);
	lk = leftCircularShift(lk, 5, 2);
	subKey[1] = lk;
	subKey[1] <<= 5;
	subKey[1] += rk;
	subKey[1] = permutation(subKey[1], 10, 8, P8);
	return;
}

int getSbox(int n, int s[][4]) {
	int row, column;
	row = 2 * getBit(n, 4, 1) + getBit(n, 4, 4);
	column = 2 * getBit(n, 4, 2) + getBit(n, 4, 3);

	return s[row][column];
}

int roundFunction(int n, int c) {
	int exrn = 0;
	int s0, s1;

	exrn = permutation(n, 4, 8, EP);             //Extention/Permutation
	exrn = exrn ^ subKey[c];                           //Key값과 xor연산 
	s0 = (exrn & 0b11110000) / 16;                  //S0,S1으로 분할 
	s1 = exrn & 0b1111;
	s0 = getSbox(s0, S0);                               //SBox 연산 
	s1 = getSbox(s1, S1);
	exrn = 4 * s0 + s1;

	exrn = permutation(exrn, 4, 4, P4);                //P4연산 
	return exrn;
}

void SW(int* a, int* b) {
	int tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
	return;
}

int round(int n, int c) {
	int rn, ln;

	rn = n & 0b00001111;
	ln = (n & 0b11110000) / 16;
	ln = ln^roundFunction(rn, c);                         //ln 값을 round함수 적용한 rn값과 xor 

	SW(&rn, &ln);          //rn,ln스위치 

	n = 16 * ln + rn;
	return n;
}



void main() {
	//int i;
	int n = 0b10100011;

	printf("0~255의 정수값 입력 >>");
	scanf_s("%d", &n, sizeof(int));
	getSubKey(KEY, subKey, 2);   //키로부터 서브키 생성 

	n = permutation(n, 8, 8, IP); // Initial Permutation 
	n = round(n, 0);                 //라운드 진행 
	n = round(n, 1);
	n = leftCircularShift(n, 8, 4);        // 스위치 !
	n = permutation(n, 8, 8, IPINVERSE);

	printf("(16진수로 표기)\n%0x\n", n);


}