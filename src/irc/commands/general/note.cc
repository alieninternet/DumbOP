
#include "config.h"
#include "commands.h"
#include "flags.h"
#include "utils.h"

/* Note - Wrapper into the note interface
 */
void Commands::Note(Person *from, String channel, String rest)
{
   from->sendLine("The note interface is not operational.");
}
