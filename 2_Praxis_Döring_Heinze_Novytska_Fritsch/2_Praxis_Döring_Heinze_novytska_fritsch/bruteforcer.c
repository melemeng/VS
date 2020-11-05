#include "bruteforcer.h"

// eigene Funktion
int index_free_worker();

int main(int argc, char *argv[]) {
	// Argumente checken
	if (argc != 4) {
		fprintf(stderr, "usage: %s pwdmaxlen workers hashfile\n", argv[0]);
		exit(1);
	}
	
	char *filename = NULL;
	hashes *loaded_hashes = NULL;
	
	// Kommandozeilenargumente auslesen und globale und lokale Variablen füllen
	pwd_maxlen = atoi(argv[1]);
	max_workers = atoi(argv[2]);
	filename = argv[3];
	
	worker = malloc(max_workers * sizeof(pid_t));
	// W�re mit calloc einfacher gegangen, malloc ist aber direkt gefordert...
	for (int i = 0; i < max_workers; i++) {
		worker[i] = 0;
	}
	
	INFO("\nBRUTEFORCER GESTARTET\n");
	INFO("---------------------\n");
	INFO("Maximale Passwortlänge: %d\n", pwd_maxlen);
	INFO("Anzahl der Worker: %d\n", max_workers);
	INFO("Hashes lesen aus Datei: %s\n", filename);
	INFO("---------------------\n\n");

	// Hashes in ein hashes struct laden
	loaded_hashes = load_hashes(filename);
	
	// Kontrollvariable des Prozess-"Rangs" (Parent/Child) f�rs aufr�umen
	int isChild = 0;	// 0: Parent; 1: Child

	// Main loop -> Iteriert über alle Hashes
	for (int i = 0; i < loaded_hashes->len; i++) {
		char *hash = loaded_hashes->array[i];
		

		// --------------------------------------------------------------------
		// 								AUFGABE B
		// --------------------------------------------------------------------
		/*
		// Hash mit crack_hash versuchen zu knacken
		pwd * password= crack_hash(hash);
		
		// Erfolg? -> print password
		// Fehlgeschlagen? -> Einfach weiter in der Schleife
		if (password != NULL) {
			printf("%s\n", password->buf);

			// password freigeben
			free_password(password);
		}
		*/

		// --------------------------------------------------------------------
		// 								AUFGABE D
		// --------------------------------------------------------------------

		while (max_workers - update_worker() <= 0);		// Warten, solange kein Worker frei ist

		int index = index_free_worker();				// freien Worker suchen

		if (index != -1) {
			INFO("Worker %d bearbeitet Hash Nr. %d\n", index, i);
			pid_t pid = fork();

			if (pid > 0) {								// Parent
				isChild  = 0;
				worker[index] = pid;
			}
			else if (pid == 0) {						// Child
				isChild = 1;

				// Hash mit crack_hash versuchen zu knacken
				pwd * password= crack_hash(hash);

				// Erfolg? -> print password
				// Fehlgeschlagen? -> Einfach weiter in der Schleife


				if (password != NULL) {
					printf("%s\n", password->buf);

					// password freigeben
					free_password(password);
				}

				break;									// Child beenden
			}
		}

	}
	
	if (isChild == 0) {
		// auf Childs warten
		while (wait(NULL) > 0);
	}

	// Aufräumen und beenden
	free_hashes(loaded_hashes);
	free(worker);
	
	return 0;
}

// Versucht den Hash zu cracken, indem systematisch Passwörter generiert werden
// und darauf getestet wird, ob deren Hash mit hash übereinstimmt
// Returns pwd or NULL if no password was found
pwd *crack_hash(char *hash) {
	// Mit new_password() ein leeres Passwort anlegen
	pwd *password = new_password(pwd_maxlen);
	
	// Mit test_string() überprüfen, ob das (zuerst leere) Passwort zum Hash passt
	// In einer Schleife next_password() aufrufen, und das nächste Passwort überprüfen
	// Schleifenabbruch, sobald next_password() 0 zurückgibt => es gibt kein weiteres Passwort,
	// d.h. alle Passwörter bis zur maximalen Länge wurden bereits generiert und überprüft
	// ODER
	// Schleifenabbruch, wenn das Passwort gefunden wurde
	
	// Zeigt an, ob das Passwort gefunden wurde
	char found = 0;	// 0: nicht gefunden; 1: gefunden

	do {
		if (test_string(password->buf, hash) == 1) {
			// aktuelles Passwort passt
			found = 1;
			break;
		}
	} while (next_password(password) == 1);

	// Aufräumen
	// TODO
	
	// Passwort nicht gefunden -> NULL zurückgeben
	// Passwort gefunden -> das Password zurückgeben

	if (found == 1) return password;
	else {
		free_password(password);
		return NULL;
	}
}

// Berechnet den Hash von string und gibt 1 zurück, wenn er mit hash übereinstimmt, sonst 0
int test_string(char *string, char *hash) {
	char * pwd_hash = sha256(string);

	int cmp = strcmp(pwd_hash, hash);
	free(pwd_hash);

	if (cmp == 0) return 1;
	else return 0;
}

/**
 * Diese Funktion soll alle worker Kindprozesse die in dem Array "worker"
 * gespeichert sind darauf überprüfen, ob diese noch laufen oder bereits
 * beendet sind. Wenn ein Prozess beendet wurde soll der Eintrag in dem Array
 * gelöscht werden (auf 0 setzen), so dass dieser wieder zur Verfügung steht.
 *
 * return value:
 *   Diese Funktion soll die Anzahl der momentan im Hintergrund laufenden
 *   Prozesse zurückgeben. Prozesse die beendet wurden zählen nicht dazu.
 */
int update_worker() {
	int n = 0;
	pid_t buf;

	for (int i = 0; i < max_workers; i++) {
		if (worker[i] != 0) {					// wenn kein Prozess angelegt ist, muss er auch nicht untersucht werden
			buf = waitpid(worker[i], NULL, WNOHANG);

			if (buf == 0) n++;					// child aktiv
			else worker[i] = 0;	// kein aktiver Prozess zu finden
		}
	}
	
	return n;
}

/**
 * Diese Funktion sucht nach freien Stellen im Array "worker". Eine freie Stelle
 * ist gekennzeichnet dadurch, dass ihr Wert 0 ist.
 *
 * return value:
 * 	 Gibt den Index der ersten freien Stelle zur�ck. Wenn keine freie Stelle vorhanden ist,
 * 	 wird -1 zur�ckgegeben.
 */
int index_free_worker() {
	int retVal = -1; 							// Defaultwert ist -1 falls keine freie Stelle gefunden wird

	for (int i = 0; i < max_workers; i++) {
		if (worker[i] == 0) {					// Suche nach Stellen, wo die PID 0 ist
			retVal = i;
			break;
		}
	}

	return retVal;
}

