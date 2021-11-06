#include <iostream>
#include <math.h>

// cp_func
double closestPairDC(
	unsigned L, unsigned R,  // current leftmost and rightmost indices
	unsigned* pt1, unsigned* pt2,  // closest pair points indices
	double* X, double* Y,  // (x, y) positions array(input)
	unsigned* Xid,  // point index array(sorted by x coordinates)
	unsigned* Yid,  // point index array(gradually sorted by y coordinates)
	unsigned* TMP,  // temporal index array
	unsigned THR  // threshord value
);

// sort
void sortXid(double* X, unsigned* Xid, unsigned* TMP, unsigned N);
void merge_sort(double* X, unsigned* Xid, unsigned* TMP, int low, int high);
void merge(double* X, unsigned* Xid, unsigned* TMP, int low, int mid, int high);

// utils
double get_dsq(double x1, double y1, double x2, double y2);



// definitions
double get_dsq(double x1, double y1, double x2, double y2) {
	return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

double closestPairDC(
	unsigned L, unsigned R, unsigned* pt1, unsigned* pt2, double* X, double* Y, 
	unsigned* Xid, unsigned* Yid, unsigned* TMP, unsigned THR
) {
	double MAX = DBL_MAX;
	if (R - L + 1 <= THR) {  // ���� ������ THR���� �۰� ���� ��� => brute force
		if (THR <= 1) {					// means one point not two
			*pt1 = *pt2 = Yid[L] = Xid[L];			
			return MAX;
		}
		
		// 1. Xid[L:R]�� Yid[L:R]�� ����
		for (unsigned i = L; i <= R; i++) {
			Yid[i] = Xid[i];
		}
		
		// 2. Yid[L:R]�� �ش� ������ y��ġ ������������ O(n^2) �������� 
		for (unsigned i = L; i <= R - 1; i++) {
			double mnv = Y[Yid[i]];
			int idx_of_mnv = i;

			for (unsigned j = i + 1; j <= R; j++) {
				if (Y[Yid[j]] < mnv) {
					mnv = Y[Yid[j]];
					idx_of_mnv = j;
				}
			}
			// swap
			unsigned tmp = Yid[i];
			Yid[i] = Yid[idx_of_mnv];
			Yid[idx_of_mnv] = tmp;
		}

		// 3. get dsq_min <- by brute force method
		double dsq_min = MAX;
		*pt1 = Yid[L];
		*pt2 = Yid[R];
		for (unsigned i = L; i <= R - 1; i++) {
			for (unsigned j = i + 1; j <= R; j++) {
				double tmp = get_dsq(X[Yid[i]], Y[Yid[i]], X[Yid[j]], Y[Yid[j]]);
				if (tmp < dsq_min) {
					dsq_min = tmp;
					*pt1 = Yid[i];
					*pt2 = Yid[j];
				}
			}
		}
		return sqrt(dsq_min);
	}

	// R - L + 1 > THR �� ��� : D&C
	double d_LR;
	unsigned ptL1, ptL2, ptR1, ptR2;
	unsigned Mid = (L + R) / 2;
	

	double d_L = closestPairDC(L, Mid, &ptL1, &ptL2, X, Y, Xid, Yid, TMP, THR);  // After this line, Yid[L:M] ���ĵ�
	double d_R = closestPairDC(Mid + 1, R, &ptR1, &ptR2, X, Y, Xid, Yid, TMP, THR);  // After this line, Yid[(M+1):R] ���ĵ�
	merge(Y, Yid, TMP, L, Mid, R);  // After this line, Yid[L:R] ���ĵ�

	if (d_L < d_R) {
		*pt1 = ptL1;
		*pt2 = ptL2;
		d_LR = d_L;
	}
	else {
		*pt1 = ptR1;
		*pt2 = ptR2;
		d_LR = d_R;
	}

	// �߾� interval((x_mid - d_LR) ~ (x_mid + d_LR))�� ������ TMP�� ������.
	// Yid[]�� ���ĵǾ� ����.
	double x_mid = (X[Xid[Mid]] + X[Xid[Mid + 1]]) / 2;
	int idx = 0;
	for (unsigned i = L; i <= R; i++) {
		if (x_mid - d_LR <= X[Yid[i]] && X[Yid[i]] <= x_mid + d_LR)
			TMP[idx++] = Yid[i];
	}
	
	// �߾� x interval ((x_mid - d_LR) ~ (x_mid + d_LR))���� �ּҰŸ��� ��´�.
	double dsq_min = d_LR * d_LR;
	for (int i = 0; i < idx - 1; i++) {
		for (int j = i + 1; j < idx; j++) {
			if ((Y[TMP[j]] - Y[TMP[i]]) * (Y[TMP[j]] - Y[TMP[i]]) >= dsq_min) break;

			double tmp = get_dsq(X[TMP[i]], Y[TMP[i]], X[TMP[j]], Y[TMP[j]]);
			if (tmp < dsq_min) {
				dsq_min = tmp;
				*pt1 = TMP[i];
				*pt2 = TMP[j];
			}
		}
	}
	return sqrt(dsq_min);
}


void sortXid(double* X, unsigned* Xid, unsigned* TMP, unsigned N) {
	merge_sort(X, Xid, TMP, 0, N - 1);
}


void merge_sort(double* X, unsigned* Xid, unsigned* TMP, int low, int high) {
	if (low >= high) return;

	int mid = (low + high) / 2;
	merge_sort(X, Xid, TMP, low, mid);
	merge_sort(X, Xid, TMP, mid + 1, high);
	merge(X, Xid, TMP, low, mid, high);
}


void merge(double* X, unsigned* Xid, unsigned* TMP, int low, int mid, int high) {
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
