#include <fstream.h>
#include <string.h>

#include "../../src/string.h"

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
      
      if (line.length() < 1) {
	 // an empty line
	 cout << endl;
      } else if (!strncasecmp("Question: ",(const char *)line,10)) {
	 // a question
	 count++;
	 cout << "# Question " << count << endl << ":" << 
	   line.subString(10) << endl;
      } else if (!strncasecmp("Answer: ",(const char *)line,8)) {
	 // an answer
	 cout << line.subString(8) << endl;
      } else {
	 // Dunno about this line.. write it as a comment
	 cout << "#??" << line << endl;
      }
   }
   
   // Close file
   file.close();
   
   return 0;
}
