#include "matrix.h"

matrix* create_matrix(unsigned int m, unsigned int n, int* numbers)
{
	/*TODO:
			-Argumente auf Gueltigkeit ueberpruefen
			-neue Matrix erstellen und zurueckgeben (Pointer!!!)
			-(das Array soll dabei zwar kopiert, aber ansonsten UNVERAENDERT abgespeichert werden.
			  Dies ist für die automatischen Tests unerlaesslich)
	*/
    matrix *mat = malloc(sizeof(matrix));
    mat -> m = m;
    mat -> n = n;
    mat -> elements = malloc(sizeof(int)*n*m);
    for (int i = 0; i < n*m; i++) {
        mat -> elements[i] = numbers[i];
    }
	return mat;
}

matrix* create_matrix_from_row(unsigned int m, unsigned int row_nr, matrix* row)
{
	/*TODO
			-Argumente auf Gueltigkeit ueberpruefen
			-Erstellen einer neuen Matrix (Initialisierung mit 0)
			-Kopieren der gegebenen Zeile an die richtige Stelle
	*/
	matrix *mat = malloc(sizeof(matrix));
    mat -> m = m;
    mat -> n = row -> n;
    mat -> elements = calloc(m*(mat->n),sizeof(int));
    for (int i = 0; i < mat->n; i++) {
        mat -> elements[(row_nr)*(mat->n)+i] = row -> elements[i];
    }
	return mat;
}

void free_matrix(matrix* matrix)
{
	/*TODO:
			-Argumente auf Gueltigkeit ueberpruefen
			-Matrix und alle dynamisch reservierten elemente der Matrix freigeben
	*/
	if (matrix == NULL) {
        return;
    }
    free(matrix -> elements);
    free(matrix);
}

matrix* duplicate_matrix(matrix* old)
{
	/*TODO:
			-Argumente auf Gueltigkeit ueberpruefen
			-Erstellen und Zurückgeben einer NEUEN Matrix mit genau den selben Parametern der gegebenen Matrix
	*/
    if (old == NULL) {
        return NULL;
    }
    matrix *new = create_matrix(old -> m, old -> n, old -> elements);
	return new;
}

void add_matrix(matrix* a, matrix* b)
{
	/*TODO:
			-Argumente auf Gueltigkeit ueberpruefen
			-Elementweises Addieren von a und b (a+b)
	*/
    if (a == NULL || b == NULL || (a -> m) != (b -> m) || (a -> n) != (b -> n)) {
        return;
    }
    

    for (int i = 0; i < a->n * a->m; i++) {
        a->elements[i] = a->elements[i]+b->elements[i];
    }
}

void subtract_matrix(matrix* a, matrix* b)
{
	/*TODO:
			-Argumente auf Gueltigkeit ueberpruefen
			-Elementweises Subtrahieren von a und b (a-b)
	*/
    if (a == NULL || b == NULL || (a -> m) != (b -> m) || (a -> n) != (b -> n)) {
        return;
    }
    

    for (int i = 0; i < a->n * a->m; i++) {
        a->elements[i] = a->elements[i]-b->elements[i];
    }	
}

int get_elem_of_matrix(matrix* matrix, unsigned int i, unsigned int j)
{
	/*TODO
		-Argumente auf Gueltigkeit ueberpruefen
		-Rueckgabe des Elements an den Indizes i und j	
	*/
    if (i < 0 || j < 0 || i > matrix->m || j > matrix->n) {
        return -1;
    }
    int elem = matrix -> elements[matrix->n*i+j];
    
	return elem;
}

int check_matrix_elements(matrix* matrix)
{
	/*TODO:
			-Argumente auf Gueltigkeit ueberpruefen
			-Pruefen, ob die Elemente der Matrix <, > oder = 0 sind
			->sobald mindestens ein Element <0 ist, soll -1 zurückgegeben werden
			->wenn alle Elemente 0 sind, soll 0 zurückgegeben werden
			->ansonsten soll 1 zurückgegeben werden
	*/

    for (int i = 0; i < matrix->n*matrix->m; i++) {
        if (matrix->elements[i] < 0) {
            return -1;
        }
        if (matrix->elements[i] > 0) {
            return 1;
        }
    }
	return 0;
}

matrix* get_row_from_matrix(matrix* mat, unsigned int m)
{
	/*TODO:
			-Argumente auf Gueltigkeit ueberpruefen
			-Rueckgabe der m-ten Zeile der Matrix (Die Zeile selbst ist auch wieder eine Matrix mit genau einer Zeile)
	*/
    
    if (mat == NULL || m >= mat->m) {
        return NULL;
    }
    
    int* x = malloc(sizeof(int)*mat->n);
    
    for(int i = 0; i < mat->n; i++) {
        x[i] = mat->elements[i+mat->n*m];
    }

    matrix *hey = create_matrix(1,mat->n,x);

	return hey;
}

void print_matrix(matrix* matrix)
{
	if(matrix == NULL)
	{
		printf("ERROR: print_matrix() | given matrix is NULL\n");
		return;
	}


	//determines the largest element (for formatting the output with field widths)
	int max = get_elem_of_matrix(matrix, 0,0);	
	for(int i = 0; i< matrix->m; i++)
	{
		for(int j = 0; j < matrix->n; j++)
		{
			int current = get_elem_of_matrix(matrix, i,j);
			max = (current>max) ? current : max;
		}
	}

	int grade = 1;	//represents the number of digits of the largest element (for formatting the output with field widths)
	while(max > 9)
	{
		max/=10;
		grade++;
	}

	//prints upper line
	printf(" ");
	for(int i = 0; i< ((grade + 1)*matrix->n +1); i++)
	{
		printf("-");
	}
	printf("\n");

	//prints body
	for(int i = 0; i< matrix->m; i++)
	{
		printf("| ");
		for(int j = 0; j < matrix->n; j++)
		{
			printf("%*i ", grade, get_elem_of_matrix(matrix, i, j));
		}
		printf("|\n");
	}

	//prints lower line
	printf(" ");
	for(int i = 0; i< ((grade + 1)*matrix->n +1); i++)
	{
		printf("-");
	}
	printf("\n");
}


/*TODO:
	Hier koennen bei Bedarf eigene Funktionen implementiert werden
*/
