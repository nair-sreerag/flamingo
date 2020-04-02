#ifndef UTIL_H
#define UTIL_H

#define U_MIN(A, B) (((A) < (B)) ? (A) : (B))
#define U_MAX(A, B) (((A) > (B)) ? (A) : (B))

#define strcmpws(S1, S2) (strcmp(trim(S1), S2) == 0)

enum str2dbl_errno {
	S2D_SUCCESS,
	S2D_OVERFLOW,
	S2D_UNDERFLOW,
	S2D_INCONVERTIBLE
};

enum str2dbl_errno str2dbl(double *out, char *s);
char *trim(char *str);

#endif
