/*
 * float_to_string.h
 *
 * Created: 18/04/2023 5:57:11 PM
 *  Author: s4720477
 */ 


#ifndef FLOAT_TO_STRING_H_
#define FLOAT_TO_STRING_H_

// This function is used in the int_to_string() function
void reverse(char* str, int len)
{
	int i = 0, j = len - 1, temp;
	while (i < j) {
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++;
		j--;
	}
}

// this function is used in the float_to_string() function
int int_to_string(int x, char str[], int d)
{
	int i = 0;
	while (x) {
		str[i++] = (x % 10) + '0';
		x = x / 10;
	}
	
	// If number of digits required is more, then
	// add 0s at the beginning
	while (i < d)
	str[i++] = '0';
	
	reverse(str, i);
	str[i] = '\0';
	return i;
}

//converts a float to a string, currently used for displaying temp value.
void float_to_string(float x, char* buffer, int afterpoint) {
	// Extract integer part
	int ipart = (int)x;
	
	// Extract floating part
	float fpart = x - (float)ipart;
	
	// convert integer part to string
	int i = int_to_string(ipart, buffer, 0);
	
	// check for display option after point
	if (afterpoint != 0) {
		buffer[i] = '.'; // add dot
		
		// Get the value of fraction part upto given no.
		// of points after dot. The third parameter
		// is needed to handle cases like 233.007
		fpart = fpart * pow(10, afterpoint);
		
		int_to_string((int)fpart, buffer + i + 1, afterpoint);
	}
}



#endif /* FLOAT_TO_STRING_H_ */