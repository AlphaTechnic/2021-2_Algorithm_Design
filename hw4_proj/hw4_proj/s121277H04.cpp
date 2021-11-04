#include <iostream>
#include <math.h>

void merge(int low, int mid, int high, double* X, unsigned* Xid, unsigned* TMP) {
	int i = low;
	int j = mid + 1;
	int k = low;

	// X[Xid[i]]와 X[Xid[j]]를 비교하여, 작은 값을 가지는 인덱스를 TMP에 복사
	while (i <= mid && j <= high) {
		if (X[Xid[i]] <= X[Xid[j]]) {
			TMP[k++] = Xid[i++];
		}
		else {
			TMP[k++] = Xid[j++];
		}
	}

	// 잔여 원소들 TMP[]에 할당
	while (i <= mid) {
		TMP[k++] = Xid[i++];
	}
	while (j <= high) {
		TMP[k++] = Xid[j++];
	}

	// copy TMP to Xid
	for (int l = low; l <= high; l++) {  
		Xid[l] = TMP[l];
	}
}

void mergesort(int low, int high, double* X, unsigned* Xid, unsigned* TMP) {
	if (low >= high) return;

	int mid = (low + high) / 2;
	mergesort(low, mid, X, Xid, TMP);
	mergesort(mid + 1, high, X, Xid, TMP);
	merge(low, mid, high, X, Xid, TMP);
}

void sortXid(double* X, unsigned* Xid, unsigned* TMP, unsigned N) {
	mergesort(0, N - 1, X, Xid, TMP);


}

