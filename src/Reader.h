/******************************************************************************
 * SUDOKU ./reader.c                                                          *
 * Einlesefunktion für Textdateien mit Sudoku Rätseln gemäß der Vorgaben      *
 * Version 2 (2016-01-29) Sascha Scherrer <dhbw.scherrer@gmail.com>           *
 ******************************************************************************/


/*
 * Hilfsfunktion rh_resetZaehler(int zaehler[]);
 * Setzt alle Werte von zaehler (muss Übergeben werden) auf 0.
 */
void rh_resetZaehler(int zaehler[]) {
	int i;
	for (i = 0; i <= MAX_ZAHL; i++)
		zaehler[i] = 0;
}

/*
 * Hilfsfunktion rh_inkrementZaehler(int[] zaehler, int wert)
 * Arbeitet auf dem Array, in dem alle zu zählenden Werte enthalten sind.
 * Ist der Wert im Array enthalten, wird er um eins erhöht.
 * int[] zaehler - array zum Zählen. Jedes Element entspricht einem Wert
 * int wert - der wert, der erhöht wird (Bereich: 0 bis MAX_ZAHL)
 * return 0 - wenn etwas schief gegangen ist
 * return n - die neue Anzahl (n) des betreffenden Wertes (wert)
 */
int rh_inkrementZaehler(int zaehler[], int wert) {
	if (wert >= 0 && wert <= MAX_ZAHL)
		return ++zaehler[wert];
	else {
		/* Unerwartetes Zeichen */
		return 0;
	}
}

/*
 * Hilfsfunktion  rh_leseDateiZeichenweise(int feld[][], FILE ptr_file)
 * Liest eine Datei Zeichen für zeichen ein, und versucht daraus ein Sudoku
 * zu erstellen.
 * Hinweis: Das einlesen erfolt ungepuffert. Diese Vorhgehensweise ist langsamer als
 *    eine gepufferte Eingabe und sollte daher nicht für große Dateien verwendet werden.
 *    Da wir bei den Sudoku-Dateien von 90 Zeichen (81 Werte und 9 Zeilenumbrüche) ausgegangen,
 *    werden kann, sind die Dateien recht klein, sodass durch das auslassen von Puffern keine
 *    spürbaren verzögerungen zu erwarten sind.
 * Funktionen:
 * - Leerzeichen werden durch "0" (Null) repräsentiert.
 * - Fehlen Zeichen für eine vollständige Reihe, wird die Reihe mit Nullen aufgefÃ¼llt.
 * - Alle Zeichen, die keine gültiges Zeichen sind, werden ignoriert.
 *   Gültige Zeichen sind die Ziffern 0 bis 9 und das Leerzeichen. 0 und Leerzeichen sind synonym.
 * - Alle Zeichen, die Über die erwartete Zeilenlänge hinausgehen werden ignoriert.
 * - Alle Zeilen, die nach der erwarteten Zeilenzahl folgen, werden ignoriert.
 * - Unerwartete Zeichen im Sudoku werden als unbekannte Ziffer (0 oder Leerzeichen) behandelt.
 * - Leerzeilen werden ignoriert
 *   Das bedeutet, dass alle Zeilen, die weniger als zwei gültige Zeichen enthalten ignoriert werden.
 * returns 0 - wenn keine Fehler aufgetreten sind
 * returns n - Anzahl der Fehler, die gefunden und korrigiert wurden.
 */
int rh_leseDateiZeichenweise(int **feld, FILE *ptr_file) {
	int posX = 0, posY = 0, anzahlZeichen = 0, zeichen = 0, fehler = 0,
			zahl = 0;

	while ((zeichen = fgetc(ptr_file)) != EOF) {
		anzahlZeichen++;

		if (posX < BREITE && posY < HOEHE) {
			/* Zeichen aus Datei in feld Übersetzen */
			if (zeichen == ' ')
				zahl = 0; // Lerrzeichen -> 0
			else if (zeichen >= '0' && zeichen <= '9')
				zahl = zeichen - '0'; // Ziffer -> Ziffer
			else if (MAX_ZAHL >= 10) {
				// Buchstaben (Basis MAX_ZAHL) -> Zahl (Basis 10)
				if (zeichen >= 'A' && zeichen <= 'Z')
					zahl = zeichen - 'A' + 10;
				else if (zeichen >= 'a' && zeichen <= 'z')
					zahl = zeichen - 'a' + 10;
				else
					fehler++;
			}

			// an richtige Feldposition setzen
			feld[posX++][posY] = fehler ? 0 : zahl;

			// Fehler, wenn Zahl größer als maximal erlaubte Zahl ist
			if (zahl > MAX_ZAHL)
				fehler++;

		} else if (zeichen == '\n') {
			/* Zeilenumbrüche behandeln */
			if (posX < 1) {
				/* Zeile wird als irrelevant betrachtet, da sie keine gültigen Zeichen enthält */
				fehler++;
			} else {
				if (posX != BREITE) {
					fehler++;
					/* Falsche Anzahl an zeichen korrigieren */
					while (posX < BREITE && posY < HOEHE) {
						feld[posX++][posY] = 0;
					}
				}

				/* Zähler bearbeiten */
				posY++;
				posX = 0;
			}
		} else {
			fehler++;
		}
	}

	return fehler;
}

/* 
 * readFile(char *pfad)
 * Liest eine Datei ein, die ein Sudoku-Rätsel enthalten sollte.
 * Ruft Funktionen auf, die die Arbeit machen.
 * Getestet mit Textdateien in utf8-Codierung auf Kubuntu 14.04LTS
 * pfad - Pfad zur Datei
 * returns  0 - Wenn keine Fehler aufgetreten sind.
 * returns -1 - Wenn Fehler auftraten, die nicht korrigiert werden konnten oder sollten.
 * returns -2 - Wenn der Dateihandle nicht geöffnet werden konnte.
 * returns  n - Anzahl der Fehler (n), die korrigiert wurden.
 */
int leseDatei(char *dateipfad) {
	/* Dateihandle Öffnen */
	FILE *ptr_datei;
	ptr_datei = fopen(dateipfad, "r");
	if (!ptr_datei)
		return -2;

	/* Datei in array einlesen */
	int **einleseFeld = allokieren(HOEHE, BREITE);
	int einleseStatus = rh_leseDateiZeichenweise(einleseFeld, ptr_datei);
	fclose(ptr_datei);

	if (einleseStatus < 0) {
		return einleseStatus;
	}

	/* Eingabe validieren */
	int validierungsStatus = testSudokuFormal(einleseFeld);
	if (validierungsStatus) {
		// Es gibt formale Fehler
	}

	/* Feld übertragen */
	// Fragen, ob das ok ist, wenn Fehler korrigiert wurden
	// Array übertragen (echte Kopie, nicht über Pointer)
	int i, j;
	for (i = 0; i < BREITE; i++) {
		for (j = 0; j < HOEHE; j++) {
			schutz[i][j] = 0;
		}
	}
	for (i = 0; i < BREITE; i++) {
		for (j = 0; j < HOEHE; j++) {
			setFeld(i, j, einleseFeld[j][i], einleseFeld[j][i]);
		}
	}

	/* Speicher freigeben */
	for (i = 0; i < HOEHE; i++) {
		free(einleseFeld[i]);
	}
	free(einleseFeld);

	return einleseStatus;
}

