#include <fstream.h>
#include <string.h>

#include "../src/string.h"
#include "../src/stringtokenizer.h"

int main(int argc, char **argv)
{
   unsigned long count = 0;
   String line = "";
   
   if (argc != 2) {
      cout << "# No input" << endl;
      return 1;
   }
   
   cout << "# Reading from " << argv[1] << endl;
   
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
      String question = st.nextToken(':');
      String answer = st.rest().toLower();
      
      count++;
      cout << endl;
      cout << "# Question " << count << endl;
      cout << ":" << 
	String(question[0]).toUpper() << question.subString(1) << "?" << endl;
      cout << answer << endl;
   }

   // Write two empty lines
   cout << endl << endl;
   
   // Close file
   file.close();
   
   return 0;
}
