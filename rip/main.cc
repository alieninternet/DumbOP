#include <fstream.h>
#include <string.h>

#include "../src/str.h"
#include "../src/stringtokenizer.h"

int main(int argc, char **argv)
{
   unsigned long count = 0;
   String line = "";
   
   if (argc != 2) {
      cout << "# No input" << endl;
      return 1;
   }
   
   cout << "# Reading from " << argv[1] << endl << "#" << endl;
   
   // Open file
   ifstream file(argv[1]);
   
   // Make sure it opened
   if (!file) {
      cout << "# Could not open file. Aborting" << endl;
      return 1;
   }
   
   // Run through the file
   while (!file.eof()) {
      // Read a line
      file >> line;
      
      // Skip empty line
      if (line.length() < 1) {
	 continue;
      }
      
      // Do it
      StringTokenizer st(line);
      String name = st.nextToken(':');
      String code = st.nextToken(':');
      String number = st.nextToken(':');
      String altname = st.rest().toLower();
      
      cout << endl << endl << "#" << endl << "# " << number << ": " << 
	name << " (" << code << ")" << endl << "#" << endl;
      
      count++;
      cout << endl;
      cout << "# Question " << count << endl;
      cout << ":What is the symbol for the element " << name << "?" << endl;
      cout << code << endl;
      
      count++;
      cout << endl;
      cout << "# Question " << count << endl;
      cout << ":What is the element name associated with the symbol '" << 
	code << "'?" << endl;
      cout << name << endl;
      if (altname.length()) {
	 cout << altname << endl;
      }

      count++;
      cout << endl;
      cout << "# Question " << count << endl;
      cout << ":What is the name of element number " << number << 
	" on the periodic table?" << endl;
      cout << name << endl;
      if (altname.length()) {
	 cout << altname << endl;
      }

      count++;
      cout << endl;
      cout << "# Question " << count << endl;
      cout << ":What is the number of the element " << name << 
	" on the periodic table?" << endl;
      cout << number << endl;
   }

   // Write two empty lines
   cout << endl << endl;
   
   // Close file
   file.close();
   
   return 0;
}
