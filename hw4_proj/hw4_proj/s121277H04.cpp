#include <iostream>
#include <math.h>

void merge(int low, int mid, int high, double* X, unsigned* Xid, unsigned* TMP) {
	int i = low;
	int j = mid + 1;
	int k = low;

	// X[Xid[i]]�� X[Xid[j]]�� ���Ͽ�, ���� ���� ������ �ε����� TMP�� ����
	while (i <= mid && j <= high) {
		if (X[Xid[i]] <= X[Xid[j]]) {
			TMP[k++] = Xid[i++];
		}
		else {
			TMP[k++] = Xid[j++];
		}
	}

	// �ܿ� ���ҵ� TMP[]�� �Ҵ�
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

