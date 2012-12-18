// liefert Laenge eines Strings zurueck
// Aufruf mit: l = strlen(&wort)

uint8_t strlen(char *s)
{
	for(uint8_t i = 0; *s != '\0'; s++)
	{
		i++;
	}
	return i;
}