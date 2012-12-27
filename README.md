PEMSY
=====

Code fuer PEMSY-Praktikum

Letzte Änderungen
-----------------
tastatur.c 
- vom Grund auf neu konzipiert
- Escape Taste löscht LCD und setzt Cursor auf 0,0
lcd.c 
- lcd_clear() um LCD Anzeige zu löschen und Cursor auf 0,0 zurückzusetzen
- lcd_get_cursor() Cursor Position auslesen (work in progress)

To Do
-----
lcd.c
- Automatischer Zeilenumbruch (über lcd_get_cursor, lcd_locate)
tastatur.c
- Enter Taste für manuellen Zeilenumbruch (lässt momentan seltsames Zeichen anzeigen)
- Backspace Taste um letztes Zeichen zu löschen (lcd_get_cursor, lcd_locate)
