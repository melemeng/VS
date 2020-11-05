#include "bankier.h"

void print_dimensions(matrix* m, char c);
void copy_row_in_matrix(matrix* target, unsigned int mt, matrix* source, unsigned int ms) ;
matrix* null_matrix(unsigned int m, unsigned int n);


int check_initial_state(matrix* f, matrix* G)
{
	/*TODO
			Testen, ob ZU BEGINN kein Prozess mehr Resourcen benÃ¶tigt, als insgesamt verfÃ¼gbar sind
			Wird nur genau einmal aufgerufen
	
			Muss nicht extra aufgerufen werden, das Ã¼bernimmt die main()
	*/

	// Berechnen des Vektors f-g_i -> Überprüfen, ob etwas kleiner 0 ist
	matrix *g = null_matrix(1, G->n), *f2;
	for (int i = 0; i < G->m; i++) {
		copy_row_in_matrix(g, 0, G, i);
		f2 = duplicate_matrix(f);

		subtract_matrix(f2, g);
		int erg = check_matrix_elements(f2);

		free_matrix(f2);

		if (erg == -1) {
			free_matrix(g);
			return 0;
		}
	}

	free_matrix(g);
	return 1;
}

int check_allocation(matrix* B, matrix* R, matrix* f, matrix* allocation, unsigned int ID)
{
	/*TODO:
			-Testen, ob Ã¼berhaupt genÃ¼gend Ressourcen vorhanden sind, um Anforderung zu erfÃ¼llen
			-"virtuelle Umgebung" fÃ¼r den Bankieralgorithmus schaffen
			-Bankieralgorithmus aufrufen
			-Bei Erfolg: Anforderung erfÃ¼llen und ggf. den Prozess 
			 beenden (Belegungen wieder freigeben, Restanforderung auf 0 setzen)
			->Ansonsten den Prozess "blockieren" (0 zurÃ¼ckgeben. Das Blockieren wird dann von der main() Ã¼bernommen)
			-Erinnerung: dynamisch reservierter Speicher muss wieder freigegeben werden!
	*/

	// virtuelle Umgebung
	matrix* B2 = duplicate_matrix(B);
	matrix* R2 = duplicate_matrix(R);
	matrix* f2 = duplicate_matrix(f);

	// Anforderungen ausführen
	matrix* temp = create_matrix_from_row(B->m, ID, allocation);
	add_matrix(B2, temp);
	subtract_matrix(R2, temp);
	subtract_matrix(f2, allocation);

	free_matrix(temp);

	// auf genügende Ressourcen testen
	if (check_matrix_elements(f2) == -1) {
		free_matrix(B2);
		free_matrix(R2);
		free_matrix(f2);

		return 0;
	}

	// Bankieralgorithmus ausführen
	if (check_state(B2, R2, f2)) {
		// Allokation passt -> B, R, f anpassen
		// Nullmatrizen erzeugen zum Überschreiben
		subtract_matrix(B, B);
		subtract_matrix(R, R);
		subtract_matrix(f, f);

		// Überschreiben
		add_matrix(B, B2);
		add_matrix(R, R2);
		add_matrix(f, f2);

		free_matrix(B2);
		free_matrix(R2);
		free_matrix(f2);

		return 1;
	}

	free_matrix(B2);
	free_matrix(R2);
	free_matrix(f2);
	return 0;
}

int check_state(matrix* B, matrix* R, matrix* f)
{
	matrix *B2, *R2;
	matrix* f2 = duplicate_matrix(f);

	// blokierte Prozesse aus der etrachtung entfernen
	// ==> Zeilen der blokierten Prozesse aus B und R löschen

	int n = B->n;
	int m = 0; // Anzahl der nicht blockierten Prozesse
	for (int i = 0; i < B->m; i++) {
		m += 1 - blocked_processes[i];
	}

	if (m == B->m) { // kein Prozess ist blockiert
		B2 = duplicate_matrix(B);
		R2 = duplicate_matrix(R);
	}
	else {
		B2 = null_matrix(m, n);
		R2 = null_matrix(m, n);
		int zaehler = 0; // bisher abgespeicherte, nicht blockierte Prozesse
		for (int i = 0; i < B->m; i++) {
			if (blocked_processes[i] == 0) {
				copy_row_in_matrix(B2, zaehler, B, i);
				copy_row_in_matrix(R2, zaehler, R, i);
				zaehler ++;
			}
		}
	}


	// Bankier-Algorithmus mit nicht blockierten Prozessen

	matrix *r = null_matrix(1, n), *tempF = null_matrix(1, n), *b = null_matrix(1, n);
	int running = 1;
	int result = -1; //-1: noch kein Ergebnis; 0: Verklemmung; 1: keine Verklemmung

	while (running) {
		// Prozessanforderungen durchsuchen nach lauffähigen Prozess
		unsigned int P = -1; // lauffähiger Prozess; wenn kein lauffähiger Prozess, dann -1

		for (int i = 0; i < B2->m; i++) {
			copy_row_in_matrix(r, 0, R2, i);					// bestehenden Vektor überschreiben
			copy_row_in_matrix(b, 0, B2, i);					// bestehenden Vektor überschreiben
			int score1 = check_matrix_elements(r);
			int score2 = check_matrix_elements(b);

			if (score1 == 1 || (score1 == 0 && score2 == 1)) {// keinen Prozess wählen, der schon fertig ist (keine Restanforderungen UND Belegungen mehr hat)
				copy_row_in_matrix(tempF, 0, f2, 0);					// bestehenden Vektor überschreiben

				subtract_matrix(tempF, r);		// f-r_i

				int erg = check_matrix_elements(tempF);

				if (erg != -1) {
					P = i;
					break;
				}
			}
		}

		if (P == -1) {	// kein Prozess ist lauffähig
			result = 0;
			running = 0;
			//printf("kein Prozess lauffähig!\n");
			break;
		}

		// Prozess P wird ausgeführt und gibt Belegungen frei
		copy_row_in_matrix(b, 0, B2, P);					// bestehenden Vektor überschreiben
		matrix *temp = create_matrix_from_row(B2->m, P, b);

		add_matrix(f2, b);
		subtract_matrix(B2, temp);

		free_matrix(temp);

		copy_row_in_matrix(r, 0, R2, P);					// bestehenden Vektor überschreiben
		temp = create_matrix_from_row(R2->m, P, r);

		subtract_matrix(R2, temp);

		free_matrix(temp);


		// wenn alle Restanforderungen abgearbeitet sind, ist die Allokation sicher!
		if (check_matrix_elements(R2) == 0) {
			result = 1;
			running = 0;
		}
	}

	// Überprüfen, ob ein Prozess keine Restanforderungen mehr hat
	// ==> Belegungen freigeben
	for (int i = 0; i < R->m; i++) {
		copy_row_in_matrix(r, 0, R, i);					// bestehenden Vektor überschreiben
		if (check_matrix_elements(r) == 0) {
			copy_row_in_matrix(b, 0, B, i);
			add_matrix(f, b);

			matrix *temp = create_matrix_from_row(B->m, i, b);
			subtract_matrix(B, temp);
			free_matrix(temp);
		}
	}


	free_matrix(b);
	free_matrix(r);
	free_matrix(tempF);
	free_matrix(B2);
	free_matrix(R2);
	free_matrix(f2);

	return result;
}

//===============================================================================

/*TODO:
	Hier koennen bei Bedarf eigene Funktionen implementiert werden
*/

void print_dimensions(matrix* m, char c) {
	printf("%c:\t%d x%d\n", c, m->m, m->n);
}

// kopiert die ms-te Zeile von source in die mt-te Zeile von Target
void copy_row_in_matrix(matrix* target, unsigned int mt, matrix* source, unsigned int ms) {
	if (target != NULL && source != NULL && target->n == source->n && mt < target->m && ms < source->m) {
		int n = target->n;
		for (int i = 0; i < n; i++) {
			target->elements[mt*n+i] = source->elements[ms*n+i];
		}
	}
}

matrix* null_matrix(unsigned int m, unsigned int n) {
	matrix* mat = malloc(sizeof(matrix));
	mat->m = m;
	mat->n = n;
	mat->elements = calloc(m*n, sizeof(int));
	return mat;
}

//===============================================================================

int abs(int n)
{
	return (n>0) ? n : -n;
}

void print_BRf(matrix* B, matrix* R, matrix* f)
{
	unsigned int m = B->m;
	unsigned int n = B->n;

	if(B==NULL || R==NULL || f==NULL)
	{
		printf("ERROR: print_BRf() | given matrices = NULL.\n");
		return;
	}

	if(f->m != 1 || f->n != n || R->m !=m || R->n != n)
	{
		print_dimensions(B, 'B');
		print_dimensions(R, 'R');
		print_dimensions(f, 'f');
		printf("ERROR: print_BRf() | given matrices dont have matching dimensions.\n");
		return;
	}

	int max_B = get_elem_of_matrix(B, 0,0);		//largest elements
	int max_R = get_elem_of_matrix(R, 0,0);
	int max_f = get_elem_of_matrix(f, 0,0);

	int grade_B = 1;	//represents the number of digits of the largest element
	int grade_R = 1;
	int grade_f = 1;

	int width_B;
	int width_R;
	int width_f;


	for(unsigned int i = 0; i< B->m; i++)
	{
		for(unsigned int j = 0; j < B->n; j++)
		{
			int current = get_elem_of_matrix(B, i,j);
			max_B = (abs(current)>max_B) ? current : max_B;
		}
	}

	for(unsigned int i = 0; i< R->m; i++)
	{
		for(unsigned int j = 0; j < R->n; j++)
		{
			int current = get_elem_of_matrix(R, i,j);
			max_R = (abs(current)>max_R) ? current : max_R;
		}
	}

	for(unsigned int j = 0; j < f->n; j++)
	{
		int current = get_elem_of_matrix(f, 0,j);
		max_f = (abs(current)>max_f) ? current : max_f;
	}

	if(max_B < 0) grade_B++;
	while(max_B > 9 || max_B < -9)
	{
		max_B/=10;
		grade_B++;
	}
	width_B = 3 + n*(grade_B+1);

	if(max_R < 0) grade_R++;
	while(max_R > 9 || max_R < -9)
	{
		max_R/=10;
		grade_R++;
	}
	width_R = 3 + n*(grade_R+1);

	if(max_f < 0) grade_f++;
	while(max_f > 9 || max_f < -9)
	{
		max_f/=10;
		grade_f++;
	}
	width_f = 3 + n*(grade_f+1);

	//print upper line (B: R: f:)
	for(int i = 0; i < width_B + width_R + width_f + 2*3; i++)
	{
		if(i==(int)((width_B-1)/2)) printf("B");
		else if(i==(int)(width_B + 3 + (width_R-1)/2)) printf("R");
		else if(i==(int)(width_B + 3 + width_R + (width_f-1)/2)) printf("f");
		else printf(" ");
	}
	printf("\n");

	//================================ print B ================================= 
	//prints upper line
	printf(" ");
	for(int i = 0; i< width_B-2; i++) 	printf("-");

	printf("\n");

	//prints body
	for(unsigned int i = 0; i< m; i++)
	{
		printf("| ");
		for(unsigned int j = 0; j < n; j++)
		{
			int elem = get_elem_of_matrix(B, i, j);
			if(blocked_processes[i]) printf("\033[1;31m%*i\033[0m ", grade_B, elem);
			else if(elem < 0) printf("\033[1;33m%*i\033[0m ", grade_B, elem);
			else 		 printf("%*i ", grade_B, elem);
			
		}
		printf("|\n");
	}

	//prints lower line
	printf(" ");
	for(int i = 0; i< width_B-2; i++)	printf("-");
	
	printf("\r\033[%iC\033[%iA", (width_B+3), (m+1));	//move curser forwards and upwards

	//================================ print R ================================= 
	//prints upper line
	printf(" ");
	for(int i = 0; i< width_R-2; i++) 	printf("-");

	printf("\033[%iD\033[%iB", (width_R-1), 1);		//move curser backwards and downwards

	//prints body
	for(int i = 0; i< m; i++)
	{
		printf("| ");
		for(int j = 0; j < n; j++)
		{
			int elem = get_elem_of_matrix(R, i, j);
			if(blocked_processes[i]) printf("\033[1;31m%*i\033[0m ", grade_R, elem);
			else if(elem < 0) printf("\033[1;33m%*i\033[0m ", grade_R, elem);
			else 		 printf("%*i ", grade_R, elem);
		}
		printf("|\033[%iD\033[%iB", width_R, 1);	//move curser backwards and downwards
	}

	//prints lower line
	printf(" ");
	for(int i = 0; i< width_R-2; i++)	printf("-");
	
	printf("\033[%iC\033[%iA", 4, (int)(m/2 +2));		//move curser forwards and upwards


	//================================ print f ================================= 
	//prints upper line
	printf(" ");
	for(int i = 0; i< width_f-2; i++) 	printf("-");

	printf("\033[%iD\033[%iB", (width_f-1), 1);		//move curser backwards and downwards

	//prints body
	printf("| ");
	for(int j = 0; j < n; j++)
	{
		int elem = get_elem_of_matrix(f, 0, j);
		if(elem < 0) printf("\033[1;33%*i\033[0m ", grade_f, elem);
		else 		 printf("%*i ", grade_f, elem);
	}
	printf("|\033[%iD\033[%iB", width_f, 1);	//move curser backwards and downwards

	//prints lower line
	printf(" ");
	for(int i = 0; i< width_f-2; i++)	printf("-");
	
	printf("\r\033[%iB\n\n", (int)(m/2 +1));	//move curser backwards and downwards

}
