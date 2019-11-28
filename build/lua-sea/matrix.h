#ifndef FISH_MATRIX_H
#define FISH_MATRIX_H

struct matrix {
	/* The matrix format is :
	 *
	 * | m[0] m[1] 0 |
	 * | m[2] m[3] 0 |
	 * | m[4] m[5] 1 |
	 *
	 * The format of the coordinates of a point is:
	 *
	 * | x y 1 |
	 *
	 * So, if you want to transform a point p with a matrix m, do:
	 *
	 * p * m
	 *
	 */
	int m[6];
};

static inline void
matrix_mul(struct matrix *mm, const struct matrix *mm1, const struct matrix *mm2) {
	int *m = mm->m;
	const int *m1 = mm1->m;
	const int *m2 = mm2->m;
	m[0] = (m1[0] * m2[0] + m1[1] * m2[2]) /1024;
	m[1] = (m1[0] * m2[1] + m1[1] * m2[3]) /1024;
	m[2] = (m1[2] * m2[0] + m1[3] * m2[2]) /1024;
	m[3] = (m1[2] * m2[1] + m1[3] * m2[3]) /1024;
	m[4] = (m1[4] * m2[0] + m1[5] * m2[2]) /1024 + m2[4];
	m[5] = (m1[4] * m2[1] + m1[5] * m2[3]) /1024 + m2[5];
}

static inline void
matrix_mul_point(struct matrix *mat, double* x, double* y) {
	int *m = mat->m;
	double xx = *x;
	double yy = *y;
	
	*x = (xx * m[0] + yy * m[2])/1024 + m[4];
	*y = (xx * m[1] + yy * m[3])/1024 + m[5];
}

static inline void
matrix_identity(struct matrix *mm) {
	int *mat = mm->m;
	mat[0] = 1024;
	mat[1] = 0;
	mat[2] = 0;
	mat[3] = 1024;
	mat[4] = 0;
	mat[5] = 0;
}

int matrix_inverse(const struct matrix *mm, struct matrix *mo);

struct srt {
	int offx;
	int offy;
	int scale;
	int rot;
};

void matrix_srt(struct matrix *mm, const struct srt *srt);
void matrix_rot(struct matrix *m, int rot);
void matrix_scale(struct matrix *m, int sx, int sy);
void matrix_sr(struct matrix *mat, int sx, int sy, int d);
void matrix_rs(struct matrix *mat, int sx, int sy, int d);

#endif
