String GetRidOfurlCharacters(String input)
{
  
input.replace("%0D%0A", String('\n'));
input.replace("+",   " ");

char data[input.length()];
input.toCharArray(data, input.length() + 1);

// Create two pointers that point to the start of the data
char *leader = data;
char *follower = leader;

// While we're not at the end of the string (current character not NULL)
while (*leader) {
    // Check to see if the current character is a %
    if (*leader == '%') {

        // Grab the next two characters and move leader forwards
        leader++;
        char high = *leader;
        leader++;
        char low = *leader;

        // Convert ASCII 0-9A-F to a value 0-15
        if (high > 0x39) high -= 7;
        high &= 0x0f;

        // Same again for the low byte:
        if (low > 0x39) low -= 7;
        low &= 0x0f;

        // Combine the two into a single byte and store in follower:
        *follower = (high << 4) | low;
    } else {
        // All other characters copy verbatim
        *follower = *leader;
    }

    // Move both pointers to the next character:
    leader++;
    follower++;
}
// Terminate the new string with a NULL character to trim it off
*follower = 0;
  return String(data);
}
